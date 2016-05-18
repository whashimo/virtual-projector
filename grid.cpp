#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* 補正テーブルファイルを指定 */
#define OPTION_FILE "option.txt"

/* 画面サイズを指定 */
#define CRTSIZE_W 1024
#define CRTSIZE_H 768

extern int mesh_ref[HDIV][VDIV][2];
extern float mesh_pos_st[HDIV][VDIV][2];
extern int mesh_nubmer_orig[HDIV];

int max_vertex;
int max_patch;

void display_table(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, CRTSIZE_W, 0.0, CRTSIZE_H);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glLineWidth(2.0);
  glColor3f(1.0, 1.0, 1.0);
  for(i=0;i<max_vertex;i++){
    glBegin(GL_LINE_STRIP);
    glVertex2fv(dsk_pos+2*patch[i][1]);
    glVertex2fv(dsk_pos+2*patch[i][2]);
    glVertex2fv(dsk_pos+2*patch[i][3]);
    if(patch[i][0]==4)
      glVertex2fv(dsk_pos+2*patch[i][4]);
    glVertex2fv(dsk_pos+2*patch[i][1]);
    glEnd();
  }

  glColor3f(1.0, 0.0, 0.0);
  for(i=0;i<max_vertex;i++){
    glRectf(*(dsk_pos+2*(i)+0) -5.0,
            *(dsk_pos+2*(i)+1) -5.0,
            *(dsk_pos+2*(i)+0) +5.0,
            *(dsk_pos+2*(i)+1) +5.0);
  }


  glFlush();
}


void init_table(char *filename)
{
  int i;
  FILE *fp;
  char filename_add[100];

  sprintf(filename_add, "%s.table", filename);

  if((fp=fopen(filename_add, "r"))==NULL){
    fprintf(stderr, "Cannot open table data %s\n", filename_add);
    return;
  }
  /* テクスチャ格子点数 */
  fscanf(fp, "%d", &max_vertex);
  printf("initialize table.dat %d\n ",max_vertex);

  /* テクスチャ座標(S,T) */
  pix_pos=(float *)malloc(sizeof(float)*2*max_vertex);
  /* テクスチャ貼付け位置(X,Y) */
  dsk_pos=(float *)malloc(sizeof(float)*2*max_vertex);

  /* 対応テーブル読み込み */
  for(i=0;i<max_vertex;i++){
    fscanf(fp, "%f %f %f %f",
           dsk_pos+2*i+0,/* width->x */
           dsk_pos+2*i+1,/* height->y */
           pix_pos+2*i+0, /* s */
           pix_pos+2*i+1);/* t */
    printf("%f %f %f %f \n",
           *(dsk_pos+2*i+0), /* width->x */
           *(dsk_pos+2*i+1), /* height->y */
           *(pix_pos+2*i+0), /* s */
           *(pix_pos+2*i+1)); /* t */
  }
  printf("initialize table data completely %d\n", max_vertex);

  /* パッチ数 */
  fscanf(fp, "%d", &max_patch);
  /* 対応テーブル読み込み */
  for(i=0;i<max_patch;i++){
    fscanf(fp, "%d", &(patch[i][0]));
    if(patch[i][0]==3)
      fscanf(fp, "%d %d %d",
             &(patch[i][1]),
             &(patch[i][2]),
             &(patch[i][3]));
    else
      fscanf(fp, "%d %d %d %d",
             &(patch[i][1]),
             &(patch[i][2]),
             &(patch[i][3]),
             &(patch[i][4]));
  }
  printf("initialize patch data completely %d\n ",max_patch);
  fclose(fp);
}



int main(int argc, char** argv)
{
  void myinit(void);

  glutInitWindowSize (CRTSIZE_W, CRTSIZE_H);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
#ifdef GAMEMODE
  glutGameModeString("width=1024 height=768 bpp~16 hertz>=60");
  if(glutEnterGameMode()==GL_FALSE)
    exit(0);
  glutEnterGameMode();
#else
    if(glutCreateWindow ("Correction of Shpere") == GL_FALSE)
    exit(0);
#endif
  load_option();  
  myinit ();
  glutFullScreen();
  Menu();
  glutKeyboardFunc(Key);
  glutMouseFunc (pickSquares);
  glutMotionFunc(Motion_track);
  glutDisplayFunc(display);
  glutMainLoop();
  return(0);
}
