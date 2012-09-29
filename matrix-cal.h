#ifndef _MATRIX_CAL_H
#define _MAXTRIX_CAL_H

#define R 2
#define DX 20
#define DY 20
#define DT 10 
#define BIG_PRIME 29927

#define Malloc(type, n) (type *)malloc((n)*sizeof(type))
#define MIN(a,b) \
        ({ __typeof__ (a) _a = (a); \
                     __typeof__ (b) _b = (b); \
                 _a > _b ? _b : _a; })
#define MAX(a,b) \
        ({ __typeof__ (a) _a = (a); \
                     __typeof__ (b) _b = (b); \
                 _a > _b ? _a : _b; })
struct _arg_step{
    double  ***map;
    int     old;
    int     st_x, st_y, ed_x, ed_y;
    int     times;
};
struct _arg_step3{
    double  ***map;
    int     old;
    int     st_x, st_y;
    int     times;
};
typedef struct _arg_step *arg_step;
typedef struct _arg_step3 *arg_step3;
#endif
