#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "matrix-cal.h"

int X,Y,T;
FILE *fp1, *fp2;
double ***map1, ***map2;

double gen_rand(double min, double max){
    double t = (double)rand() / RAND_MAX;
    return min + t * (max - min);
}

arg_step create_arg_step(double ***map, int old, int st_x, 
                           int st_y, int ed_x, int ed_y, int times){
    arg_step ans= Malloc(struct _arg_step, 1);
    ans->map = map;
    ans->old = old;
    ans->st_x = st_x;
    ans->st_y = st_y;
    ans->ed_x = ed_x;
    ans->ed_y = ed_y;
    ans->times = times;
    return ans;
}

arg_step3 create_arg_step3(double ***map, int old, int st_x, 
                           int st_y, int times){
    arg_step3 ans= Malloc(struct _arg_step3, 1);
    ans->map = map;
    ans->old = old;
    ans->st_x = st_x;
    ans->st_y = st_y;
    ans->times = times;
    return ans;
}

void init(double min, double max){
    map1 = Malloc(double **, R);
    map2 = Malloc(double **, R);
    int i,j,t;
    for (t=0;t<R;t++){
        map1[t] = Malloc(double*, X+2);
        map2[t] = Malloc(double*, X+2);
    }
    for (t=0;t<R;t++)
        for (i=0;i<X+2;i++){
            map1[t][i] = Malloc(double, Y+2);
            map2[t][i] = Malloc(double, Y+2);
            memset(map1[t][i], 0, sizeof(map1[t][i]));
            memset(map2[t][i], 0, sizeof(map2[t][i]));
        }
    for (i=1;i<=X;i++)
        for (j=1;j<=Y;j++){
            map1[0][i][j] = gen_rand((double)min, (double)max);
            map2[0][i][j] = map1[0][i][j];
        }
    if ((fp1 = fopen("map1.txt","w")) == NULL){
        printf("init: Can't create file.\n");
        exit(0);
    }
    if ((fp2 = fopen("map2.txt","w")) == NULL){
        printf("init: Can't create file.\n");
        exit(0);
    }
}

double cal(double** m, int x, int y){
    return fmod(m[x-1][y] * 1.25 - m[x+1][y] * 2.5 +
                m[x][y-1] * 1.25 - m[x][y+1] * 2.5 +
                m[x][y] * 2, (double)BIG_PRIME);
    //return (m[x-1][y]+m[x+1][y]+m[x][y-1]+m[x][y+1]+m[x][y]);
}

void print_map(double ***m, int t, int st_x, int st_y, int ed_x, int ed_y, FILE *fp){
    int i,j;
    for (i=st_x;i<=ed_x;i++){
        for (j=st_y;j<=ed_y;j++)
            fprintf(fp, "%lf ",m[t][i][j]);
        fprintf(fp, "\n");
    }
}

int func_norm(double ***m, int old, int st_x, int st_y, int ed_x, int ed_y, int times){
    int i,j,t;
    int now = old;
    for (t=0;t<times;t++){
        now = 1 - now;
        for (i=st_x;i<=ed_x;i++)
            for (j=st_y;j<=ed_y;j++)
                m[now][i][j]=cal(m[1-now], i, j);
    }
    return now;
}

void *func_div_step1(void *arg){
    int i,j,t;
    arg_step as = (arg_step)arg;
    int now = as->old;
    for (t=1;t <= as->times;t++){
        now = 1 - now;
        for (i=MIN(as->st_x + t, X); i<=MAX(as->ed_x - t, 1); i++)
            for (j=MIN(as->st_y + t, Y); j<=MAX(as->ed_y - t, 1); j++)
                if ((i<1)||(i>X)||(j<1)||(j>Y)) continue;
                else as->map[now][i][j]=cal(as->map[1-now], i, j);
    }
    free(as);
    return (void*)now;
}
void *func_div_step2(void *arg){
    int i,j,t;
    arg_step as = (arg_step)arg;
    int now = as->old;
    if (as->st_x == as->ed_x){
        for (t=0;t<as->times;t++){
            now = 1 - now;
            for (i=MAX(as->st_x - t,1); i<=MIN(as->ed_x + t, X); i++)
                for (j=MIN(as->st_y + (t+1), Y); j<=MAX(as->ed_y - (t+1), 1);j++)
                    if ((i<1)||(i>X)||(j<1)||(j>Y)) continue;
                    else as->map[now][i][j]=cal(as->map[1-now], i, j);
        }
    }
    else if (as->st_y == as->ed_y){
        for (t=0;t<as->times;t++){
            now = 1 - now;
            for (i=MIN(as->st_x + (t+1), X); i<=MAX(as->ed_x - (t+1), 1); i++)
                for (j=MAX(as->st_y - t, 1); j<=MIN(as->st_y + t, Y);j++)
                    if ((i<1)||(i>X)||(j<1)||(j>Y)) continue;
                    else as->map[now][i][j]=cal(as->map[1-now], i, j);
        }
    }else{
        fprintf(stderr,"func_div_step2: Input error.\n");
        exit(0);
    }
    free(as);
    return (void*)now;
}
void* func_div_step3(void *arg){
    int i,j,t;
    arg_step3 as = (arg_step3)arg;
    int now = as->old;
    for (t=0;t < as->times;t++){
        now = 1 - now;
        for (i=MAX(as->st_x - t,1);i<=MIN(as->st_x + t,X); i++)
            for (j=MAX(as->st_y - t,1);j<=MIN(as->st_y +t,Y);j++)
                if ((i<1)||(i>X)||(j<1)||(j>Y)) continue;
                else as->map[now][i][j]=cal(as->map[1-now], i, j);
    }
    free(as);
    return (void*)now;
}

int func_div(double ***m, int old, int st_x, int st_y, int ed_x, int ed_y, int times){
    int i,j,t;
    int now = old;
    int t_x = (ed_x - st_x + DX -1) / DX;
    int t_y = (ed_y - st_y + DY -1) / DY; 
    pthread_t thread[t_x+1][t_y+1];
    while(times > 0){
        int now2;
        if (times - DT >= 0){
            t = DT;
            times-=DT;
        }else{
            t = times;
            times = 0;
        }

        for (i=0;i<t_x;i++)
            for (j=0;j<t_y;j++){
                arg_step arg = create_arg_step(m, now, st_x + i*DX, st_y + j*DY, st_x + (i+1)*DX, st_y + (j+1)*DY, t);
                pthread_create(&thread[i][j], NULL, func_div_step1, arg); 
            }
        for (i=0;i<t_x;i++)
            for (j=0;j<t_y;j++){
                pthread_join(thread[i][j],(void*)&now2);
            }
        
        for (i=0;i<=t_x;i++)
            for (j=0;j<t_y;j++){
                arg_step arg = create_arg_step(m, now, st_x + i*DX, st_y + j*DY, st_x + i*DX, st_y + (j+1)*DY, t);
                pthread_create(&thread[i][j], NULL, func_div_step2, arg); 
            }
        for (i=0;i<=t_x;i++)
            for (j=0;j<t_y;j++)
                pthread_join(thread[i][j],(void*)&now2);
        
        for (i=0;i<t_x;i++)
            for (j=0;j<=t_y;j++){
                arg_step arg = create_arg_step(m, now, st_x + i*DX, st_y + j*DY, st_x + (i+1)*DX, st_y + j*DY, t);
                pthread_create(&thread[i][j], NULL, func_div_step2, arg); 
            }
        for (i=0;i<t_x;i++)
            for (j=0;j<=t_y;j++)
                pthread_join(thread[i][j],(void*)&now2);

        for (i=0;i<=t_x;i++)
            for (j=0;j<=t_y;j++){
                arg_step3 arg = create_arg_step3(m, now, st_x + i*DX, st_y + j*DY, t);
                pthread_create(&thread[i][j], NULL, func_div_step3, arg); 
            }
        for (i=0;i<=t_x;i++)
            for (j=0;j<=t_y;j++){
                pthread_join(thread[i][j],(void*)&now2);
            }
        now = now2;
    }
    return now;
}
int main(int argc, char **argv){
    if (argc!=3){
        X = 100;
        Y = 100;
        T = 1000;
    }else{
        X = atoi(argv[1]);
        Y = atoi(argv[2]);
        T = atoi(argv[3]);
    }
    init((double)0, (double)1000);
    int now1 = func_norm(map1, 0, 1, 1, X, Y, T);
    int now2 = func_div(map2, 0, 0, 0, X, Y, T);
    print_map(map1, now1, 1, 1, X, Y, fp1);
    print_map(map2, now2, 1, 1, X, Y, fp2);
    fclose(fp1);
    fclose(fp2);
    return 0;
}
