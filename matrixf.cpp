/********************************************************/
/*	Program for Matrix with OpenGL  		*/
/*	Matrix Calculation module			*/
/*	Coded by Iwata lab. & whashimo                  */
/*	Windows NT Version	1996/01/18       	*/
/*	IRIS       Version	1996/04/15       	*/
/*	float      Version	2001/01/15       	*/
/********************************************************/
#include <stdio.h>
#include <math.h>

#include "matrixf.h"

#define DEG2RAD		57.29577951

void ind_mat(float *m,int v)
{
    int i,j;
    for(i=0;i != v;i++)
        for(j=0;j != v;j++)
            *(m+i*v+j) = (float)(i==j);
}


void print_mat(float *m, int v)
{
    int i,j;
    printf("-------------------------\n\r");
    for(i=0;i<v;i++){
	for(j=0;j<v;j++)
	    printf("%f ",*(m+i*v+j));
	printf("\n\r");
    }
}


void copy_mat(float *a, float *b, int v)
{
    int i;
    for(i=0;i<v*v;i++)
	*(b+i)=*(a+i);
}


void multi_mat(float *a, float *b, float *c, int v)
{
    int i,j,k;
    for(i=0;i<v*v;i++)*(c+i)=0.0;
    
    for(i=0;i<v;i++)
	for(j=0;j<v;j++)                
	    for(k=0;k<v;k++)            
		*(c+i*v+j)+=*(a+i*v+k)**(b+k*v+j);
}


void gaiseki(float a[3], float b[3], float c[3])
{
    c[0]=a[1]*b[2]-a[2]*b[1];
    c[1]=a[2]*b[0]-a[0]*b[2];
    c[2]=a[0]*b[1]-a[1]*b[0];
}


void xrot_mat(float *m, float deg)
{
    m[0]=m[15]=1.0;
    m[1]=m[2]=m[3]=m[4]=m[7]=m[8]=m[11]=m[12]=m[13]=m[14]=0.0;
	
    m[5]=m[10]=(float)cos(deg/DEG2RAD);
    m[9]=(float)sin(deg/DEG2RAD);
    m[6]=-m[9];
}

void xrot_mult_mat(float *m, float deg)
{
    float a[16];
    float b[16];
    xrot_mat(a, deg);
    copy_mat(m, b, 4);
    multi_mat(a, b, m, 4);
}

void yrot_mat(float *m, float deg)
{
    m[5]=m[15]=1.0;
    m[1]=m[3]=m[4]=m[6]=m[7]=m[9]=m[11]=m[12]=m[13]=m[14]=0.0;
	
    m[0]=m[10]=(float)cos(deg/DEG2RAD);
    m[2]=(float)sin(deg/DEG2RAD);
    m[8]=-m[2];
}

void yrot_mult_mat(float *m, float deg)
{
    float a[16];
    float b[16];
    yrot_mat(a, deg);
    copy_mat(m, b, 4);
    multi_mat(a, b, m, 4);
}

void zrot_mat(float *m, float deg)
{
    m[10]=m[15]=1.0;
    m[2]=m[3]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=0.0;

    m[0]=m[5]=(float)cos(deg/DEG2RAD);
    m[4]=(float)sin(deg/DEG2RAD);
    m[1]=-m[4];
}

void zrot_mult_mat(float *m, float deg)
{
    float a[16];
    float b[16];
    zrot_mat(a, deg);
    copy_mat(m, b, 4);
    multi_mat(a, b, m, 4);
}

void yzxrot_mat(float *m, float deg_x, float deg_y, float deg_z)
{
    float cx, cy, cz, sx, sy, sz;
	
    cx=(float)cos(deg_x/DEG2RAD);
    cy=(float)cos(deg_y/DEG2RAD);
    cz=(float)cos(deg_z/DEG2RAD);
    sx=(float)sin(deg_x/DEG2RAD);
    sy=(float)sin(deg_y/DEG2RAD);
    sz=(float)sin(deg_z/DEG2RAD);

    m[0]=cy*cz;
    m[1]=-cx*cy*sz+sx*sy;
    m[2]=sx*cy*sz+cx*sy;
	
    m[4]=sz;
    m[5]=cx*cz;
    m[6]=-sx*cz;
	
    m[8]=-cz*sy;
    m[9]=cx*sy*sz+sx*cy;
    m[10]=-sx*sy*sz+cx*cy;
    m[3]=m[7]=m[11]=m[12]=m[13]=m[14]=0.0;
    m[15]=1.0;
}

void hand_mat(float *hand, float *m)
{
    yzxrot_mat(m, hand[3], hand[4], hand[5]);
    m[3]=hand[0];
    m[7]=hand[1];
    m[11]=hand[2];
}

void trans_mat(float *in, float *out, int v)
{
    int i, j;
    for(i=0;i<v;i++)
	for(j=0;j<v;j++)
	    *(out+v*j+i)=*(in+v*i+j);
}

void inv_homoge_mat(float *in, float *out)
{
    out[0]=in[0];
    out[1]=in[4];
    out[2]=in[8];
    out[3]=-(in[0]*in[3]+in[4]*in[7]+in[8]*in[11]);
    out[4]=in[1];
    out[5]=in[5];
    out[6]=in[9];
    out[7]=-(in[1]*in[3]+in[5]*in[7]+in[9]*in[11]);
    out[8]=in[2];
    out[9]=in[6];
    out[10]=in[10];
    out[11]=-(in[2]*in[3]+in[6]*in[7]+in[10]*in[11]);
    out[12]=out[13]=out[14]=0.0;
    out[15]=in[15];
}

void multi_vec(float *m, float *x, float *y)
{
    y[0]=(float)(m[0]*x[0]+m[1]*x[1]+m[2]*x[2]+m[3]*1.0);
    y[1]=(float)(m[4]*x[0]+m[5]*x[1]+m[6]*x[2]+m[7]*1.0);
    y[2]=(float)(m[8]*x[0]+m[9]*x[1]+m[10]*x[2]+m[11]*1.0);
}

void multi_vec_notrans(float *m, float *x, float *y)
{
    y[0]=(float)(m[0]*x[0]+m[1]*x[1]+m[2]*x[2]);
    y[1]=(float)(m[4]*x[0]+m[5]*x[1]+m[6]*x[2]);
    y[2]=(float)(m[8]*x[0]+m[9]*x[1]+m[10]*x[2]);
}

void gramshumit_z_mat(float *vec, float *m)
{
    float norm;
    float tmp[3];

    norm=(float)sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    m[2]=vec[0]/norm;
    m[6]=vec[1]/norm;
    m[10]=vec[2]/norm;
    
    tmp[0]=(float)   -m[6]*m[2];
    tmp[1]=(float)1.0-m[6]*m[6];
    tmp[2]=(float)   -m[6]*m[10];
    norm=(float)sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
    m[1]=tmp[0]/norm;
    m[5]=tmp[1]/norm;
    m[9]=tmp[2]/norm;
    
    tmp[0]=(float)1.0-m[1]*m[1]-m[2]*m[2];
    tmp[1]=(float)   -m[1]*m[5]-m[2]*m[6];
    tmp[2]=(float)   -m[1]*m[9]-m[2]*m[10];
    norm=(float)sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
    m[0]=tmp[0]/norm;
    m[4]=tmp[1]/norm;
    m[8]=tmp[2]/norm;

    m[3]=m[7]=m[11]=m[12]=m[13]=m[14]=0.0;
    m[15]=1.0;
}

