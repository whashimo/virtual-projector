#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "cg_track.h"
#include "fontout.h"
#include "vproj.h"
#include "matrixf.h"
#include "glui.h"
#include "texture.h"
#include <windows.h>

void init_model(void);
void Maininit(void);
void MainReshape(GLsizei w, GLsizei h);
void display(void);
void Key(unsigned char, int, int);
void MainWinMenu(void);
void StatusWinMenu(void);
long count=0;
time_t time_start, time_end;
void RedisplayBoth(void);


/* for display table */
void display_table(void);
void init_table(char *);
void init_texture(void);

GLUI *glui;
TEXTURE lena;

int MainWindow,StatusWindow,TableWindow;

int toggle_lens=0;
int toggle_focus=0;
int toggle_dispose=0;
int toggle_view=0;
int toggle_real=1;
int toggle_calc=0;
int toggle_four=0;
int toggle_projection1=1;
int toggle_projection2=0;
int toggle_cmirror=1;
int toggle_pmirror=0;

int main_window_w,main_window_h;
int status_window_w,status_window_h;

PROJECTION *pro_ptr=&projection1;/* 操作対象となるプロジェクタへのポインタ*/
float *exch_ptr=&(projection1.projector.param[0]); /* 位置 */
float *exchr_ptr=&(projection1.projector.param[3]); /* 姿勢 */

void exit_prog(int num)
{
  free(projection1.ray);
  free(projection2.ray);
  exit(num);
}

void Key(unsigned char key, int x, int y)
{
   switch (key) {
   case 'A':
     init_table(pro_ptr->filename);
     break;
   case 'Q':
     time(&time_end);
     printf("Connection Rate %lf Hz \n",
            (double)count/difftime(time_end, time_start));
     exit_prog(0);
     break;
     /****************************************************/
     /* 選択されているプロジェクタのパラメータを変更する */
   case 'l':
     *(exch_ptr+2)+=10.0;
     calc_screen(pro_ptr);
     break;
   case 'j':
     *(exch_ptr+2)-=10.0;
     calc_screen(pro_ptr);
     break;
   case 'i':
     *(exch_ptr+1)+=10.0;
     calc_screen(pro_ptr);
     break;
   case ',':
     *(exch_ptr+1)-=10.0;
     calc_screen(pro_ptr);
     break;
   case 'u':
     *(exch_ptr+0)+=10.0;
     calc_screen(pro_ptr);
     break;
   case 'm':
     *(exch_ptr+0)-=10.0;
     calc_screen(pro_ptr);
     break;
   case 'f':
     *(exchr_ptr+2)+=1.0;
     calc_screen(pro_ptr);
     break;
   case 's':
     *(exchr_ptr+2)-=1.0;
     calc_screen(pro_ptr);
     break;
   case 'e':
     *(exchr_ptr+1)+=1.0;
     calc_screen(pro_ptr);
     break;
   case 'c':
     *(exchr_ptr+1)-=1.0;
     calc_screen(pro_ptr);
     break;
   case 'w':
     *(exchr_ptr+0)+=1.0;
     calc_screen(pro_ptr);
     break;
   case 'x':
     *(exchr_ptr+0)-=1.0;
     calc_screen(pro_ptr);
     break;
   case 'k':
     projection1.cmirror_radius+=10.0; /* 凸面鏡の曲率半径 */
     projection2.cmirror_radius+=10.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case 'K':
     projection1.cmirror_radius-=10.0;
     projection2.cmirror_radius-=10.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case 'd':
     screen_radius+=10.0;       /* スクリーンの半径 */
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case 'D':
     screen_radius-=10.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;

   case '0':
     screen_view+=1.0;          /* スクリーンの水平角 */
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case '9':
     screen_view-=1.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case ']':
     screen_up+=1.0;            /* 中心から見たスクリーンの仰角 */
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case '[':
     screen_up-=1.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case '=':
     screen_down+=1.0;          /* 中心から見たスクリーンの俯角 */
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case '-':
     screen_down-=1.0;
     calc_screen(&projection1);
     calc_screen(&projection2);
     break;
   case '1':
     pro_ptr->projector_zoomratio-=0.02; /* プロジェクタのズーム */
     if(pro_ptr->projector_zoomratio<1.0)pro_ptr->projector_zoomratio=1.0;
     focus_initialize(pro_ptr);
     calc_screen(pro_ptr);
     break;
   case '2':
     pro_ptr->projector_zoomratio+=0.02;
     if(pro_ptr->projector_zoomratio>1.5)pro_ptr->projector_zoomratio=1.5;
     focus_initialize(pro_ptr);
     calc_screen(pro_ptr);
     break;
   case '3':
     pro_ptr->projector_focus-=10.0; /* プロジェクタの焦点距離 */
     if(pro_ptr->projector_focus<300.0)pro_ptr->projector_focus=300.0;
     focus_initialize(pro_ptr);
     calc_screen(pro_ptr);
     break;
   case '4':
     pro_ptr->projector_focus+=10.0;
     if(pro_ptr->projector_focus>30000.0)pro_ptr->projector_focus=30000.0;
     focus_initialize(pro_ptr);
     calc_screen(pro_ptr);
     break;
  }
}

/* メイン画面上でのメニュー処理 */
void menu_select(int mode)
{
  switch(mode){
  case 1:                   /* 元のフォーカス面表示，非表示切り替え */
    toggle_focus=!toggle_focus;
    break;
  case 2:                   /* レンズによる結像状態表示，非表示切り替え */
    toggle_lens=!toggle_lens;
    break;
  case 3:                   /* 主光線表示，非表示切り替え */
    toggle_dispose=!toggle_dispose;
    break;
  case 4:                   /* 視点の位置主観視，俯瞰視切り替え */
    toggle_view=!toggle_view;
    break;
  case 5:                   /* 架台フレーム表示，非表示 */
    toggle_real=!toggle_real;
    break;
  case 6:
    break;
  case 7:                   /* 光線表示，非表示 */
    toggle_calc=!toggle_calc;
    break;
  case 8:                   /* 三面図表示，非表示 */
    toggle_four=!toggle_four;
    break;

  case 10:                  /* 上記設定をリセット */
    toggle_lens=1;
    toggle_focus=1;
    toggle_dispose=0;
    toggle_view=0;
    toggle_real=0;
    toggle_calc=0;
    toggle_four=0;

    projector_load(&projection1);
    projector_load(&projection2);
    projection_load(&projection1);
    projection_load(&projection2);
    focus_initialize(&projection1);
    focus_initialize(&projection2);
    calc_screen(&projection1);
    calc_screen(&projection2);
    break;

  case 20:                      /* プロジェクタ1アクティブ切り替え */
    toggle_projection1=!toggle_projection1;
    break;
  case 21:                      /* プロジェクタ2アクティブ切り替え */
    toggle_projection2=!toggle_projection2;
    break;
  case 0:                       /* 終了処理 */
    time(&time_end);
    printf("Connection Rate %lf Hz \n",
           (double)count/difftime(time_end, time_start));
    exit_prog(0);
    break;
    }
}

/* メイン画面上でのメニュー項目 */
void MainWinMenu(void)
{
    glutCreateMenu(menu_select);
    glutAddMenuEntry("Toggle Projection1", 20);    
    glutAddMenuEntry("Toggle Projection2", 21);    
    glutAddMenuEntry("-----------------", -1);
    glutAddMenuEntry("Focus Plane on/off", 1);    
    glutAddMenuEntry("state of focus on/off ", 2);    
    glutAddMenuEntry("dispose on/off", 3);
    glutAddMenuEntry("viewpoint outside/inside", 4);
    glutAddMenuEntry("Toggle Real", 5);
    glutAddMenuEntry("Toggle Calc", 7);
    glutAddMenuEntry("Toggle Four", 8);
    glutAddMenuEntry("Reset All", 10);
    glutAddMenuEntry("-----------------", -1);
    glutAddMenuEntry("Quit", 0);
    glutSetWindow(MainWindow);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/* ステータス画面上でのメニュー処理 */
void status_menu_select(int mode)
{
  switch(mode){
  case 1:                       /* カレントデータをプロジェクタ位置にする */
    exch_ptr=&(pro_ptr->projector.param[0]);
    break;
  case 2:                       /* カレントデータをプロジェクタ姿勢にする */
    exchr_ptr=&(pro_ptr->projector.param[3]);
    break;
  case 3:                       /* カレントデータを平面鏡位置にする */
    exch_ptr=&(pro_ptr->pmirror.param[0]);
    break;
  case 4:                       /* カレントデータを平面鏡姿勢にする */
    exchr_ptr=&(pro_ptr->pmirror.param[3]);
    break;
  case 5:                       /* カレントデータを凸面鏡位置にする */
    exch_ptr=&(pro_ptr->cmirror.param[0]);
    break;
  case 6:                       /* カレントデータを凸面鏡姿勢にする */
    exchr_ptr=&(pro_ptr->cmirror.param[3]);
    break;
  case 7:                       /* カレントデータをスクリーン位置にする */
    exch_ptr=&(pro_ptr->screen.param[0]);
    break;
  case 8:                       /* カレントデータをスクリーン姿勢にする */
    exchr_ptr=&(pro_ptr->screen.param[3]);
    break;
  case 20:                      /* プロジェクタ1パラメータをロード */
    projector_load(&projection1);
    projection_load(&projection1);
    focus_initialize(&projection1);
    calc_screen(&projection1);
    break;
  case 21:                      /* プロジェクタ2パラメータをロード */
    projector_load(&projection2);
    projection_load(&projection2);
    focus_initialize(&projection2);
    calc_screen(&projection2);
    break;
  case 22:                      /* プロジェクタ1パラメータをセーブ */
    projector_save(projection1);
    projection_save(projection1);
    break;
  case 23:                      /* プロジェクタ2パラメータをセーブ */
    projector_save(projection2);
    projection_save(projection2);
    break;
  case 24:
    projection_calc_save(projection1);
    projection_calc_save(projection2);
    break;
  case 0:
    /* 終了処理 */
    time(&time_end);
    printf("Connection Rate %lf Hz \n",
	   (double)count/difftime(time_end, time_start));
    exit_prog(0);
    break;
  case 99:
    /* カレントプロジェクタを切り替え */
    if(pro_ptr==&projection1)
      pro_ptr=&projection2;
    else
      pro_ptr=&projection1;
    break;
  case 100:
    /*カレントのパラメータを違うほうに反映させる*/
    if(pro_ptr==&projection1){
#define P1TOP2_CM(x) projection2.cmirror.param[x]=projection1.cmirror.param[x]
      P1TOP2_CM(0);      P1TOP2_CM(1);      P1TOP2_CM(2);
      P1TOP2_CM(3);      P1TOP2_CM(4);      P1TOP2_CM(5);
#define P1TOP2_PM(x) projection2.pmirror.param[x]=projection1.pmirror.param[x]
      P1TOP2_PM(0);      P1TOP2_PM(1);      P1TOP2_PM(2);
      P1TOP2_PM(3);      P1TOP2_PM(4);      P1TOP2_PM(5);
      calc_screen(&projection2);
    }else{
#define P2TOP1_CM(x) projection1.cmirror.param[x]=projection2.cmirror.param[x]
      P2TOP1_CM(0);      P2TOP1_CM(1);      P2TOP1_CM(2);
      P2TOP1_CM(3);      P2TOP1_CM(4);      P2TOP1_CM(5);
#define P2TOP1_PM(x) projection1.pmirror.param[x]=projection2.pmirror.param[x]
      P2TOP1_PM(0);      P2TOP1_PM(1);      P2TOP1_PM(2);
      P2TOP1_PM(3);      P2TOP1_PM(4);      P2TOP1_PM(5);
      calc_screen(&projection1);
    }
  case -1:
    break;
  }
}

void null_select(int mode)
{
}

void StatusWinMenu(void)
{
  int glut_status_menu[5];
  glut_status_menu[0]=glutCreateMenu(null_select);
  glutAddMenuEntry("zoom-: '1' key", -1);
  glutAddMenuEntry("zoom+: '2' key", -1);
  glutAddMenuEntry("focus-: '3' key", -1);
  glutAddMenuEntry("focus+: '4' key", -1);

  glut_status_menu[1]=glutCreateMenu(null_select);
  glutAddMenuEntry("radius+: 'k' key", -1);
  glutAddMenuEntry("radius-: 'K' key", -1);

  glut_status_menu[2]=glutCreateMenu(null_select);
  glutAddMenuEntry("radius+: 'd' key", -1);
  glutAddMenuEntry("radius-: 'D' key", -1);

  glut_status_menu[3]=glutCreateMenu(null_select);
  glutAddMenuEntry("view+: '0' key", -1);
  glutAddMenuEntry("view-: '9' key", -1);
  glutAddMenuEntry("up+: ']' key", -1);
  glutAddMenuEntry("up-: '[' key", -1);
  glutAddMenuEntry("down+: '=' key", -1);
  glutAddMenuEntry("down-: '-' key", -1);

  glut_status_menu[4]=glutCreateMenu(null_select);
  glutAddMenuEntry("x+: 'u' key", -1);
  glutAddMenuEntry("x-: 'm' key", -1);
  glutAddMenuEntry("y+: 'i' key", -1);
  glutAddMenuEntry("y-: ',' key", -1);
  glutAddMenuEntry("z+: 'l' key", -1);
  glutAddMenuEntry("z-: 'j' key", -1);
  glutAddMenuEntry("------------", -1);
  glutAddMenuEntry("rx+: 'w' key", -1);
  glutAddMenuEntry("rx-: 'x' key", -1);
  glutAddMenuEntry("ry+: 'e' key", -1);
  glutAddMenuEntry("ry-: 'c' key", -1);
  glutAddMenuEntry("rz+: 'f' key", -1);
  glutAddMenuEntry("rz-: 's' key", -1);

  glutCreateMenu(status_menu_select);
  glutAddMenuEntry("Projection CHG", 99); 
  glutAddMenuEntry("Projector pos", 1);
  glutAddMenuEntry("Projector ori", 2);    
  glutAddSubMenu("Projector zoom", glut_status_menu[0]);
  glutAddMenuEntry("Mirror1 pos", 3);
  glutAddMenuEntry("Mirror1 ori", 4);
  glutAddMenuEntry("Mirror2 pos", 5);
  glutAddMenuEntry("Mirror2 ori", 6);
  glutAddSubMenu("Mirror2 radius", glut_status_menu[1]);
  glutAddMenuEntry("Screen pos", 7);
  glutAddMenuEntry("Screen ori", 8);
  glutAddSubMenu("Screen radius", glut_status_menu[2]);
  glutAddSubMenu("Screen param", glut_status_menu[3]);
  glutAddSubMenu("Keyboard help", glut_status_menu[4]);
  glutAddMenuEntry("Data copy", 100); 
  glutAddMenuEntry("Load projection1", 20);    
  glutAddMenuEntry("Load projection2", 21);    
  glutAddMenuEntry("Save projection1", 22);    
  glutAddMenuEntry("Save projection2", 23);    
  glutAddMenuEntry("Save calc table(capture)", 24);    
  glutAddMenuEntry("Quit", 0);

  glutSetWindow(StatusWindow);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void display_string(char *str,int x,  int y)
{
  int wy;
  int width, height;

  width = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

  /* 固定パイプラインに戻す */
//  glUseProgram(0);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0f, (float)width, 0.0f, (float)height);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

#if OPENGL_IMAGE_COORDINATE
  wy = y;
#else
   wy = height - y - 15;//13
#endif
  glRasterPos2i(x, wy);

  for (; *str != 0; str++)
    {
      if (*str == '\n')
        {
          wy -= (15);//(13)
          glRasterPos2i( x, wy );
          continue;
        }

//      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str);
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *str);
    }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


void display(void)
{
  void display_main(void);
  double matrix[16];
  count++;
  glViewport(0, 0, main_window_w, main_window_h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
    glLoadIdentity ();

    glPushMatrix();
    if(toggle_view){
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glFrustum(-2.0, 2.0, -2.0, 2.0, 1.0, 100000.0);
      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();
      glTranslatef(-view_pos[0], -view_pos[1], -view_pos[2]);
      ViewFromMouse_track();
    }else{
      glMatrixMode (GL_PROJECTION);   /*  prepare for and then  */
      glLoadIdentity ();  /*  define the projection  */
 //     glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100000.0);
      glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 100000.0);
      glMatrixMode (GL_MODELVIEW);    /*  back to modelview matrix    */
      glTranslatef(0.0, -0.0, -1000.0);
      ViewFromMouse_track();
    }
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();

    if(toggle_four){
      /* 3面図 */
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho(-700.0, 700.0, -700.0, 700.0, 1.0, 100000.0);
      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();

      glViewport(0, 0, main_window_w/2, main_window_h/2);
      glPushMatrix();
      glTranslatef(0.0, 0.0, -1000.0);
      display_main();
      glPopMatrix();

      glViewport(main_window_w/2, 0, main_window_w/2, main_window_h/2);
      glPushMatrix();
      glTranslatef(0.0, 0.0, -1000.0);
      glRotatef(90.0, 0.0, 1.0, 0.0);
      display_main();
      glPopMatrix();

      glViewport(0, main_window_h/2, main_window_w/2, main_window_h/2);
      glPushMatrix();
      glTranslatef(0.0, 0.0, -1000.0);
      glRotatef(90.0, 1.0, 0.0, 0.0);
      display_main();
      glPopMatrix();

      /* 投影図 */
      glMatrixMode (GL_PROJECTION); /*  prepare for and then  */ 
      glLoadIdentity ();	/*  define the projection  */
      glFrustum(-10.0, 10.0, -10.0, 10.0, 10.0, 10000.0);
      glMatrixMode (GL_MODELVIEW); /*  back to modelview matrix    */

      glLoadIdentity ();
      glViewport(main_window_w/2, main_window_h/2, 
		 main_window_w/2, main_window_h/2);
      glMultMatrixd(matrix);
      display_main();

    }else{
      glLoadIdentity ();
      glMultMatrixd(matrix);
      /*回転中心*/
      display_main();
    }

  //display_string("test",1,1);
  glFlush();
  glutSwapBuffers();
}

void model_projector(void)
{
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glColor4d(1.0, 1.0, 1.0, 0.2);

#if 0
  glBegin(GL_QUADS);
  glNormal3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, -187.0, 0.0);
  glVertex3f(0.0, -187.0, 375.0);
  glVertex3f(0.0, 59.0, 375.0);
  glVertex3f(0.0, 59.0, 0.0);
  glEnd();
#endif
  
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-340.0, 59.0, 133.0);
  glVertex3f(35.0, 59.0, 133.0);
  glVertex3f(35.0, 59.0, -292.0);
  glVertex3f(-340.0, 59.0, -292.0);

  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(-340.0, -184.0, 133.0);
  glVertex3f(35.0, -184.0, 133.0);
  glVertex3f(35.0, -184.0, -292.0);
  glVertex3f(-340.0, -184.0, -292.0);

  glNormal3f(0.0, 0.0, 1.0);
  glVertex3f(-340.0, 59.0, 133.0);
  glVertex3f(35.0, 59.0, 133.0);
  glVertex3f(35.0, -184.0, 133.0);
  glVertex3f(-340.0, -184.0, 133.0);

  glNormal3f(0.0, 0.0, -1.0);
  glVertex3f(-340.0, 59.0, -292.0);
  glVertex3f(35.0, 59.0, -292.0);
  glVertex3f(35.0, -184.0, -292.0);
  glVertex3f(-340.0, -184.0, -292.0);

  glNormal3f(1.0, 0.0, 0.0);
  glVertex3f(35.0, 59.0, -292.0);
  glVertex3f(35.0, 59.0, 133.0);
  glVertex3f(35.0, -184.0, 133.0);
  glVertex3f(35.0, -184.0, -292.0);

  glNormal3f(-1.0, 0.0, 0.0);
  glVertex3f(-340.0, 59.0, -292.0);
  glVertex3f(-340.0, 59.0, 133.0);
  glVertex3f(-340.0, -184.0, 133.0);
  glVertex3f(-340.0, -184.0, -292.0);

  glEnd();
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
 
}


void model_screen(PROJECTION *pr)
{
  int i,j;
  float _tmp[3];
  multi_vec(pr->projector.matrix_inv, pr->screen.param, _tmp);
  glColor3f(0.8, 0.8, 0.8);
  for(i=0;i<=NUM_I-1;i++){
    for(j=0;j<=NUM_J-1;j++){
      glBegin(GL_POLYGON);
      if(pr->ray[i][j].active){
	glNormal3f(pr->ray[i][j].scr.refpos[0]-_tmp[0],
		   pr->ray[i][j].scr.refpos[1]-_tmp[1],
		   pr->ray[i][j].scr.refpos[2]-_tmp[2]);
	glVertex3fv(pr->ray[i][j].scr.refpos);
      }
      if(pr->ray[i+1][j].active){
	glNormal3f(pr->ray[i+1][j].scr.refpos[0]-_tmp[0],
		   pr->ray[i+1][j].scr.refpos[1]-_tmp[1],
		   pr->ray[i+1][j].scr.refpos[2]-_tmp[2]);
	glVertex3fv(pr->ray[i+1][j].scr.refpos);
      }
      if(pr->ray[i+1][j+1].active){
	glNormal3f(pr->ray[i+1][j+1].scr.refpos[0]-_tmp[0],
		   pr->ray[i+1][j+1].scr.refpos[1]-_tmp[1],
		   pr->ray[i+1][j+1].scr.refpos[2]-_tmp[2]);
	glVertex3fv(pr->ray[i+1][j+1].scr.refpos);
      }
      if(pr->ray[i][j+1].active){
	glNormal3f(pr->ray[i][j+1].scr.refpos[0]-_tmp[0],
		   pr->ray[i][j+1].scr.refpos[1]-_tmp[1],
		   pr->ray[i][j+1].scr.refpos[2]-_tmp[2]);
	glVertex3fv(pr->ray[i][j+1].scr.refpos);
      }
      glEnd();
    }
  }
}

void model_mirror(PROJECTION *pr)
{
  int i,j;
  float _tmpc[3];
  for(i=0;i<=NUM_I-1;i++){
    for(j=0;j<=NUM_J-1;j++){
      multi_vec(pr->projector.matrix_inv, pr->cmirror.param, _tmpc);
      glBegin(GL_POLYGON);
      if(pr->ray[i][j].active){
	glNormal3f(pr->ray[i][j].mir2.refpos[0]-_tmpc[0],
		   pr->ray[i][j].mir2.refpos[1]-_tmpc[1],
		   pr->ray[i][j].mir2.refpos[2]-_tmpc[2]);
	glVertex3fv(pr->ray[i][j].mir2.refpos);
      }
      if(pr->ray[i+1][j].active){
	glNormal3f(pr->ray[i+1][j].mir2.refpos[0]-_tmpc[0],
		   pr->ray[i+1][j].mir2.refpos[1]-_tmpc[1],
		   pr->ray[i+1][j].mir2.refpos[2]-_tmpc[2]);
	glVertex3fv(pr->ray[i+1][j].mir2.refpos);
      }
      if(pr->ray[i+1][j+1].active){
	glNormal3f(pr->ray[i+1][j+1].mir2.refpos[0]-_tmpc[0],
		   pr->ray[i+1][j+1].mir2.refpos[1]-_tmpc[1],
		   pr->ray[i+1][j+1].mir2.refpos[2]-_tmpc[2]);
	glVertex3fv(pr->ray[i+1][j+1].mir2.refpos);
      }
      if(pr->ray[i][j+1].active){
	glNormal3f(pr->ray[i][j+1].mir2.refpos[0]-_tmpc[0],
		   pr->ray[i][j+1].mir2.refpos[1]-_tmpc[1],
		   pr->ray[i][j+1].mir2.refpos[2]-_tmpc[2]);
	glVertex3fv(pr->ray[i][j+1].mir2.refpos);
      }
      glEnd();
    }
  }
}


void display_main(void)
{
  void display_projection(PROJECTION *);

  if(toggle_projection1)
    display_projection(&projection1);
  if(toggle_projection2)
    display_projection(&projection2);
}    

void display_projection(PROJECTION *pr)
{
  int i,j;
  void circle(void), harf_circle(void);

  glPushMatrix();
  {
    /* 回転中心 */
    glTranslatef(-pr->screen.param[0],
		 -pr->screen.param[1],
		 -pr->screen.param[2]);

      glBegin(GL_LINES);
      glColor3d(0.0, 1.0, 0.0);
      glVertex3d(0.0, 2000.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);

      glColor3d(0.0, 0.0, 1.0);
      glVertex3d(0.0, 0.0, 2000.0);
      glVertex3d(0.0, 0.0, 0.0);

      glColor3d(1.0, 0.0, 0.0);
      glVertex3d(2000.0, 0.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glEnd();

	  			      /* 外枠 */
    glLineWidth(1.0);
    glColor3f(1.0, 1.0, 1.0);
    glutWireCube(screen_radius*2.0);

    glPushMatrix();
    {
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glColor4d(1.0, 1.0, 1.0, 0.2);
      glBegin(GL_QUADS);
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(-screen_radius,-screen_radius,-screen_radius);
      glVertex3f(-screen_radius,-screen_radius,screen_radius);
      glVertex3f(screen_radius,-screen_radius,screen_radius);
      glVertex3f(screen_radius,-screen_radius,-screen_radius);
      glEnd();
    }
    glPopMatrix();

    /*ワールド座標系*/
    glPushMatrix();
    {
      /* pivot */
      glColor3d(1.0, 1.0, 1.0);
      glutSolidCube(10.0);
      
      /* スクリーン座標系 */
      glTranslatef(pr->screen.param[0],
		   pr->screen.param[1],
		   pr->screen.param[2]);

      glRotatef(pr->screen.param[4], 0.0, 1.0, 0.0);
      glRotatef(pr->screen.param[5], 0.0, 0.0, 1.0);
      glRotatef(pr->screen.param[3], 1.0, 0.0, 0.0);

      glBegin(GL_LINES);
      glColor3d(0.3, 0.6, 0.3);
      glVertex3d(0.0, 2000.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);

      glColor3d(0.3, 0.3, 0.6);
      glVertex3d(0.0, 0.0, 2000.0);
      glVertex3d(0.0, 0.0, 0.0);
      glColor3d(0.6, 0.3, 0.3);
      glVertex3d(2000.0, 0.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glEnd();
      
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glColor4d(1.0, 1.0, 1.0, 0.2);

      {
        GLdouble eqn0[]={0.0,-1.0,0.0, screen_radius*sin(screen_up/180.0*M_PI)};
        GLdouble eqn1[]={0.0,1.0,0.0, -screen_radius*sin(screen_down/180.0*M_PI)};
        GLdouble eqn2[]={-cos(screen_view*DEG2RAD),0.0, -sin(screen_view*DEG2RAD),0.0,0.0};
        GLdouble eqn3[]={1.0, 0.0, 0.0, 0.0};
        glClipPlane(GL_CLIP_PLANE0,eqn0);
        glClipPlane(GL_CLIP_PLANE1,eqn1);
        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_CLIP_PLANE1);

        glClipPlane(GL_CLIP_PLANE2,eqn2);
        glClipPlane(GL_CLIP_PLANE3,eqn3);
        glEnable(GL_CLIP_PLANE2);
        glEnable(GL_CLIP_PLANE3);
        //右半球
        glutSolidSphere(screen_radius,32.0,32.0); 
        glDisable(GL_CLIP_PLANE3);
        glDisable(GL_CLIP_PLANE2);

        eqn2[0]=cos(screen_view*DEG2RAD);
        eqn2[2]=-sin(screen_view*DEG2RAD);
        eqn3[0]=-1.0;
        
        glClipPlane(GL_CLIP_PLANE2,eqn2);
        glClipPlane(GL_CLIP_PLANE3,eqn3);
        glEnable(GL_CLIP_PLANE2);
        glEnable(GL_CLIP_PLANE3);
        //左半球
        glutSolidSphere(screen_radius,32.0,32.0); 
        glDisable(GL_CLIP_PLANE3);
        glDisable(GL_CLIP_PLANE2);

        //glutSolidSphere(screen_radius,32.0,32.0); 
        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
      }
      /* スクリーン水平調節用 */
      glPushMatrix();
      {
	glRotatef(screen_view, 0.0, 1.0, 0.0);
	glScalef(1.0, screen_radius, screen_radius);
	glColor4d(1.0, 1.0, 1.0, 0.2);
	harf_circle();
      }
      glPopMatrix();

      glPushMatrix();
      {
	glRotatef(-screen_view, 0.0, 1.0, 0.0);
	glScalef(1.0, screen_radius, screen_radius);
	glColor4d(1.0, 1.0, 1.0, 0.2);
	harf_circle();
      }
      glPopMatrix();

      /* スクリーン仰角・俯角調節用 */
      glPushMatrix();
      {
	glTranslatef(0.0, screen_radius*sin(screen_up/180.0*M_PI), 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(screen_radius*cos(screen_up/180.0*M_PI),
		 screen_radius*cos(screen_up/180.0*M_PI),0.0);
	glColor4d(1.0, 1.0, 1.0, 0.2);
	circle();
      }
      glPopMatrix();

      glPushMatrix();
      {
	glTranslatef(0.0, screen_radius*sin(screen_down/180.0*M_PI), 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(screen_radius*cos(screen_down/180.0*M_PI),
		 screen_radius*cos(screen_down/180.0*M_PI),0.0);
	glColor4d(1.0, 1.0, 1.0, 0.2);
	circle();
      }
      glPopMatrix();

      if(toggle_cmirror){//凸面鏡を使用する場合のみ表示
        glPushMatrix();
        {
          glTranslatef(pr->cmirror.param[0],
                       pr->cmirror.param[1],
                       pr->cmirror.param[2]);
          glRotatef(pr->cmirror.param[4], 0.0, 1.0, 0.0);
          glRotatef(pr->cmirror.param[5], 0.0, 0.0, 1.0);
          glRotatef(pr->cmirror.param[3], 1.0, 0.0, 0.0);
          glPushMatrix();
          {
            //          glScalef(0.8768,0.8768,1.0);
            glutWireSphere(pr->cmirror_radius, 36,18);
          }
          glPopMatrix();
          glTranslatef(0.0, 0.0, -50.0);
          glColor4d(1.0, 0.0, 0.0, 1.0);
          glBegin(GL_LINES);
          glVertex3f(0.0, 0.0, 0.0);
          glVertex3f(0.0, 100.0, 0.0);
          glEnd();
        }
        glPopMatrix();
      }

      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);
    }
    glPopMatrix();

    /* kouro */
    glPointSize(3.0);
    glBegin(GL_POINTS);
    glVertex3fv(pr->screen.param);
    glVertex3fv(pr->cmirror.param);
    glVertex3fv(pr->pmirror.param);
    glVertex3fv(pr->projector.param);
    glEnd();

    glPushMatrix();
    {
      glTranslatef(pr->projector.param[0],
                   pr->projector.param[1],
                   pr->projector.param[2]);
      glRotatef(pr->projector.param[4], 0.0, 1.0, 0.0);
      glRotatef(pr->projector.param[5], 0.0, 0.0, 1.0);
      glRotatef(pr->projector.param[3], 1.0, 0.0, 0.0);

      if(toggle_real){
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        model_projector();
        //	model_mirror(pr);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_COLOR_MATERIAL);
      }
      glBegin(GL_POINTS);
      if(toggle_dispose){
        for(i=0;i<=NUM_I;i++)
          for(j=0;j<=NUM_J;j++){
            if(pr->ray[i][j].scr.refpos[0]!=0.0 ||
               pr->ray[i][j].scr.refpos[1]!=0.0){
              glColor3d(0.0, 1.0, 0.0);
              glVertex3fv(pr->ray[i][j].mir2.refpos);
            }
          }
      }else{
        glColor3d(0.0, 1.0, 0.0);
        for(i=0;i<=NUM_I;i++)
          for(j=0;j<=NUM_J;j++){
            if(pr->ray[i][j].active==1){
              if(toggle_cmirror){
                glColor3d(0.0, 1.0, 0.0);
                glVertex3fv(pr->ray[i][j].mir2.refpos);
              }
              glColor3d(0.0, 1.0, 1.0);
              glVertex3fv(pr->ray[i][j].scr.refpos);
            }
          }
      }
      glEnd();
      if(toggle_real){
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glColor4d(1.0, 1.0, 1.0, 0.7);
        glBindTexture(GL_TEXTURE_2D, lena.tex_id);

        for(i=0;i<NUM_I;i++){
          glEnable(GL_TEXTURE_2D);
          for(j=0;j<NUM_J;j++){

            glBegin(GL_POLYGON);
            if(pr->ray[i][j].active==1){
              glTexCoord2f((float)j/NUM_J, (float)i/NUM_I);
              glVertex3fv(pr->ray[i][j].scr.refpos);
            }
            if(pr->ray[i][j+1].active==1){
              glTexCoord2f((float)(j+1)/NUM_J, (float)i/NUM_I);
              glVertex3fv(pr->ray[i][j+1].scr.refpos);
            }
            if(pr->ray[i+1][j+1].active==1){
              glTexCoord2f((float)(j+1)/NUM_J, (float)(i+1)/NUM_I);
              glVertex3fv(pr->ray[i+1][j+1].scr.refpos);
            }
            if(pr->ray[i+1][j].active==1){
              glTexCoord2f((float)j/NUM_J, (float)(i+1)/NUM_I);
              glVertex3fv(pr->ray[i+1][j].scr.refpos);
            }
            glEnd();

          }
        }
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
      }
      if(toggle_focus){
        glPointSize(2.0);
        glBegin(GL_POINTS);
        glColor3d(1.0, 1.0, 0.0);
        for(i=0;i<=NUM_I;i++)
          for(j=0;j<=NUM_J;j++){
            if(pr->ray[i][j].active==1)
              glColor3d(1.0, 0.0, 1.0);
            else if(pr->ray[i][j].active==255)
              glColor3d(0.0, 0.0, 1.0);
            else
              glColor3d(1.0, 1.0, 0.0);
            //glVertex3fv(pr->ray[i][j].focus_pos);
          }
        glEnd();
        if(toggle_real){
          glEnable(GL_BLEND);
          glDepthMask(GL_FALSE);
          glBlendFunc(GL_SRC_ALPHA,GL_ONE);
          glColor4d(1.0, 1.0, 1.0, 0.4);
//          glColor3d(1.0, 1.0, 1.0);

          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, lena.tex_id);
          glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);
          glVertex3fv(pr->ray[0][0].focus_pos);
          glTexCoord2f(1.0, 0.0);
          glVertex3fv(pr->ray[0][NUM_J].focus_pos);
          glTexCoord2f(1.0, 1.0);
          glVertex3fv(pr->ray[NUM_I][NUM_J].focus_pos);
          glTexCoord2f(0.0, 1.0);
          glVertex3fv(pr->ray[NUM_I][0].focus_pos);
          glEnd();
          glDisable(GL_TEXTURE_2D);
          glDisable(GL_BLEND);
          glDepthMask(GL_TRUE);
        }
      }

      if(toggle_lens){
        glLineWidth(1.0);
        glColor3d(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        for(i=0;i<=NUM_I;i++)
          for(j=0;j<=NUM_J;j++)
            {
              if(pr->ray[i][j].active==1){
                glVertex3fv(pr->ray[i][j].mir2_lens[0].refpos);
                glVertex3fv(pr->ray[i][j].mir2_lens[1].refpos);
                glVertex3fv(pr->ray[i][j].mir2_lens[2].refpos);
                glVertex3fv(pr->ray[i][j].mir2_lens[3].refpos);
                glVertex3fv(pr->ray[i][j].scr_lens[0].refpos);
                glVertex3fv(pr->ray[i][j].scr_lens[1].refpos);
                glVertex3fv(pr->ray[i][j].scr_lens[2].refpos);
                glVertex3fv(pr->ray[i][j].scr_lens[3].refpos);
              }
            }
        glVertex3fv(lens[0]);
        glVertex3fv(lens[1]);
        glVertex3fv(lens[2]);
        glVertex3fv(lens[3]);
        glEnd();
      }
    }
    glPopMatrix();

  }
  glPopMatrix();
}

#define PI 3.1415925358979
void circle(void)
{
  double i;
  glBegin(GL_LINE_STRIP);
  for(i=0;i<=100;i++)
    glVertex3d(cos(i*PI/50.0), sin(i*PI/50.0), 0.0);
  glEnd();
}
void harf_circle(void)
{
  double i;
  glBegin(GL_LINE_STRIP);
  for(i=0;i<=50;i++)
    glVertex3d(0.0, cos(i*PI/50.0), -sin(i*PI/50.0));
  glEnd();
}

void Maininit (void)
{
  void initialize(void);
  GLfloat Position[] = {0.0, 10000.0, 10000.0, 1.0};
  GLfloat Specular[] = {0.2,0.2,0.2,1.0};
  GLfloat Shiness[] =  {0.2,0.2,0.2,1.0};
  GLfloat Diffuse[] =  {0.8,0.8,0.8,1.0};
  GLfloat Ambient[] =  {0.1,0.1,0.1,1.0};

  // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
  glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, Position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
  glLightfv(GL_LIGHT0, GL_SHININESS, Shiness);

  glEnable(GL_DEPTH_TEST);
  glShadeModel (GL_SMOOTH);
  glEnable(GL_NORMALIZE);
  glDepthFunc(GL_LEQUAL);

  projection1.ray = (RAY (*)[NUM_J+1])malloc(
    sizeof(RAY) * (NUM_I+1) * (NUM_J+1));
  projection2.ray = (RAY (*)[NUM_J+1])malloc(
    sizeof(RAY) * (NUM_I+1) * (NUM_J+1));

  projector_load(&projection1);
  projector_load(&projection2);
  projection_load(&projection1);
  projection_load(&projection2);
  focus_initialize(&projection1);
  focus_initialize(&projection2);
  calc_screen(&projection1);
  calc_screen(&projection2);

//  checker_texture(&checker);
  open_tga_texture("lena_std.tga",&lena,512,512);
}

void MainReshape(GLsizei w, GLsizei h)
{
  main_window_w=w;main_window_h=h;
  /* 射影変換をおこなう */
  glMatrixMode (GL_PROJECTION);   /*  prepare for and then  */
  glLoadIdentity ();  /*  define the projection  */
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10000.0);
//  glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 100000.0);
  glMatrixMode (GL_MODELVIEW);    /*  back to modelview matrix    */
}


void RedisplayBoth(void)
{
  glutPostWindowRedisplay(MainWindow);
  glutPostWindowRedisplay(TableWindow);
  glui->sync_live();
  Sleep(10);
}


void copy_mirror_data(int tmp)
{
  //アクティブになっているプロジェクタのミラー系パラメータをコピーする
  if(toggle_projection1>toggle_projection2){
    P1TOP2_CM(0);      P1TOP2_CM(1);      P1TOP2_CM(2);
    P1TOP2_CM(3);      P1TOP2_CM(4);      P1TOP2_CM(5);

    P1TOP2_PM(0);      P1TOP2_PM(1);      P1TOP2_PM(2);
    P1TOP2_PM(3);      P1TOP2_PM(4);      P1TOP2_PM(5);
    calc_screen(&projection2);
  }

  if(toggle_projection2>toggle_projection1){
    P2TOP1_CM(0);      P2TOP1_CM(1);      P2TOP1_CM(2);
    P2TOP1_CM(3);      P2TOP1_CM(4);      P2TOP1_CM(5);

    P2TOP1_PM(0);      P2TOP1_PM(1);      P2TOP1_PM(2);
    P2TOP1_PM(3);      P2TOP1_PM(4);      P2TOP1_PM(5);
    calc_screen(&projection1);
  }
}


int main(int argc, char** argv)
{ 
  void glui_main(void);
  /* Main window */
  glutInitWindowSize (700, 700);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  if((MainWindow=glutCreateWindow ("Virtual Projector")) == GL_FALSE)
    exit(0);

  sprintf(projection1.filename,"status1");
  sprintf(projection2.filename,"status2");

  sprintf(projection1.proj_filename,"projector1.txt");
  sprintf(projection2.proj_filename,"projector1.txt");

  time(&time_start);
  Maininit ();

  ControlMouse_track();
  glutReshapeFunc(MainReshape);
  glutKeyboardFunc(Key);
  MainWinMenu();
  glutDisplayFunc(display);
  glui_main();
  StatusWinMenu();
  
  glutInitWindowPosition(0, 0);
  glutInitWindowSize (512, 384);
  if((TableWindow=glutCreateWindow ("Table")) == GL_FALSE)
    exit(0);

  init_table(projection1.filename);
  glutDisplayFunc(display_table);
  init_texture();
  
  glutIdleFunc(RedisplayBoth);
  glutMainLoop();
  return(0);
}

