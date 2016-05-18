/********************************************************/
/*	mouse as trackball operating module with GLUT   */
/*	Produced by whashimo				*/
/*	IRIS       Version	1996/07/11       	*/
/********************************************************/
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "cg_track.h"
#include "matrixf.h"

#define TRUE        1
#define FALSE       0

#define TRACKBALLSIZE  (0.8)

int mouse_moving=FALSE;
int beginx, beginy;
float model_matrix[16];

void pre_track(float *, float, float, float, float);

/* store mouse status and beginning pos */
void Mouse_track(int button, int state, int x, int y)
{
    /* left button is pressed */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	mouse_moving=TRUE;
	beginx = x;
	beginy = y;
	/* initialize of view matrix */
	ind_mat(model_matrix, 4);
    }
    /* left button is released */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	mouse_moving = FALSE;
}

/* store mouse motion(location) */
void Motion_track(int x, int y)
{
    void pre_track(float *, float, float, float, float);

    if(mouse_moving){
	int ww, wh;
	ww=glutGet(GLUT_WINDOW_WIDTH);
	wh=glutGet(GLUT_WINDOW_HEIGHT);
	/* calculation of model_matrix */
	pre_track(
		  model_matrix,
		  (2.0*beginx - ww) / ww,
		  (wh - 2.0*beginy) / wh,
		  (2.0*x - ww) / ww,
		  (wh - 2.0*y) / wh);
    }
}


/* translation of model                         */
/* This module should be called by display func */
void ViewFromMouse_track(void)
{
    static double matrix[16]={1.0, 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0, 0.0,
				 0.0, 0.0, 1.0, 0.0,
				 0.0, 0.0, 0.0, 1.0};
    glPushMatrix();
    {
	glLoadMatrixf(model_matrix);
	glMultMatrixd(matrix);    
	glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
    }
    glPopMatrix();

    glMultMatrixd(matrix);
}


/* prepare for mouse operation               */
/* This module should be called by main func */
void ControlMouse_track(void)
{
    ind_mat(model_matrix, 4);
    glutMouseFunc(Mouse_track);
    glutMotionFunc(Motion_track);
}

void pre_track(float model_matrix[16],
	       float p1x, float p1y,
	       float p2x, float p2y)
{
    float p1[3], p2[3];
    float tb_project_to_sphere(float ,float ,float);
    float tmp_matrix[16];

    void TrackBall(float *, float *, float *, float);

    /* Zero rotation */
    if (p1x == p2x && p1y == p2y) {
	/* initialize of view matrix */
	ind_mat(model_matrix, 4);
        return;
    }

    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */
    p1[0]=p1x;
    p1[1]=p1y;
    p1[2]=tb_project_to_sphere(TRACKBALLSIZE,p1x,p1y);
    p2[0]=p2x;
    p2[1]=p2y;
    p2[2]=tb_project_to_sphere(TRACKBALLSIZE,p2x,p2y);

    TrackBall(tmp_matrix, p1, p2, TRACKBALLSIZE);
    trans_mat(tmp_matrix, model_matrix, 4);
}

float tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;
    d = sqrt(x*x + y*y);

    /* Inside sphere */
    /* d < r/sqrt(2.0); */
    if (d < r * 0.70710678118654752440){
        z = sqrt(r*r - d*d);
    } else {
	/* t=r/sqrt(2.0); */
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}
