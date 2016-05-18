/********************************************************/
/*	Virtual Projector main calculation (vproj.c)    */
/*	needs matrixf.h vproj.h        			*/
/*	Original coded by whashimo	        	*/
/*	2001 Version. 2001/01/15     	        	*/
/********************************************************/

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "matrixf.h"
#define VPROJ_C
#include "vproj.h"
#include <stdlib.h>
#include <windows.h>
void exit_prog(int);//配列のクリア


extern int toggle_cmirror;
extern int toggle_mmirror;

/* パラメータをファイルに保存する */
void projection_save(PROJECTION pr)
{
  FILE *fp_status;
  float orig[3]={0.0, 0.0, 0.0};
  int i,j;
  float _tmp[3];
  float h_gakaku[3]={9999.0,9999.0,9999.0},gyokaku=0.0,fukaku=0.0;
  float tmp_matrix[16];

  if((fp_status=fopen(pr.filename,"w"))==NULL)
    exit_prog(1);

  /* 一般配置情報 */
  /* レンズ面中心 */
  fprintf(fp_status,"れんず面中心位置\n%lf %lf %lf\n",
	  pr.projector.param[0],
	  pr.projector.param[1],
	  pr.projector.param[2]);

  /* プロジェクタの傾き角 (水平に対して) */
  fprintf(fp_status,"ぷろじぇくた傾き角\n%lf %lf %lf\n",
	  pr.projector.param[3],
	  pr.projector.param[4],
	  pr.projector.param[5]);


  /* 第一反射鏡 位置(x, y) */
  fprintf(fp_status,"第一反射鏡位置\n%lf %lf %lf\n",
	  pr.pmirror.param[0],
	  pr.pmirror.param[1],
	  pr.pmirror.param[2]);

  /* 第一反射鏡角度 */
  fprintf(fp_status,"第一反射鏡角度\n%lf %lf %lf\n",
	  pr.pmirror.param[3],
	  pr.pmirror.param[4],
	  pr.pmirror.param[5]);
  
  /* 凸面鏡パラメータ(中心x,中心y,曲率半径) */
  fprintf(fp_status,"凸面鏡中心位置\n%lf %lf %lf\n曲率半径\n%lf\n",
	  pr.cmirror.param[0],
	  pr.cmirror.param[1],
	  pr.cmirror.param[2], 
	  pr.cmirror_radius);

  fprintf(fp_status,"凸面鏡角度\n%lf %lf %lf\n",
	  pr.cmirror.param[3],
	  pr.cmirror.param[4],
	  pr.cmirror.param[5]);

  /* スクリーンパラメータ */
  fprintf(fp_status,"すくりーん中心位置\n%lf %lf %lf\n",
	  pr.screen.param[0],
	  pr.screen.param[1],
	  pr.screen.param[2]);

  /* プロジェクタの傾き角 (水平に対して) */
  fprintf(fp_status,"すくりーん傾き角\n%lf %lf %lf\n",
	  pr.screen.param[3],
	  pr.screen.param[4],
	  pr.screen.param[5]);

  /* 全有効結像点の数 */
  fprintf(fp_status,"有効結像点割合\n %d / %d\n", 
	  pr.vi, (NUM_I+1)*(NUM_J+1));

  /* スクリーン上の結像点 */
  multi_mat(pr.screen.matrix_inv, pr.projector.matrix, tmp_matrix, 4);
  for(i=0;i<=NUM_I;i++){
    for(j=0;j<=NUM_J;j++){
      if(pr.ray[i][j].active){
	multi_vec(tmp_matrix, pr.ray[i][j].scr.refpos, _tmp);
	fprintf(fp_status,"%lf %lf %lf\n", _tmp[0], _tmp[1], _tmp[2]);
	multi_vec(tmp_matrix, pr.ray[i][j].mir2.refpos, _tmp);
        if(h_gakaku[1]>_tmp[1]){
          h_gakaku[0]=_tmp[0];
          h_gakaku[1]=_tmp[1];
          h_gakaku[2]=_tmp[2];
        }
      }
    }
    if(!pr.ray[i][NUM_J/2].active)
      continue;
    multi_vec(tmp_matrix, pr.ray[i][NUM_J/2].scr.refpos, _tmp);
    if(gyokaku<asin(_tmp[0]/screen_radius))
      gyokaku=asin(_tmp[0]/screen_radius);
    if(fukaku>asin(_tmp[0]/screen_radius))
      fukaku=asin(_tmp[0]/screen_radius);
  }    
  fprintf(fp_status,"正面仰角 %.2lf 正面俯角 %.2lf 最大水平角度 %.2lf\n",
	  gyokaku*180.0/M_PI,fukaku*180.0/M_PI,
	  acos(h_gakaku[1]/screen_radius)*180.0/M_PI);
#if 0
  /* レンズ面特徴点 */
  for(i=0;i<4;i++){
    multi_vec(pr.projector.matrix, lens[i], _tmp);
    fprintf(fp_status,"lens%d: %lf %lf %lf\n",
	    i, _tmp[0], _tmp[1], _tmp[2]);
  }
#endif

  fclose(fp_status);

  {
    /* 平面鏡の交点(平面鏡座標系) */
    FILE *fp_pmirror;
    char filename_add[100];
    sprintf(filename_add, "%s.pmirror", pr.filename);
    if((fp_pmirror=fopen(filename_add,"w"))==NULL)
      exit_prog(1);
    multi_mat(pr.pmirror.matrix_inv, pr.projector.matrix, tmp_matrix, 4);
    for(i=0;i<=NUM_I;i++){
      for(j=0;j<=NUM_J;j++){
	if(pr.ray[i][j].active){
	  multi_vec(tmp_matrix, pr.ray[i][j].mir1.refpos, _tmp);
	  fprintf(fp_pmirror,"%d %lf %lf %lf\n", pr.ray[i][j].active,
		  _tmp[0], _tmp[1], _tmp[2]);
	}
      }
    }
    fclose(fp_pmirror);
  }

 {
   /* 凸面鏡の交点(凸面鏡座標系) */
   FILE *fp_cmirror;
   char filename_add[100];
   sprintf(filename_add, "%s.cmirror", pr.filename);
   if((fp_cmirror=fopen(filename_add,"w"))==NULL)
      exit_prog(1);
   multi_mat(pr.cmirror.matrix_inv, pr.projector.matrix, tmp_matrix, 4);
   for(i=0;i<=NUM_I;i++){
     for(j=0;j<=NUM_J;j++){
	if(pr.ray[i][j].active){
	  multi_vec(tmp_matrix, pr.ray[i][j].mir2.refpos, _tmp);
	  fprintf(fp_cmirror,"%d %lf %lf %lf\n", pr.ray[i][j].active,
	       _tmp[0], _tmp[1], _tmp[2]);
	}
     }
   }
   fclose(fp_cmirror);
 }
}



/* フォーカス面設定 */
void focus_initialize(PROJECTION *pr)
{
  int i,j;

  for(i=0;i<=NUM_I;i++){
    for(j=0;j<=NUM_J;j++){
      pr->ray[i][j].focus_pos[0]=
        ((pr->projector_right- pr->projector_left)*((float)j/((float)NUM_J))
         +pr->projector_left)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[1]=
        ((pr->projector_top- pr->projector_bottom)*((float)i/((float)NUM_I))
         +pr->projector_bottom)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[2]=-pr->projector_focus;
    }
  }
  
  for(i=0;i<=NUM_I-1;i++){
    for(j=0;j<=NUM_J-1;j++){
      /* フォーカス点における格子面積算出 */
      pr->ray[i][j].area_focus=
        polygon_surface(pr->ray[i][j].focus_pos,
                        pr->ray[i][j+1].focus_pos,
                        pr->ray[i+1][j].focus_pos,
                        pr->ray[i+1][j+1].focus_pos);
      /* フォーカス面における光束の量算出 */
      pr->ray[i][j].area_kousoku=PROJ_LUMEN*
        pr->ray[i][j].area_focus/
          (pr->projector_right - pr->projector_left)*pr->projector_zoomratio*
            (pr->projector_top - pr->projector_bottom)*pr->projector_zoomratio;
      /* 後半はフォーカス面の面積 */
    }
  }


  
#if 0
  for(i=0;i<=NUM_I;i++){
    for(j=0;j<=NUM_J;j++){
      /* width */
      pr->ray[i][j].focus_pos[0]=pr->projector_focus*tan(H_VIEW)*2.0*
	((float)j/((float)NUM_J)-0.50)*pr->projector_zoomratio;
      /* height */
      pr->ray[i][j].focus_pos[1]=pr->projector_focus*
        (tan(V_VIEW)*(float)i/(float)NUM_I+tan(V_VIEW_OS))*
          pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[2]=-pr->projector_focus;
    }
  }
  
  for(i=0;i<=NUM_I-1;i++){
    for(j=0;j<=NUM_J-1;j++){
      /* フォーカス点における格子面積算出 */
      pr->ray[i][j].area_focus=
        polygon_surface(pr->ray[i][j].focus_pos,
                        pr->ray[i][j+1].focus_pos,
                        pr->ray[i+1][j].focus_pos,
                        pr->ray[i+1][j+1].focus_pos);
      /* フォーカス面における光束の量算出 */
      pr->ray[i][j].area_kousoku=PROJ_LUMEN*
        pr->ray[i][j].area_focus/
          ((pr->projector_focus*tan(V_VIEW)*pr->projector_zoomratio)*
           (pr->projector_focus*tan(H_VIEW)*2.0*pr->projector_zoomratio));
      /* 後半はフォーカス面の面積 */
    }
  }
#endif
  pr->lux_min=9999999999.9;
  pr->lux_max=0.0;
  pr->pix_min=9999999999.9;
  pr->pix_max=0.0;
}

void projector_load(PROJECTION *pr)
{
  FILE *fp_proj;
  char dummy[100];
  
  if((fp_proj=fopen(pr->proj_filename,"r"))==NULL)
      exit_prog(1);
  
  fscanf(fp_proj,"%s", dummy);
  fscanf(fp_proj,"%s", dummy);//bottom
  fscanf(fp_proj,"%f", &pr->projector_bottom);
  fscanf(fp_proj,"%s", dummy);//top
  fscanf(fp_proj,"%f", &pr->projector_top);
  fscanf(fp_proj,"%s", dummy);//left
  fscanf(fp_proj,"%f", &pr->projector_left);
  fscanf(fp_proj,"%s", dummy);//right
  fscanf(fp_proj,"%f", &pr->projector_right);
  fscanf(fp_proj,"%s", dummy);//near
  fscanf(fp_proj,"%f", &pr->projector_focus);
  fscanf(fp_proj,"%s", dummy);//near
  fscanf(fp_proj,"%f", &pr->projector_zoomratio);

  fclose(fp_proj);
}

void projector_save(PROJECTION pr)
{
  FILE *fp_proj;
  char dummy[100];
  
  if((fp_proj=fopen(pr.proj_filename,"w"))==NULL)
    exit_prog(1);

  fprintf(fp_proj,"プロジェクタパラメータファイル\n");
  fprintf(fp_proj,"bottom\n");
  fprintf(fp_proj,"%f\n", pr.projector_bottom);
  fprintf(fp_proj,"top\n");
  fprintf(fp_proj,"%f\n", pr.projector_top);
  fprintf(fp_proj,"left\n");
  fprintf(fp_proj,"%f\n", pr.projector_left);
  fprintf(fp_proj,"right\n");
  fprintf(fp_proj,"%f\n", pr.projector_right);
  fprintf(fp_proj,"near\n");
  fprintf(fp_proj,"%f\n", pr.projector_focus);
  fprintf(fp_proj,"zoom\n");
  fprintf(fp_proj,"%f\n", pr.projector_zoomratio);

  fclose(fp_proj);
}

/* パラメータをファイルから読み出す */
void projection_load(PROJECTION *pr)
{
  FILE *fp_status;
  char dummy[100];

  if((fp_status=fopen(pr->filename,"r"))==NULL)
    exit_prog(1);

  /* 一般配置情報 */
  /* レンズ面中心 */
  fscanf(fp_status,"%s", dummy);/*れんず面中心位置*/
  fscanf(fp_status,"%f %f %f",
	  &pr->projector.param[0],
	  &pr->projector.param[1],
	  &pr->projector.param[2]);
  
  /* プロジェクタの傾き角 (水平に対して) */
  fscanf(fp_status,"%s", dummy);/*ぷろじぇくた傾き角*/
  fscanf(fp_status,"%f %f %f",
	  &pr->projector.param[3],
	  &pr->projector.param[4],
	  &pr->projector.param[5]);

  /* 第一反射鏡 位置(x, y) */
  fscanf(fp_status,"%s", dummy);/*第一反射鏡 位置*/
  fscanf(fp_status,"%f %f %f",
	  &pr->pmirror.param[0],
	  &pr->pmirror.param[1],
	  &pr->pmirror.param[2]);

  /* 第一反射鏡角度 */
  fscanf(fp_status,"%s", dummy);/*第一反射鏡 角度*/
  fscanf(fp_status,"%f %f %f",
	 &pr->pmirror.param[3],
	 &pr->pmirror.param[4],
	 &pr->pmirror.param[5]);
  
  /* 凸面鏡パラメータ(中心x,中心y,曲率半径) */
  fscanf(fp_status,"%s", dummy);/*凸面鏡 中心位置*/
  fscanf(fp_status,"%f %f %f",
	  &pr->cmirror.param[0],
	  &pr->cmirror.param[1],
	  &pr->cmirror.param[2]);

  fscanf(fp_status,"%s", dummy);/*曲率半径*/
  fscanf(fp_status,"%f",
	  &pr->cmirror_radius);

  fscanf(fp_status,"%s", dummy);/*凸面鏡 角度*/
  fscanf(fp_status,"%f %f %f",
	  &pr->cmirror.param[3],
	  &pr->cmirror.param[4],
	  &pr->cmirror.param[5]);

  /* スクリーンパラメータ */
  fscanf(fp_status,"%s", dummy);/*すくりーん中心位置*/
  fscanf(fp_status,"%f %f %f",
	  &pr->screen.param[0],
	  &pr->screen.param[1],
	  &pr->screen.param[2]);

  /* プロジェクタの傾き角 (水平に対して) */
  fscanf(fp_status,"%s", dummy);/*すくりーん傾き角*/
  fscanf(fp_status,"%f %f %f",
	  &pr->screen.param[3],
	  &pr->screen.param[4],
	  &pr->screen.param[5]);

  fclose(fp_status);
}

/* 撮像系シミュレーション用参照データ保存 */
void projection_calc_save(PROJECTION pr)
{
  FILE *fp_status;
  int i,j;
  float _tmp[3];
  float tmp_matrix[16];
  char filename_add[100];

  sprintf(filename_add, "%s_calc.dat", pr.filename);
  if((fp_status=fopen(filename_add,"w"))==NULL)
    exit_prog(1);
  fprintf(fp_status,"%d %d\n", NUM_I+1, NUM_J+1);

  /* スクリーン上の結像点 */
  multi_mat(pr.screen.matrix_inv, pr.projector.matrix, tmp_matrix, 4);
  for(i=0;i<=NUM_I;i++){
    for(j=0;j<=NUM_J;j++){
      if(pr.ray[i][j].active){
	multi_vec(tmp_matrix, pr.ray[i][j].scr.refpos, _tmp);
        fprintf(fp_status,"%lf %lf %lf %lf %lf\n",
                (float)j/((float)NUM_J),        /* width */
                (float)i/((float)NUM_I),        /* height */
                _tmp[0], _tmp[1], _tmp[2]);
	multi_vec(tmp_matrix, pr.ray[i][j].mir2.refpos, _tmp);
      }
    }
    if(!pr.ray[i][NUM_J/2].active)
      continue;
  }    

  fclose(fp_status);
}

/* 光線追跡本体 */
int calc_screen(PROJECTION *pr)
{
  int i,j;
  /* プロジェクタ光源原点 */
  float orig[3];
  float mirror[4],ellipse1[10],ellipse2[10];
  float tmp_matrix[16];
  int flag;
  static int count=0;

  LARGE_INTEGER startTime;
  LARGE_INTEGER endTime;
  static int sumTime=0;

  LARGE_INTEGER ticksPerSec;

  QueryPerformanceCounter(&startTime);
  
  /* スクリーンの位置姿勢を行列Msに変換 */
  hand_mat(pr->screen.param, pr->screen.matrix);
  /* プロジェクタの位置姿勢を行列Mpに変換 */ 
  hand_mat(pr->projector.param, pr->projector.matrix);
  /* 平面鏡の位置姿勢を行列Mhに変換 */
  hand_mat(pr->pmirror.param, pr->pmirror.matrix);
  /* 凸面鏡の位置姿勢を行列Mcに変換 */
  hand_mat(pr->cmirror.param, pr->cmirror.matrix);

  /* スクリーンの位置姿勢の逆行列Ms-1算出 */
  inv_homoge_mat(pr->screen.matrix, pr->screen.matrix_inv);
  /* プロジェクタの位置姿勢の逆行列Mp-1算出 */
  inv_homoge_mat(pr->projector.matrix, pr->projector.matrix_inv);
  /* 平面鏡の位置姿勢の逆行列Mh-1算出 */
  inv_homoge_mat(pr->pmirror.matrix, pr->pmirror.matrix_inv);
  /* 凸面鏡の位置姿勢の逆行列Mc-1算出 */
  inv_homoge_mat(pr->cmirror.matrix, pr->cmirror.matrix_inv);

  /* 平面鏡の式z=0をプロジェクタ座標系に変換する */
  /* Mh-1xMp のをかけたときのzの値 */
  multi_mat(pr->pmirror.matrix_inv, pr->projector.matrix, tmp_matrix, 4);
  mirror[0]=tmp_matrix[8];
  mirror[1]=tmp_matrix[9];
  mirror[2]=tmp_matrix[10];
  mirror[3]=tmp_matrix[11];

  /* プロジェクタ座標系->スクリーン座標系変換行列 Ms-1xMp */
  multi_mat(pr->screen.matrix_inv, pr->projector.matrix, pr->matrix_p2s, 4);

  /* 凸面鏡の中心位置をプロジェクタ座標系に変換する */
  /* Mc-1xMp */
  multi_mat(pr->cmirror.matrix_inv, pr->projector.matrix, tmp_matrix, 4);
#define ELLIPSE_X 0.75
#define ELLIPSE_Y 0.75
#define ELLIPSE_Z 1.0

  ellipse1[0]=
    ELLIPSE_X*tmp_matrix[0]*tmp_matrix[0]+
      ELLIPSE_Y*tmp_matrix[4]*tmp_matrix[4]+
        ELLIPSE_Z*tmp_matrix[8]*tmp_matrix[8];
  ellipse1[1]=
    ELLIPSE_X*tmp_matrix[1]*tmp_matrix[1]+
      ELLIPSE_Y*tmp_matrix[5]*tmp_matrix[5]+
        ELLIPSE_Z*tmp_matrix[9]*tmp_matrix[9];
  ellipse1[2]=
    ELLIPSE_X*tmp_matrix[2]*tmp_matrix[2]+
      ELLIPSE_Y*tmp_matrix[6]*tmp_matrix[6]+
        ELLIPSE_Z*tmp_matrix[10]*tmp_matrix[10];
  
  ellipse1[3]=
    2.0*ELLIPSE_X*tmp_matrix[0]*tmp_matrix[1]+
    2.0*ELLIPSE_Y*tmp_matrix[4]*tmp_matrix[5]+
    2.0*ELLIPSE_Z*tmp_matrix[8]*tmp_matrix[9];
  ellipse1[4]=
    2.0*ELLIPSE_X*tmp_matrix[1]*tmp_matrix[2]+
    2.0*ELLIPSE_Y*tmp_matrix[5]*tmp_matrix[6]+
    2.0*ELLIPSE_Z*tmp_matrix[9]*tmp_matrix[10];
  ellipse1[5]=
    2.0*ELLIPSE_X*tmp_matrix[2]*tmp_matrix[0]+
    2.0*ELLIPSE_Y*tmp_matrix[6]*tmp_matrix[4]+
    2.0*ELLIPSE_Z*tmp_matrix[10]*tmp_matrix[8];
  
  ellipse1[6]=
    2.0*ELLIPSE_X*tmp_matrix[0]*tmp_matrix[3]+
    2.0*ELLIPSE_Y*tmp_matrix[4]*tmp_matrix[7]+
    2.0*ELLIPSE_Z*tmp_matrix[8]*tmp_matrix[11];
  ellipse1[7]=
    2.0*ELLIPSE_X*tmp_matrix[1]*tmp_matrix[3]+
    2.0*ELLIPSE_Y*tmp_matrix[5]*tmp_matrix[7]+
    2.0*ELLIPSE_Z*tmp_matrix[9]*tmp_matrix[11];
  ellipse1[8]=
    2.0*ELLIPSE_X*tmp_matrix[2]*tmp_matrix[3]+
    2.0*ELLIPSE_Y*tmp_matrix[6]*tmp_matrix[7]+
    2.0*ELLIPSE_Z*tmp_matrix[10]*tmp_matrix[11];

  ellipse1[9]=
    ELLIPSE_X*tmp_matrix[3]*tmp_matrix[3]+
    ELLIPSE_Y*tmp_matrix[7]*tmp_matrix[7]+
    ELLIPSE_Z*tmp_matrix[11]*tmp_matrix[11]-
      pr->cmirror_radius*pr->cmirror_radius;
  
  /* スクリーンの中心位置をプロジェクタ座標系に変換する */
  /* Ms-1xMp */
  multi_mat(pr->projector.matrix_inv, pr->screen.matrix, tmp_matrix, 4);
  ellipse2[0]=1.0;ellipse2[1]=1.0;ellipse2[2]=1.0;
  
  ellipse2[3]=ellipse2[4]=ellipse2[5]=0.0;
  ellipse2[6]=-2.0*tmp_matrix[3]; 
  ellipse2[7]=-2.0*tmp_matrix[7]; 
  ellipse2[8]=-2.0*tmp_matrix[11];
  ellipse2[9]=
    tmp_matrix[3]*tmp_matrix[3]
    +tmp_matrix[7]*tmp_matrix[7]
    +tmp_matrix[11]*tmp_matrix[11]
    -screen_radius*screen_radius;

  /* プロジェクタ原点はズームによって前後に移動する */
  orig[0]=0.0;
  orig[1]=0.0;
  orig[2]=0.0;
  /*ZOOM_T_BACK+(ZOOM_T_BACK+ZOOM_W_BACK)*(pr->projector_zoomratio-1.0);*/

  pr->all_sum=0.0;
  pr->vi=0;
  for(j=0;j<=NUM_J;j++){
    for(i=0;i<=NUM_I;i++){
      float begin_vec[3];
      /* 有効画素セット */
      pr->ray[i][j].active=1;
      flag=1;
      
      begin_vec[0]=pr->ray[i][j].focus_pos[0]-orig[0];
      begin_vec[1]=pr->ray[i][j].focus_pos[1]-orig[1];
      begin_vec[2]=pr->ray[i][j].focus_pos[2]-orig[2];

      if(toggle_cmirror){//
        /* 主光線追跡(プロジェクタから凸面鏡へ) */
        if(store_points(ellipse1,
                        orig,
                        begin_vec,
                        pr->ray[i][j].mir2.refpos,
                        pr->ray[i][j].mir2.refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        /* 主光線追跡(凸面鏡からスクリーンへ) */
        if(flag){
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2.refpos,
                              pr->ray[i][j].mir2.refvec,
                              pr->ray[i][j].scr.refpos,
                              pr->ray[i][j].scr.refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
        }else{
          if(store_points(ellipse2,
                          pr->ray[i][j].mir2.refpos,
                          pr->ray[i][j].mir2.refvec,
                          pr->ray[i][j].scr.refpos,
                          pr->ray[i][j].scr.refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
        }
      }else{
        /* 主光線追跡(凸面鏡からスクリーンへ) */
        if(store_points_ext(ellipse2,
                            orig,
                            begin_vec,
                            pr->ray[i][j].scr.refpos,
                            pr->ray[i][j].scr.refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
      }
      /* スクリーン干渉チェック */
      /* スクリーンの形にあたっているかどうかをチェック */
      {
	float _tmp[3];
	float alpha, beta;
        /* 光線追跡の結果をプロジェクタ座標系からスクリーン座標系に変換 */
        multi_vec(pr->matrix_p2s, pr->ray[i][j].scr.refpos, _tmp);
	beta=asin(_tmp[1]/screen_radius);
	alpha=atan2(_tmp[0],-_tmp[2]);

	if(alpha < -(screen_view+1.0)/180.0*M_PI ||
	   alpha > (screen_view+1.0)/180.0*M_PI){
	  pr->ray[i][j].active=0;
	  continue;
	}
	if(beta > screen_up/180.0*M_PI || beta < screen_down/180.0*M_PI){
	  /*近いほうの点がスクリーンにあたっていない場合*/
	  /*遠いほうの点について調べる*/
            pr->ray[i][j].active=0;
        }
      }
      
      begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[0][0];
      begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[0][1];
      begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[0][2];
      /* レンズ面光線追跡 */
      if(toggle_cmirror){
        if(store_points(ellipse1,
                        lens[0],
                        begin_vec,
                        pr->ray[i][j].mir2_lens[0].refpos,
                        pr->ray[i][j].mir2_lens[0].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[1][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[1][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[1][2];
        if(store_points(ellipse1,
                        lens[1],
                        begin_vec,
                        pr->ray[i][j].mir2_lens[1].refpos,
                        pr->ray[i][j].mir2_lens[1].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[2][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[2][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[2][2];
        if(store_points(ellipse1,
                        lens[2],
                        begin_vec,
                        pr->ray[i][j].mir2_lens[2].refpos,
                        pr->ray[i][j].mir2_lens[2].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[3][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[3][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[3][2];
        if(store_points(ellipse1,
                        lens[3],
                        begin_vec,
                        pr->ray[i][j].mir2_lens[3].refpos,
                        pr->ray[i][j].mir2_lens[3].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        if(flag){
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2_lens[0].refpos,
                              pr->ray[i][j].mir2_lens[0].refvec,
                              pr->ray[i][j].scr_lens[0].refpos,
                              pr->ray[i][j].scr_lens[0].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2_lens[1].refpos,
                              pr->ray[i][j].mir2_lens[1].refvec,
                              pr->ray[i][j].scr_lens[1].refpos,
                              pr->ray[i][j].scr_lens[1].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2_lens[2].refpos,
                              pr->ray[i][j].mir2_lens[2].refvec,
                              pr->ray[i][j].scr_lens[2].refpos,
                              pr->ray[i][j].scr_lens[2].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2_lens[3].refpos,
                              pr->ray[i][j].mir2_lens[3].refvec,
                              pr->ray[i][j].scr_lens[3].refpos,
                              pr->ray[i][j].scr_lens[3].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
        }else{
          if(store_points(ellipse2,
                          pr->ray[i][j].mir2_lens[0].refpos,
                          pr->ray[i][j].mir2_lens[0].refvec,
                          pr->ray[i][j].scr_lens[0].refpos,
                          pr->ray[i][j].scr_lens[0].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points(ellipse2,
                          pr->ray[i][j].mir2_lens[1].refpos,
                          pr->ray[i][j].mir2_lens[1].refvec,
                          pr->ray[i][j].scr_lens[1].refpos,
                          pr->ray[i][j].scr_lens[1].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points(ellipse2,
                          pr->ray[i][j].mir2_lens[2].refpos,
                          pr->ray[i][j].mir2_lens[2].refvec,
                          pr->ray[i][j].scr_lens[2].refpos,
                          pr->ray[i][j].scr_lens[2].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
          if(store_points(ellipse2,
                          pr->ray[i][j].mir2_lens[3].refpos,
                          pr->ray[i][j].mir2_lens[3].refvec,
                          pr->ray[i][j].scr_lens[3].refpos,
                          pr->ray[i][j].scr_lens[3].refvec)){
            pr->ray[i][j].active=0;
            continue;
          }
        }
      }else{
        if(store_points_ext(ellipse2,
                            lens[0],
                            begin_vec,
                            pr->ray[i][j].scr_lens[0].refpos,
                            pr->ray[i][j].scr_lens[0].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[1][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[1][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[1][2];
        if(store_points_ext(ellipse2,
                            lens[1],
                            begin_vec,
                            pr->ray[i][j].scr_lens[1].refpos,
                            pr->ray[i][j].scr_lens[1].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[2][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[2][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[2][2];
        if(store_points_ext(ellipse2,
                            lens[2],
                            begin_vec,
                            pr->ray[i][j].scr_lens[2].refpos,
                            pr->ray[i][j].scr_lens[2].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
        begin_vec[0]=pr->ray[i][j].focus_pos[0]-lens[3][0];
        begin_vec[1]=pr->ray[i][j].focus_pos[1]-lens[3][1];
        begin_vec[2]=pr->ray[i][j].focus_pos[2]-lens[3][2];
        if(store_points_ext(ellipse2,
                            lens[3],
                            begin_vec,
                            pr->ray[i][j].scr_lens[3].refpos,
                            pr->ray[i][j].scr_lens[3].refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
      }
    }
  }
  

  pr->view_up=0.0;
  pr->view_low=0.0;
  pr->view_right=0.0;
  pr->view_left=0.0;

  for(j=0;j<=NUM_J;j++){
    for(i=0;i<=NUM_I;i++){
      float view_tmp, raypos[3];

      /*結像評価*/
      if(pr->ray[i][j].active==1){
        /* レンズ特徴点の大きさの指標 */
        pr->all_sum+=(DIST(pr->ray[i][j].scr_lens[0].refpos[0],
		       pr->ray[i][j].scr_lens[0].refpos[1],
		       pr->ray[i][j].scr_lens[0].refpos[2],
		       pr->ray[i][j].scr_lens[1].refpos[0],
		       pr->ray[i][j].scr_lens[1].refpos[1],
		       pr->ray[i][j].scr_lens[1].refpos[2])+
		  DIST(pr->ray[i][j].scr_lens[2].refpos[0],
		       pr->ray[i][j].scr_lens[2].refpos[1],
		       pr->ray[i][j].scr_lens[2].refpos[2],
		       pr->ray[i][j].scr_lens[3].refpos[0],
		       pr->ray[i][j].scr_lens[3].refpos[1],
		       pr->ray[i][j].scr_lens[3].refpos[2]));
	pr->vi++;
      }

      if(pr->ray[i][j].active){
        //multi_vec(pr->matrix_p2s, pr->ray[i][j].scr.refpos, raypos);
        multi_vec(pr->projector.matrix, pr->ray[i][j].scr.refpos, raypos);
//        multi_vec(pr->screen.matrix_inv, pr->ray[i][j].scr.refpos, raypos);
        
        /*補正テーブル用の画角算出*/
        view_tmp= 180.0/M_PI*
          atan2(raypos[1]-view_pos[1],
                        sqrt(
                          (raypos[0]-view_pos[0])*
                          (raypos[0]-view_pos[0])+
                          (raypos[2]-view_pos[2])*
                          (raypos[2]-view_pos[2])));
        if(pr->view_up<view_tmp)
          pr->view_up=view_tmp;//最大値を求める．
        if(pr->view_low>view_tmp)
          pr->view_low=view_tmp;//最小値を求める．

        view_tmp= 180.0/M_PI*
          atan2(raypos[0]-view_pos[0],
                -raypos[2]+view_pos[2]);
        if(pr->view_right<view_tmp)
          pr->view_right=view_tmp;
        if(pr->view_left>view_tmp)
          pr->view_left=view_tmp;
      }
      
    }
  }

  for(j=0;j<NUM_J;j++){
    for(i=0;i<NUM_I;i++){
      float pixdist_tmp;
  
      if(pr->ray[i][j].active==1 && pr->ray[i][j+1].active==1){
        pixdist_tmp=DIST(pr->ray[i][j].scr.refpos[0],
                         pr->ray[i][j].scr.refpos[1],
                         pr->ray[i][j].scr.refpos[2],
                         pr->ray[i][j+1].scr.refpos[0],
                         pr->ray[i][j+1].scr.refpos[1],
                         pr->ray[i][j+1].scr.refpos[2]);
        if(pixdist_tmp>pr->pix_max)
          pr->pix_max=pixdist_tmp;
        if(pixdist_tmp<pr->pix_min)
          pr->pix_min=pixdist_tmp;
      }
      if(pr->ray[i][j].active==1 && pr->ray[i+1][j].active==1){
        pixdist_tmp=DIST(pr->ray[i][j].scr.refpos[0],
                         pr->ray[i][j].scr.refpos[1],
                         pr->ray[i][j].scr.refpos[2],
                         pr->ray[i+1][j].scr.refpos[0],
                         pr->ray[i+1][j].scr.refpos[1],
                         pr->ray[i+1][j].scr.refpos[2]);
        if(pixdist_tmp>pr->pix_max)
          pr->pix_max=pixdist_tmp;
        if(pixdist_tmp<pr->pix_min)
          pr->pix_min=pixdist_tmp;
      }
      /* 照度 */
      if(pr->ray[i][j].active==1 && pr->ray[i][j+1].active==1 &&
         pr->ray[i+1][j].active==1 && pr->ray[i+1][j+1].active==1){
        pr->ray[i][j].area_screen=
          polygon_surface(pr->ray[i][j].scr.refpos,
                          pr->ray[i][j+1].scr.refpos,
                          pr->ray[i+1][j].scr.refpos,
                          pr->ray[i+1][j+1].scr.refpos);
        /* スクリーン面における照度算出 */
        pr->ray[i][j].area_syoudo=pr->ray[i][j].area_kousoku/
          (pr->ray[i][j].area_screen/(1000.0*1000.0));
#if 0
        printf("%.2f %.2f %.2f %.2f\n",
               pr->ray[i][j].area_focus,
               pr->ray[i][j].area_kousoku,
               pr->ray[i][j].area_screen,
               pr->ray[i][j].area_syoudo);
#endif
        if(pr->ray[i][j].area_syoudo>pr->lux_max)
          pr->lux_max=pr->ray[i][j].area_syoudo;
        if(pr->ray[i][j].area_syoudo<pr->lux_min)
          pr->lux_min=pr->ray[i][j].area_syoudo;


      }
    }
  }

  count++;
  QueryPerformanceCounter(&endTime);
  sumTime+= (endTime.QuadPart-startTime.QuadPart);

  if(count%100==0){
    QueryPerformanceFrequency(&ticksPerSec);
    printf("%3.8f\n", ((float)sumTime)
                           /(float)ticksPerSec.QuadPart/100.0);
    sumTime=0;
  }
  return(0);
}

/* calc_plane_points(float *p, float *m, float *n, float *ans)      */
/* get cross point with plane and line which consists of point m and n  */
/* plane  p[0]*x+p[1]*y+p[2]*z+p[3]=0                                   */
/* point1 (m[0],m[1],m[2])                                              */
/* point2 (n[0],n[1],n[2])                                              */
/* answer (ans[0],ans[1],ans[2])                                        */
/* return value 0 means that the line is parallel to the plane          */
int calc_plane_points(float p[4],float m[3],float n[3],float ans[3])
{
  float t, _t;
  ans[0]=ans[1]=ans[2]=0.0;

  _t=p[0]*(n[0]-m[0])+p[1]*(n[1]-m[1])+p[2]*(n[2]-m[2]);
  /* no cross point */
  if(_t==0.0){
    fprintf(stderr, "line is parallel with plane\n");
    return (0);
  }
  t=-(p[0]*m[0]+p[1]*m[1]+p[2]*m[2]+p[3])/_t;
  ans[0]=t*(n[0]-m[0])+m[0];
  ans[1]=t*(n[1]-m[1])+m[1];
  ans[2]=t*(n[2]-m[2])+m[2];
  return(1);
}


void calc_points(float p1[3],float p2[3],float v1[3],float v2[3],
		 float k[3])
{
  float kz1,kz2;

  kz1=((p2[0]-p1[0])+v1[0]/v1[2]*p1[2]-v2[0]/v2[2]*p2[2])/
    (v1[0]/v1[2]-v2[0]/v2[2]);
  kz2=((p2[1]-p1[1])+v1[1]/v1[2]*p1[2]-v2[1]/v2[2]*p2[2])/
    (v1[1]/v1[2]-v2[1]/v2[2]);

  /*    if(fabs(kz1-kz2)>10.0){
	k[0]=k[1]=k[2]=0.0;
	return;
	}else{
	*/	k[2]=(kz1+kz2)/2.0;
	k[0]=v1[0]/v1[2]*(k[2]-p1[2])+p1[0];
	k[1]=v1[1]/v1[2]*(k[2]-p1[2])+p1[1];
	/*    }
	 */
}

void store_mirror_ext(float m[4],float p1[3],float p2[3],
		      float a1[3],float a2[3])
{
  calc_plane_points(m,p1,p2,a1);

  {
    float vec1[3],vec2[3],vec3[3];
    float theta;
    float vec1_[3], vec3_[3];
    vec1_[0]=p2[0]-p1[0];
    vec1_[1]=p2[1]-p1[1];
    vec1_[2]=p2[2]-p1[2];
    norm(vec1_,vec1);
    norm(m, vec2);
    theta=naiseki(vec1, vec2);
    vec3_[0]=vec1[0]-2.0*vec2[0]*theta;
    vec3_[1]=vec1[1]-2.0*vec2[1]*theta;
    vec3_[2]=vec1[2]-2.0*vec2[2]*theta;
    norm(vec3_, vec3);
    a2[0]=vec3[0];
    a2[1]=vec3[1];
    a2[2]=vec3[2];
  }
}
 
/* 2次平面と直線との交点の吟味                                          */
/* m[3]:開始点                                                          */
/* n[3]:開始点からのベクトル                                            */
/* a1[3]:交点1                                                          */
/* a2[3]:交点2                                                          */
/* 帰り値　解が同じ向きに存在しa1[3]が近い場合 1                        */
/*       　解が同じ向きに存在しa2[3]が近い場合 0                        */
/*       　解が違う向きに存在しa1[3]が該当する場合 3                    */
/*       　解が違う向きに存在しa2[3]が該当する場合 2                    */
int check_twovalue(float m[3], float n[3], float a1[3], float a2[3])
{
  float theta1,theta2;
  float vec[3];
  vec[0]=a1[0]-m[0];
  vec[1]=a1[1]-m[1];
  vec[2]=a1[2]-m[2];
  theta1=naiseki(n, vec);
  vec[0]=a2[0]-m[0];
  vec[1]=a2[1]-m[1];
  vec[2]=a2[2]-m[2];
  theta2=naiseki(n, vec);

  if(theta1<0.0 && theta2<0.0)/*2解とも違う向きに存在*/
    return(-1);
  if(theta1>=0.0 && theta2>=0.0){/*2解とも同じ向きに存在*/
    if(DIST(a1[0],a1[1],a1[2],m[0],m[1],m[2])<
       DIST(a2[0],a2[1],a2[2],m[0],m[1],m[2]))
      return(1);/*a1のほうが近い*/
    else
      return(0);/*a2のほうが近い*/
  }

  if(theta1>=0.0 && theta2<0.0)
    return(3);/*a1の向きが正しい*/
  else
    return(2);/*a2の向きが正しい*/
}


/* store_points(float e[10],m[3],n[3],k[3],s[3])                        */
/* get cross point with ellipse and line that consists of point m and   */
/*                                                             vector n */
/* ellipse                                                              */
/*         Ax^2 + By^2 + Cz^2 + Dxy + Eyz + Fzx + Gx + Hy + Iz + J = 0  */
/*         A-J: e[0]-e[9]                                               */
/* point  (m[0],m[1],m[2])                                              */
/* vector (n[0],n[1],n[2])                                              */
/* answer                                                               */
/* cross point   (k[0],k[1],k[2])                                       */
/* normal vector (s[0],s[1],s[2])                                       */
int store_points(float e[10], float m[3], float n[3], float k[3], float s[3])
{
  float tmp1[3],tmp2[3];
  float housen[3];

  /* calculation of across point with mirror and light */
  switch(surface_line(e,m,n,tmp1,tmp2)){
  case 0:                       /* 解なし */
    k[0]=k[1]=k[2]=0.0;
    s[0]=s[1]=s[2]=0.0;
    return(1);
  case 1:                       /* 重解 */
    k[0]=tmp1[0];
    k[1]=tmp1[1];
    k[2]=tmp1[2];
    break;
  case 2:                       /* 解が2つある場合 */
    switch(check_twovalue(m,n,tmp1,tmp2)){
      /*同じ向きにあり近いほうの解を返す*/
    case 1:
    case 3:
      k[0]=tmp1[0];
      k[1]=tmp1[1];
      k[2]=tmp1[2];
      break;
    case 0:
    case 2:
      k[0]=tmp2[0];
      k[1]=tmp2[1];
      k[2]=tmp2[2];
      break;
    case -1:
      return(1);
    }
    break;
  }
  /* 交点における法線ベクトル */ 
  housen[0]=2.0*e[0]*k[0]+
    e[3]*k[1]+
    e[5]*k[2]+
    e[6];
  housen[1]=2.0*e[1]*k[1]+
    e[3]*k[0]+
    e[4]*k[2]+
    e[7];
  housen[2]=2.0*e[2]*k[2]+
    e[4]*k[1]+
    e[5]*k[0]+
    e[8];
  /* スネルの法則に基づき，法線ベクトルに対する入出射計算をする */
  {
    float vec1[3],vec2[3],vec3[3];
    float theta;
    float vec1_[3], vec3_[3];

    vec1_[0]=k[0]-m[0];
    vec1_[1]=k[1]-m[1];
    vec1_[2]=k[2]-m[2];
    norm(vec1_,vec1);
    norm(housen, vec2);
    theta=naiseki(vec1, vec2);
    vec3_[0]=vec1[0]-2.0*vec2[0]*theta;
    vec3_[1]=vec1[1]-2.0*vec2[1]*theta;
    vec3_[2]=vec1[2]-2.0*vec2[2]*theta;
    norm(vec3_, vec3);
    s[0]=vec3[0];
    s[1]=vec3[1];
    s[2]=vec3[2];
  }
  return(0);
}

int store_points_ext(float e[10], float m[3], float n[3], float k[3], float s[3])
{
  float tmp1[3],tmp2[3];
  float housen[3];

  /* calculation of across point with mirror and light */
  switch(surface_line(e,m,n,tmp1,tmp2)){
  case 0:
    k[0]=k[1]=k[2]=0.0;
    s[0]=s[1]=s[2]=0.0;
    return(1);
  case 1:
    k[0]=tmp1[0];
    k[1]=tmp1[1];
    k[2]=tmp1[2];
    break;
  case 2:
    switch(check_twovalue(m,n,tmp1,tmp2)){
      /*同じ向きにあり遠いほうの解を返す*/
      /*スクリーンの凹面を使いたい場合，近くの凸面を無視するため*/
    case 0:
    case 3:
      k[0]=tmp1[0];
      k[1]=tmp1[1];
      k[2]=tmp1[2];
      break;
    case 1:
    case 2:
      k[0]=tmp2[0];
      k[1]=tmp2[1];
      k[2]=tmp2[2];
      break;
    case -1:
      return(1);
    }
    break;
  }
  /* first housen */ 
  housen[0]=2.0*e[0]*k[0]+
    e[3]*k[1]+
    e[5]*k[2]+
    e[6];
  housen[1]=2.0*e[1]*k[1]+
    e[3]*k[0]+
    e[4]*k[2]+
    e[7];
  housen[2]=2.0*e[2]*k[2]+
    e[4]*k[1]+
    e[5]*k[0]+
    e[8];
  {
    float vec1[3],vec2[3],vec3[3];
    float theta;
    float vec1_[3], vec3_[3];

    vec1_[0]=k[0]-m[0];
    vec1_[1]=k[1]-m[1];
    vec1_[2]=k[2]-m[2];
    norm(vec1_,vec1);
    norm(housen, vec2);
    theta=naiseki(vec1, vec2);
    vec3_[0]=vec1[0]-2.0*vec2[0]*theta;
    vec3_[1]=vec1[1]-2.0*vec2[1]*theta;
    vec3_[2]=vec1[2]-2.0*vec2[2]*theta;
    norm(vec3_, vec3);
    s[0]=vec3[0];
    s[1]=vec3[1];
    s[2]=vec3[2];
  }
  return(0);
}

    
/* get_ellipse_ext(float k1_[2],k2_[2],d_,e[6])                        */
/* get ellipse from two focus point and distance between them           */
/* point1 (k1_[0],k1_[1],k1_[2])                                        */
/* point2 (k2_[0],k2_[1],k2_[2])                                        */
/* distance d_                                                          */
/* answer                                                               */
/* ellipse                                                              */
/*        e[0]*x*x + e[1]*y*y + e[2]*x*y + e[3]*x + e[4]*y + e[5] = 0   */
void get_ellipse_ext(float k1_[2], float k2_[2], float d_, float e[6])
{
  e[0]=4.0*(k1_[0]-k2_[0])*(k1_[0]-k2_[0])-4.0*d_*d_;
  e[1]=4.0*(k1_[1]-k2_[1])*(k1_[1]-k2_[1])-4.0*d_*d_;
  e[2]=8.0*(k1_[0]-k2_[0])*(k1_[1]-k2_[1]);
  e[3]=-4.0*(k1_[0]+k2_[0])*(k1_[0]-k2_[0])*(k1_[0]-k2_[0])
    +4.0*d_*d_*(k1_[0]-k2_[0])
    -4.0*(k1_[1]+k2_[1])*(k1_[0]-k2_[0])*(k1_[1]-k2_[1])
    +8.0*d_*d_*k2_[0];
  e[4]=-4.0*(k1_[1]+k2_[1])*(k1_[1]-k2_[1])*(k1_[1]-k2_[1])
    +4.0*d_*d_*(k1_[1]-k2_[1])
    -4.0*(k1_[0]+k2_[0])*(k1_[0]-k2_[0])*(k1_[1]-k2_[1])
    +8.0*d_*d_*k2_[1];
  e[5]=d_*d_*d_*d_
    +(k1_[0]-k2_[0])*(k1_[0]-k2_[0])*(k1_[0]+k2_[0])*(k1_[0]+k2_[0])
    +(k1_[1]-k2_[1])*(k1_[1]-k2_[1])*(k1_[1]+k2_[1])*(k1_[1]+k2_[1])
    +2.0*d_*d_*(-(k1_[0]+k2_[0])*(k1_[0]-k2_[0])-(k1_[1]+k2_[1])*(k1_[1]-k2_[1]))
    +2.0*(k1_[0]-k2_[0])*(k1_[1]-k2_[1])*(k1_[0]+k2_[0])*(k1_[1]+k2_[1])
    -4.0*d_*d_*(k2_[0]*k2_[0]+k2_[1]*k2_[1]);
}


/* Ax^2 + By^2 + Cz^2 + Dxy + Eyz + Fzx + Gx + Hy + Iz + J = 0
   e[0]: A
   e[1]: B
   e[2]: C
   e[3]: D
   e[4]: E
   e[5]: F
   e[6]: G
   e[7]: H
   e[8]: I 
   e[9]: J 
*/
/* 
   line which is on two point as 
   (m[0],m[1],m[2]),(n[0],n[1],n[2])
*/
/*
   0: not cross
   1: one point cross
   2: two point cross 
*/

int surface_line(float *e, float *m, float *n, float *ans1, float *ans2)
{
  float a, b, c;
  float A,B;
  float alpha,beta,ganma;
  int kai;
  float ans[2];
  int niji(float, float, float, float *);

  alpha=n[0];
  beta =n[1];
  ganma=n[2];
    
  if(ganma!=0.0){
    A=alpha/ganma;
    B=beta/ganma;
    a=e[0]*A*A+e[1]*B*B+e[2]+e[3]*A*B+e[4]*B+e[5]*A;
    b=e[0]*(-2.0*A*A*m[2]+2.0*A*m[0])+
      e[1]*(-2.0*B*B*m[2]+2.0*B*m[1])+
      e[3]*(-2.0*A*B*m[2]+B*m[0]+A*m[1])+
      e[4]*(-B*m[2]+m[1])+
      e[5]*(-A*m[2]+m[0])+
      e[6]*A+
      e[7]*B+
      e[8];
    c=e[0]*(A*A*m[2]*m[2]-2.0*A*m[0]*m[2]+m[0]*m[0])+
      e[1]*(B*B*m[2]*m[2]-2.0*B*m[1]*m[2]+m[1]*m[1])+
      e[3]*(A*B*m[2]*m[2]-m[2]*(B*m[0]+A*m[1])+m[0]*m[1])+
      e[6]*(-A*m[2]+m[0])+
      e[7]*(-B*m[2]+m[1])+
      e[9];
    switch(kai=niji(a, b, c, ans)){
    case 2:
      ans2[2]=ans[1];
      ans2[0]=A*(ans[1]-m[2])+m[0];
      ans2[1]=B*(ans[1]-m[2])+m[1];
      ans1[2]=ans[0];
      ans1[0]=A*(ans[0]-m[2])+m[0];
      ans1[1]=B*(ans[0]-m[2])+m[1];
      return(2);
    case 1:
      ans1[2]=ans[0];
      ans1[0]=A*(ans[0]-m[2])+m[0];
      ans1[1]=B*(ans[0]-m[2])+m[1];
      return(1);
    case 0:
      return(0);
    }
  }else{
    if(beta!=0.0){
      A=alpha/beta;
      a=e[0]*A*A+
	e[1]+
	e[3]*A;
      b=e[0]*(-2.0*A*A*m[1]+2.0*A*m[0])+
	e[3]*(-A*m[1]+m[0])+
	e[4]*m[2]+
	e[5]*m[2]*A+
	e[6]*A+
	e[7];
      c=e[0]*(A*A*m[1]*m[1]-2.0*A*m[1]*m[0]+m[0]*m[0])+
	e[2]*m[2]*m[2]+
	e[5]*m[2]*(-A*m[1]+m[0])+
	e[6]*(-A*m[1]+m[0])+
	e[8]*m[2]+
	e[9];
	    
      switch(kai=niji(a, b, c, ans)){
      case 2:
	ans2[2]=m[2];
	ans2[0]=A*(ans[1]-m[1])+m[0];
	ans2[1]=ans[1];
	ans1[2]=m[2];
	ans1[0]=A*(ans[0]-m[1])+m[0];
	ans1[1]=ans[0];
	return(2);
      case 1:
	ans1[2]=m[2];
	ans1[0]=A*(ans[0]-m[1])+m[0];
	ans1[1]=ans[0];
	return(1);
      case 0:
	return(0);
      }
    }else{
      if(alpha==0.0)
	return(3);
      a=e[0];
      b=e[3]*m[1]+
	e[5]*m[2]+
	e[6];
      c=e[1]*m[1]*m[1]+
	e[2]*m[2]*m[2]+
	e[4]*m[1]*m[2]+
	e[7]*m[1]+
	e[8]*m[2]+
	e[9];
      switch(kai=niji(a, b, c, ans)){
      case 2:
	ans2[2]=m[2];
	ans2[0]=ans[1];
	ans2[1]=m[1];
	ans1[2]=m[2];
	ans1[0]=ans[0];
	ans1[1]=m[1];
	return(2);
      case 1:
	ans1[2]=m[2];
	ans1[0]=ans[0];
	ans1[1]=m[1];
	return(1);
      case 0:
	return(0);
      }
    }
  }
  return(3);
}
    

int niji(float a, float b, float c, float *kai)
{
  float d;

  d=b*b-4.0*a*c;
  if(d<0.0)
    return(0);
  else if(d==0.0){
    kai[0]=-b/2.0;
    return(1);
  }else{
    kai[0]=-b/(2.0*a)+sqrt(d)/(2.0*a);
    kai[1]=-b/(2.0*a)-sqrt(d)/(2.0*a);
    return(2);
  }
}

void norm(float *in, float *out)
{
  float normal;
  normal=sqrt(in[0]*in[0]+in[1]*in[1]+in[2]*in[2]);
  out[0]=in[0]/normal;
  out[1]=in[1]/normal;
  out[2]=in[2]/normal;
}

float naiseki(float *a, float *b)
{
  return(a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}

/* polygon_surface(float a[3],b[3],c[3],d[3])                          */
/* point3 (a[3],b[3],c[3],d[3])                                         */
/* return polygon[a,b,d,c] area size                                    */
/*      a  ________  c                                                  */
/*         |     /|                                                     */
/*         |    / |                                                     */
/*         |   /  |                                                     */
/*         |  /   |                                                     */
/*         | /    |                                                     */
/*         |/     |                                                     */
/*      b  ~~~~~~~~  d                                                  */

float polygon_surface(float a[3],float b[3],float c[3],float d[3])
{
  float _ab, _ac, _bc, _bd, _cd; 
  float triangle_surface(float,float,float);
  _ab=DIST(a[0],a[1],a[2],b[0],b[1],b[2]);
  _ac=DIST(a[0],a[1],a[2],c[0],c[1],c[2]);
  _bc=DIST(b[0],b[1],b[2],c[0],c[1],c[2]);
  _bd=DIST(b[0],b[1],b[2],d[0],d[1],d[2]);
  _cd=DIST(c[0],c[1],c[2],d[0],d[1],d[2]);
  return(triangle_surface(_ab,_ac,_bc)+
	 triangle_surface(_bc,_bd,_cd));
}

/* triangle_surface(float a,b,c                                        */
/* point3 (a[3],b[3],c[3],d[3])                                         */
/* return polygon[a,b,d,c] area size                                    */
/*             a                                                        */
/*         _______                                                      */
/*         |     /                                                      */
/*         |    /                                                       */
/*      b  |   /                                                        */
/*         |  / c                                                       */
/*         | /                                                          */
/*         |/                                                           */
float triangle_surface(float a, float b, float c)
{
  float s;
  s=(a+b+c)/2.0;
  return(sqrt(s*(s-a)*(s-b)*(s-c)));
}
