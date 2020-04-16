/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED

int N_ITEMS;
#define N_THREADS 8

pthread_t THREAD[N_THREADS];

typedef struct
{
    object * O;
    int START;
    int END;
}
targs;

targs TARGS[N_THREADS];

typedef struct
{
    object * O;
    unsigned char radius;
    int START;
    int END;
}
targs_box;

targs_box TARGS_B[N_THREADS];

typedef struct
{
    object * O;
    unsigned char radius;
    int L;
    int START;
    int END;
}
targs_box_L;

targs_box_L TARGS_L[N_THREADS];

typedef struct
{
    object * O;
    int L;
    int START;
    int END;
}
targs_box_T;

targs_box_T TARGS_T[N_THREADS];

#endif // THREADING_H_INCLUDED
