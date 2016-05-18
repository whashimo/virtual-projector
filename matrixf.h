/* matrix.h */

/* ind_mat(float *m, int v)                            */
/* return M(matrix V x V) Identity                      */
void ind_mat(float *, int);

/* print_mat(float *m, int v)                          */
/* print M(matrix V x V)                                */
void print_mat(float *, int);

/* copy_mat(float *a, float *b, int v)                */
/* copy matrix  B=A (V x V)                             */
void copy_mat(float *, float *, int);

/* multi_mat(float *a, float *b, float *c, int v)    */
/* multiply matrix  C=A*B (V x V)                       */
void multi_mat(float *, float *, float *, int);

/* trans_mat(float *in, float *out, int v)            */
/* transform matrix  OUT=t(IN) (V x V)                  */
void trans_mat(float *, float *, int);

/* inv_homoge_mat(float *in, float *out               */
/* get inverse homogeneous matrix                       */
/*     OUT=inv(IN)    IN,OUT:homogeneous matrix         */
void inv_homoge_mat(float *in, float *out);

/* gaiseki(float *a, float *b, float *c)             */
/* calculation gaiseki  c=axb  a[3],b[3],c[3]           */
void gaiseki(float *, float *, float *);

/* multi_vec(float *a, float *b, float *c)           */
/* multiply matrix  c=A*b (V x V)                       */
/* c[3]=A[16]*b[3]                                      */
void multi_vec(float *, float *, float *);

/* multi_vec_notrans(float *a, float *b, float *c)   */
/* multiply matrix  c=A*b (V x V) with no trans         */
/* c[3]=A[16]*b[3] without A[3,7,11]                    */
void multi_vec_notrans(float *, float *, float *);

/* xrot_mat(float *m, float deg)                      */
/* get x-rotation matrix                                */
/* M[16]=  1.0       0.0       0.0       0.0            */
/*         0.0       cos(deg)  -sin(deg) 0.0            */
/*         0.0       sin(deg)  cos(deg)  0.0            */
/*         0.0       0.0       0.0       1.0            */
void xrot_mat(float *, float);

/* xrot_mult_mat(float *m, float deg)                 */
/* get x-rotation matrix and multiply M                 */
/* newM=X(x)*oldM                                       */
void xrot_mult_mat(float *, float);


/* yrot_mat(float *m, float deg)                      */
/* get y-rotation matrix                                */
/* M[16]=  cos(deg)  0.0       sin(deg)  0.0            */
/*         0.0       1.0       0.0       0.0            */
/*         -sin(deg) 0.0       cos(deg)  0.0            */
/*         0.0       0.0       0.0       1.0            */
void yrot_mat(float *, float);

/* yrot_mult_mat(float *m, float deg)                 */
/* get y-rotation matrix and multiply M                 */
/* newM=Y(y)*oldM                                       */
void yrot_mult_mat(float *, float);

/* yrot_mat(float *m, float deg)                      */
/* get y-rotation matrix                                */
/* M[16]=  cos(deg)  -sin(deg) 0.0       0.0            */
/*         sin(deg)  cos(deg)  0.0       0.0            */
/*         0.0       0.0       1.0       0.0            */
/*         0.0       0.0       0.0       1.0            */
void zrot_mat(float *, float);

/* zrot_mult_mat(float *m, float deg)                 */
/* get zrotation matrix and multiply M                 */
/* newM=Z(z)*oldM                                       */
void zrot_mult_mat(float *, float);

/* yzxrot_mat(float *m, float x, float y, float z)  */
/* get y-z-x rotation matrix                            */
/* M=Y(y)Z(z)X(x)                                       */
/* M=yrot_mat(y)*zrot_mat(z)*xrot_mat(x)                */
void yzxrot_mat(float *, float , float , float );

/* hand_mat(float *hand, float *matrix)               */
/* get hand homogeneous matrix                          */
/* M[16]=                                hand[0]        */
/*                Y(y)Z(Z)X(x)           hand[1]        */
/*                                       hand[2]        */
/*         0.0       0.0       0.0       1.0            */
void hand_mat(float *, float *);

/* gramshumit_z_mat(float *vector, float *matrix)     */
/* get normalized matrix by gramshumit                  */
/*        vector=M*z       z=(0.0, 0.0, 1.0)            */
/*                                                      */
/* M[16]=  ->        ->        ->        0.0            */
/*         e1        e2        e3        0.0            */
/*                                       0.0            */
/*         0.0       0.0       0.0       1.0            */
void gramshumit_z_mat(float *, float *);
