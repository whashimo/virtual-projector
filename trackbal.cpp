/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 *   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 */

#include <math.h>

void TrackBall(float *, float *, float *, float);

void TrackBall(float m[16], float p1[3], float p2[3], float ballsize)
{
    float a[3];
    float d[3];
    float t;
    float phi;
    
    float co, si;
    float _co;


    a[0]=p1[1]*p2[2]-p1[2]*p2[1];
    a[1]=p1[2]*p2[0]-p1[0]*p2[2];
    a[2]=p1[0]*p2[1]-p1[1]*p2[0];

    /* normalize */
    t=(float)sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=t;
    a[1]/=t;
    a[2]/=t;

    d[0]=p1[0]-p2[0];
    d[1]=p1[1]-p2[1];
    d[2]=p1[2]-p2[2];

    t=(float)((d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/(2.0*ballsize));

    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    phi =(float)asin(t);

    co=(float)cos(phi);
    si=(float)sin(phi);
    _co=(float)1.0-co;

    /* matrix calculation */
    m[0]=co+_co*a[0]*a[0];
    m[1]=_co*a[0]*a[1]-si*a[2];
    m[2]=_co*a[0]*a[2]+si*a[1];

    m[4]=_co*a[0]*a[1]+si*a[2];
    m[5]=co+_co*a[1]*a[1];
    m[6]=_co*a[1]*a[2]-si*a[0];

    m[8]=_co*a[0]*a[2]-si*a[1];
    m[9]=_co*a[1]*a[2]+si*a[0];
    m[10]=co+_co*a[2]*a[2];

    m[3]=m[7]=m[11]=0.0;
    m[12]=m[13]=m[14]=0.0;
    m[15]=1.0;
}
