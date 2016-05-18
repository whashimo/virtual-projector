/********************************************************/
/*	fontout.h                                       */
/********************************************************/

/* stroke_output(float x, float y, float z, float scale,*/
/*               char *format,...)                      */ 
/* output stroke font -> position   (x, y, z)           */
/*                       scale rate (scale)             */
/*                       format:    "hogehoge"          */
/*                                  "%d %d", int, int   */                    
void stroke_output(float, float, float, float, char *,...);


/* bitmap_output(float x, float y, float z, char *,     */
/*               void *)                                */
/* output bitmap font -> position   (x, y, z)           */
/*                       char*:     "hogehoge"          */
/*                       void*      FontName            */
/*******************************************************
  Bitmap font constants (in GLUT)
  GLUT_BITMAP_9_BY_15
  GLUT_BITMAP_8_BY_13
  GLUT_BITMAP_TIMES_ROMAN_10
  GLUT_BITMAP_TIMES_ROMAN_24
  
  GLUT_BITMAP_HELVETICA_10
  GLUT_BITMAP_HELVETICA_12
  GLUT_BITMAP_HELVETICA_18
*******************************************************/
void bitmap_output(float, float, float, char *, void *);

