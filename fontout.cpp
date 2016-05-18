/********************************************************/
/*	fontout.c                                       */
/*	Output Raster Font and Bitmap Font with GLUT    */
/********************************************************/

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


void stroke_output(float x, float y, float z, float scale, char *format,...)
{
    va_list args;
    char buffer[200], *p;
    
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);
    
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    for (p = buffer; *p; p++)
	glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


void bitmap_output(float x, float y, float z, char *string, void *font)
{
    int len, i;
    glRasterPos3f(x, y, z);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
	glutBitmapCharacter(font, string[i]);
    }
}
