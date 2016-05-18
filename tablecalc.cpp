#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "matrixf.h"
#include "vproj.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "texture.h"
#include <Windows.h>

/* 格子点数 */
#define HDIV 11                 /* 上下11本　10分割 */
#define VDIV 11                 /* 左右11本　10分割 */

TEXTURE lena2;
extern int toggle_real;

int max_vertex[4];
int max_patch[4];
float dsk_pos[4][300][2];
float pix_pos[4][300][2];
int patch[4][500][6];

int mesh_ref[HDIV][VDIV][4][2];//一番近い点を選ぶ
float mesh_pos[HDIV][VDIV][4][3];//４つの視体積の場所（スクリーン上）
float mesh_pos_st[HDIV][VDIV][4][2];//格納されるテクスチャ点を代入する

/* アクティブな領域に関する統計情報 */
int CenterActiveI = -1;   /* アクティブ領域の平均i座標値 */
int CenterActiveJ = -1;   /* アクティブ領域の平均j座標値 */
int NumBetweenMaxMinActiveI = -1;  /* i軸方向におけるアクティブ領域の最大画素数 */
int NumBetweenMaxMinActiveJ = -1;  /* j軸方向におけるアクティブ領域の最大画素数 */

void init_texture(void)
{
  open_tga_texture("lena_std.tga",&lena2,512,512);
}

void display_each_table(int k)
{
  int i;
  glLineWidth(1.0);
  glColor3f(1.0, 1.0, 1.0);
  for(i=0;i<max_vertex[k];i++){
    glBegin(GL_LINE_STRIP);
    if(dsk_pos[k][patch[k][i][1]][0]>0)
      glVertex2fv(dsk_pos[k][0]+2*patch[k][i][1]);
    if(dsk_pos[k][patch[k][i][2]][0]>0)
      glVertex2fv(dsk_pos[k][0]+2*patch[k][i][2]);
    if(dsk_pos[k][patch[k][i][3]][0]>0)
      glVertex2fv(dsk_pos[k][0]+2*patch[k][i][3]);
    if(patch[k][i][0]==4 && dsk_pos[k][patch[k][i][4]][0]>0)
      glVertex2fv(dsk_pos[k][0]+2*patch[k][i][4]);
    if(dsk_pos[k][patch[k][i][1]][0]>0)
      glVertex2fv(dsk_pos[k][0]+2*patch[k][i][1]);
    glEnd();
  }
  
  glColor3f(1.0, 0.0, 0.0);
  for(i=0;i<max_vertex[k];i++){
    if(dsk_pos[k][i][0]<0)continue;
    glRectf(dsk_pos[k][i][0]-5.0,
            dsk_pos[k][i][1]-5.0,
            dsk_pos[k][i][0]+5.0,
            dsk_pos[k][i][1]+5.0);
  }
  
  if(toggle_real){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBindTexture(GL_TEXTURE_2D, lena2.tex_id);
    glEnable(GL_TEXTURE_2D);
    for(i=0;i<max_patch[k];i++){
    glBegin(GL_POLYGON);
    /* (i,j) */
    /*********************************************************
      以下では
    glTexCoord2f(pix_pos[patch[i][1]][0],pix_pos[patch[i][1]][1]);
    glVertex2f(dsk_pos[patch[i][1]][0],dsk_pos[patch[i][1]][1]);
     とおなじことをやっている．
    *********************************************************/
    glTexCoord2fv(pix_pos[k][0]+2*(patch[k][i][1]));
    glVertex2fv  (dsk_pos[k][0]+2*(patch[k][i][1]));

    /* (i,j+1) */
    glTexCoord2fv(pix_pos[k][0]+2*(patch[k][i][2]));
    glVertex2fv  (dsk_pos[k][0]+2*(patch[k][i][2]));

    /* (i,(j+1)*2) or (i+1,j+1) */
    glTexCoord2fv(pix_pos[k][0]+2*(patch[k][i][3]));
    glVertex2fv  (dsk_pos[k][0]+2*(patch[k][i][3]));

    if(patch[k][i][0]==4){
      /* (i,j*2) */
      glTexCoord2fv(pix_pos[k][0]+2*(patch[k][i][4]));
      glVertex2fv  (dsk_pos[k][0]+2*(patch[k][i][4]));
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
 
  }
  
}

void display_table(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 1024.0, 0.0, 768.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  display_each_table(0);
  display_each_table(1);
  display_each_table(2);
  display_each_table(3);

  glFlush();
  glutSwapBuffers();
}

void init_each_table(char *filename, int k)
{
  int i;
  FILE *fp;
  char filename_add[100];

  sprintf(filename_add, "%s.%d.table", filename, k);

  if((fp=fopen(filename_add, "r"))==NULL){
    fprintf(stderr, "Cannot open table data %s\n", filename_add);
    return;
  }
  /* テクスチャ格子点数 */
  fscanf(fp, "%d", max_vertex+k);
  printf("initialize table.dat %d\n ",max_vertex[k]);


  /* 対応テーブル読み込み */
  for(i=0;i< max_vertex[k];i++){
    fscanf(fp, "%f %f %f %f",
           dsk_pos[k][i]  ,/* width->x */
           dsk_pos[k][i]+1,/* height->y */
           pix_pos[k][i]  , /* s */
           pix_pos[k][i]+1);/* t */

  }
  printf("initialize table data completely %d\n", max_vertex[k]);

  /* パッチ数 */
  fscanf(fp, "%d", max_patch+k);
  /* 対応テーブル読み込み */
  for(i=0;i< max_patch[k];i++){
    fscanf(fp, "%d", &(patch[k][i][0]));
    if(patch[k][i][0]==3)
      fscanf(fp, "%d %d %d",
             &(patch[k][i][1]),
             &(patch[k][i][2]),
             &(patch[k][i][3]));
    else
      fscanf(fp, "%d %d %d %d",
             &(patch[k][i][1]),
             &(patch[k][i][2]),
             &(patch[k][i][3]),
             &(patch[k][i][4]));
  }
  printf("initialize patch data completely %d\n ", max_patch[k]);
  fclose(fp);
}

void init_table(char *filename)
{
  init_each_table(filename,0);
  init_each_table(filename,1);
  init_each_table(filename,2);
  init_each_table(filename,3);
}  


void table_calc(int control)// control 1:projection1 2:projection2
{
  void refposition(float (*)[TABLE_NUM_J][4]);
  void refposition2(float (*)[TABLE_NUM_J][4]);  /* added by mizutani */
  void tabledata_save(int );

  LARGE_INTEGER startTime;
  LARGE_INTEGER endTime;
  LARGE_INTEGER ticksPerSec;
  
  int i,j;
  float scr_matrix[16];
  float (*raypos)[TABLE_NUM_J][4];
  T_PROJECTION t_projection;
  int t_calc_screen(T_PROJECTION *); 

  QueryPerformanceCounter(&startTime);
  
  /* 配列をキープ */
  t_projection.ray = (RAY (*)[TABLE_NUM_J+1])malloc(
      sizeof(RAY) * (TABLE_NUM_I+1) * (TABLE_NUM_J+1));

  /* 配列をキープ */
  raypos=(float (*)[TABLE_NUM_J][4])malloc(sizeof(float)*(TABLE_NUM_I)*(TABLE_NUM_J)*4);

  if( raypos == NULL ){
    fprintf(stderr,"Cannot allocate dynamic memory\n");
    return;
  }

  //テーブル計算用の配列にパラメータをコピーする
  for(int i=0;i<6;i++){
    if(control==1){
      t_projection.projector.param[i] = projection1.projector.param[i];
      t_projection.pmirror.param[i]   = projection1.pmirror.param[i];
      t_projection.cmirror.param[i]   = projection1.cmirror.param[i];
      t_projection.screen.param[i]    = projection1.screen.param[i];
    }else if(control==2){
      t_projection.projector.param[i] = projection2.projector.param[i];
      t_projection.pmirror.param[i]   = projection2.pmirror.param[i];
      t_projection.cmirror.param[i]   = projection2.cmirror.param[i];
      t_projection.screen.param[i]    = projection2.screen.param[i];
    }
  }
  if(control==1){
    t_projection.cmirror_radius  = projection1.cmirror_radius;
    t_projection.projector_bottom= projection1.projector_bottom;
    t_projection.projector_top   = projection1.projector_top;
    t_projection.projector_left  = projection1.projector_left;
    t_projection.projector_right = projection1.projector_right;
    t_projection.projector_focus = projection1.projector_focus;
    t_projection.projector_zoomratio= projection1.projector_zoomratio;
  }else if(control==2){
    t_projection.cmirror_radius  = projection2.cmirror_radius;
    t_projection.projector_bottom= projection2.projector_bottom;
    t_projection.projector_top   = projection2.projector_top;
    t_projection.projector_left  = projection2.projector_left;
    t_projection.projector_right = projection2.projector_right;
    t_projection.projector_focus = projection2.projector_focus;
    t_projection.projector_zoomratio= projection2.projector_zoomratio;
  }    
  /* 全画素分の光線追跡計算 */
  t_calc_screen(&t_projection);

  /* プロジェクタ座標系->スクリーン座標系変換行列 */
  multi_mat(t_projection.screen.matrix_inv,
	    t_projection.projector.matrix, 
	    scr_matrix, 4);

  /* 配列の計算結果をコピー */
  for(i=0;i<TABLE_NUM_I;i++)
    for(j=0;j<TABLE_NUM_J;j++){
      multi_vec(scr_matrix, t_projection.ray[i][j].scr.refpos,
		raypos[i][j]);
      raypos[i][j][3]=t_projection.ray[i][j].active;
    }

  /* 配列を開放 */
  free(t_projection.ray);
  /* 配列参照 */
  refposition2(raypos);
  
  /* 補正テーブル作成 */
  tabledata_save(control);
  QueryPerformanceCounter(&endTime);
  QueryPerformanceFrequency(&ticksPerSec);
  printf("%3.3f\n", (((float)endTime.QuadPart - startTime.QuadPart)
                           /(float)ticksPerSec.QuadPart));

  free(raypos);
  if(control==1){
    init_table(projection1.filename);
  }else if(control==2){
    init_table(projection2.filename);
  }
}

void tabledata_save(int control)
{
  FILE *fp_table;
  FILE *fp_obj;
  float orig[3]={0.0, 0.0, 0.0};
  int i,j,k;
  float _tmp[3],__tmp[3];
  char filename_add[100];
  char filename_obj[100];
  float rot_matrix[16];
  int mesh_count=0;
  float trans_view[3]={0.0};

  for(k=0;k<4;k++){
    if(control==1){//projector 1
      sprintf(filename_add, "%s.%d.table", projection1.filename,k);
      sprintf(filename_obj, "%s.%d.obj", projection1.filename,k);
    }else{//projector 2
      sprintf(filename_add, "%s.%d.table", projection2.filename,k);
      sprintf(filename_obj, "%s.%d.obj", projection2.filename,k);
    }
    /* 補正用テーブル作成 */
    if((fp_table=fopen(filename_add,"w"))==NULL){
      fprintf(stderr, "Cannot open %s\n", filename_add);
      exit(1);
    }

    /* OBJ用テーブル作成 */
    if((fp_obj=fopen(filename_obj,"w"))==NULL){
      fprintf(stderr, "Cannot open %s\n", filename_obj);
      exit(1);
    }

    fprintf(fp_table,"%d\n", (VDIV*HDIV)); /* 頂点個数 */
    fprintf(fp_obj , "mtllib none.mtl\n");//OBJ用先頭行

    double view_left=9999999.0;
    double view_right=-9999999.0;
    double view_top=-9999999.0;
    double view_bottom=9999999.0;
    double view_near=-9999999.0;

//#define TEXTURE_MAX_EFFICIENT

    switch(k){
    case 0:
      yrot_mat(rot_matrix, -45.0);
      multi_vec(rot_matrix, view_pos, trans_view);
      break;
    case 1:
      yrot_mat(rot_matrix,  45.0);
      multi_vec(rot_matrix, view_pos, trans_view);
      break;
    case 2:
      yrot_mat(rot_matrix, -45.0);
      xrot_mult_mat(rot_matrix, -90.0);
      multi_vec(rot_matrix, view_pos, trans_view);
      break;
    case 3:
      yrot_mat(rot_matrix,  45.0);
      xrot_mult_mat(rot_matrix, 90.0);
      multi_vec(rot_matrix, view_pos, trans_view);
      break;
    }

    //OBJファイル用パッチデータ書き込み
    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){

#ifdef TEXTURE_MAX_EFFICIENT
        //ここで最大最少をあらかじめ計算しておく
        //球面上に張り付いた視体積のデータを回転
        multi_vec(projection1.screen.matrix, mesh_pos[i][j][k], __tmp);
        /* 結像点を回転し,正面(z軸負方向)にもってくる */
        multi_vec(rot_matrix, __tmp, _tmp);

        if(mesh_ref[i][j][k][0]>0){
          if(view_left>_tmp[0]-trans_view[0])
            view_left=_tmp[0]-trans_view[0];//左（最小）
          if(view_right<_tmp[0]-trans_view[0])
            view_right=_tmp[0]-trans_view[0];//右（最大）
          if(view_bottom>_tmp[1]-trans_view[1])
            view_bottom=_tmp[1]-trans_view[1];//下（最少）
          if(view_top<_tmp[1]-trans_view[1])
            view_top=_tmp[1]-trans_view[1];//上（最大）
          if(view_near<_tmp[2]-trans_view[2])
            view_near=_tmp[2]-trans_view[2];//最も近い位置（zが最大）
        }
#endif

        fprintf(fp_obj,"v %f 0 %f\n",
                /* x value 張りつけ点 (NUM_J), [-1.0:1.0]*/
                (float)mesh_ref[i][j][k][1]/((float)TABLE_NUM_J/2.0)-1.0,
                /* y value 張りつけ点 (NUM_I), [-1.0:1.0] */
                (float)mesh_ref[i][j][k][0]/((float)TABLE_NUM_I/2.0)-1.0);
      }
    }

    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){
        //球面上に張り付いた視体積のデータを回転
        multi_vec(projection1.screen.matrix, mesh_pos[i][j][k], __tmp);
        /* 結像点を回転し,正面(z軸負方向)にもってくる */
        multi_vec(rot_matrix, __tmp, _tmp);
        //テクスチャを効率的に使用する場合

#ifdef TEXTURE_MAX_EFFICIENT
        mesh_pos_st[i][j][k][0]=          ((view_near/(_tmp[2]-trans_view[2])*(_tmp[0]-trans_view[0])-view_left))/
            (view_right - view_left);
        mesh_pos_st[i][j][k][1]=          ((view_near/(_tmp[2]-trans_view[2])*(_tmp[1]-trans_view[1])-view_bottom))/
            (view_top - view_bottom);
#else
        //キューブマッピングなので，viewportの比率を任意に変えることができない
        mesh_pos_st[i][j][k][0]=-0.5/(_tmp[2])*(_tmp[0])+0.5;
        mesh_pos_st[i][j][k][1]=-0.5/(_tmp[2])*(_tmp[1])+0.5;
#endif
        
        fprintf(fp_table,"%f %f %f %f\n",
                /* x value 張りつけ点 (NUM_J), [0:1024]*/
                (float)mesh_ref[i][j][k][1],
                /* y value 張りつけ点 (NUM_I), [0:768] */
                (float)mesh_ref[i][j][k][0],
                /* S value [0.0:1.0] */
                mesh_pos_st[i][j][k][0],
                /* T value */
                mesh_pos_st[i][j][k][1]);

        fprintf(fp_obj,"vt %f %f\n",
                /* S value [0.0:1.0] */
                mesh_pos_st[i][j][k][0],
                /* T value */
                mesh_pos_st[i][j][k][1]);//OBJファイル用書き込み
      }
    }


    /* normal情報書き込み */
    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){
        fprintf(fp_obj,"vn 0 -1 0\n");
      }
    }
    fprintf(fp_obj, "g 0\n");
    fprintf(fp_obj, "usemtl MTL0\n");
    mesh_count=0;

    /* パッチの作成ルーチン */
    for(i=0;i<HDIV-1;i++){
      for(j=0;j<VDIV-1;j++){
        int count=0;
        count =
          ( mesh_ref[i][j][k][0]>0 ? 1:0 )+
            ( mesh_ref[i+1][j][k][0]>0 ? 1:0 )+
              ( mesh_ref[i+1][j+1][k][0]>0 ? 1:0 )+
                ( mesh_ref[i][j+1][k][0]>0 ? 1:0 );
        if(count>=3)
          mesh_count++;
      }
    }
    fprintf(fp_table, "%d\n", mesh_count);

    /* パッチの作成ルーチン */
    for(i=0;i<HDIV-1;i++){
      for(j=0;j<VDIV-1;j++){
        int count=0;
        count = ( mesh_ref[i][j][k][0]>0 ? 1:0 )+
          ( mesh_ref[i+1][j][k][0]>0 ? 1:0 )+
            ( mesh_ref[i+1][j+1][k][0]>0 ? 1:0 )+
              ( mesh_ref[i][j+1][k][0]>0 ? 1:0 );
        if(count==4){
          fprintf(fp_table,"4 %d %d %d %d\n",
                  i    *VDIV+j,  //1
                  i    *VDIV+j+1, //2
                  (i+1)*VDIV+j+1, //3
                  (i+1)*VDIV+j); //4

          //OBJ:四角形だけど三角形で表現
          fprintf(fp_obj,
                  "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                  i    *VDIV+j+1,  //1
                  i    *VDIV+j+1,  //1
                  i    *VDIV+j+1,  //1
                  (i+1)*VDIV+j+1, //4
                  (i+1)*VDIV+j+1, //4
                  (i+1)*VDIV+j+1, //4
                  (i+1)*VDIV+j+1+1, //3
                  (i+1)*VDIV+j+1+1, //3
                  (i+1)*VDIV+j+1+1); //3
          fprintf(fp_obj,
                  "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                  i    *VDIV+j+1,  //1
                  i    *VDIV+j+1,  //1
                  i    *VDIV+j+1,  //1
                  (i+1)*VDIV+j+1+1, //3
                  (i+1)*VDIV+j+1+1, //3
                  (i+1)*VDIV+j+1+1, //3
                  i    *VDIV+j+1+1, //2
                  i    *VDIV+j+1+1, //2
                  i    *VDIV+j+1+1); //2
        }

        if(count==3){
          fprintf(fp_table,"3 ");
          fprintf(fp_obj, "f ");

          if(mesh_ref[i][j][k][0]>0)
            fprintf(fp_table, "%d ", i*VDIV+j); //1
          if(mesh_ref[i][j+1][k][0]>0)
            fprintf(fp_table, "%d ", i*VDIV+j+1);//2
          if(mesh_ref[i+1][j+1][k][0]>0)
            fprintf(fp_table, "%d ", (i+1)*VDIV+j+1);//3
          if(mesh_ref[i+1][j][k][0]>0)
            fprintf(fp_table, "%d ", (i+1)*VDIV+j);//4
          fprintf(fp_table, "\n");

          if(mesh_ref[i][j][k][0]>0)
            fprintf(fp_obj, "%d/%d/%d ",
                    i    *VDIV+j+1,  //1
                    i    *VDIV+j+1,  //1
                    i    *VDIV+j+1);  //1
          if(mesh_ref[i+1][j][k][0]>0)
            fprintf(fp_obj, "%d/%d/%d ",
                    (i+1)*VDIV+j+1, //4
                    (i+1)*VDIV+j+1, //4
                    (i+1)*VDIV+j+1); //4
          if(mesh_ref[i+1][j+1][k][0]>0)
            fprintf(fp_obj, "%d/%d/%d ",
                    (i+1)*VDIV+j+1+1, //3
                    (i+1)*VDIV+j+1+1, //3
                    (i+1)*VDIV+j+1+1); //3
          if(mesh_ref[i][j+1][k][0]>0)
            fprintf(fp_obj, "%d/%d/%d ",
                    i    *VDIV+j+1+1, //2
                    i    *VDIV+j+1+1, //2
                    i    *VDIV+j+1+1); //2
          fprintf(fp_obj, "\n");
        }
      }
    }

    /* 撮像鏡用参照テーブル作成 */
    fclose(fp_table);
    fclose(fp_obj);
  }
}



void refposition(float (*rpos)[TABLE_NUM_J][4])
{
  int i,j,k,l,m;
  float refdist, mindist;
  float tmp[3],front[3],dist;
  float rot_matrix[16];

  /* 緯線経線チェック */
  for(l=0;l<HDIV;l++){
    for(m=0;m<VDIV;m++){
      mindist=999999.99f;
      /* 比較する参照点セット */
      tmp[0]=-0.5+(float)l*(1.0/(HDIV-1));
      tmp[1]=-0.5+(float)m*(1.0/(VDIV-1));
      tmp[2]=-0.5;

      //距離を求めて
      dist=sqrt( (tmp[0]*tmp[0])+(tmp[1]*tmp[1])+(tmp[2]*tmp[2]) );

      //球面上に点を配置する
      front[0]= tmp[0]/dist*screen_radius;
      front[1]= tmp[1]/dist*screen_radius;
      front[2]= tmp[2]/dist*screen_radius;

        //左面用に回転
        yrot_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][0]);

        //右面用に回転
        yrot_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][1]);

        //上面用に回転
        xrot_mat(rot_matrix, 90.0);
        yrot_mult_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][2]);

        //下面用に回転
        xrot_mat(rot_matrix, -90.0);
        yrot_mult_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][3]);

      for(k=0;k<4;k++){
        for(j=0;j<TABLE_NUM_J;j++)
          for(i=0;i<TABLE_NUM_I;i++){
            if(rpos[i][j][3]==0.0)//アクティブでない
              continue;
            refdist=sqrt(
              (rpos[i][j][0]-mesh_pos[l][m][k][0])*
              (rpos[i][j][0]-mesh_pos[l][m][k][0])+
              (rpos[i][j][1]-mesh_pos[l][m][k][1])*
              (rpos[i][j][1]-mesh_pos[l][m][k][1])+
              (rpos[i][j][2]-mesh_pos[l][m][k][2])*
              (rpos[i][j][2]-mesh_pos[l][m][k][2]));
            if(refdist<mindist){
              mindist=refdist;
              mesh_ref[l][m][k][0]=i;
              mesh_ref[l][m][k][1]=j;
            }
          }
        //見つからない
        if(mindist>20.0)
          mesh_ref[l][m][k][0]=mesh_ref[l][m][k][1]=-1;

      }
    }
  }
}

/* 光線追跡本体 */
int t_calc_screen(T_PROJECTION *pr)
{
  int i,j;
  /* プロジェクタ光源原点 */
  float orig[3];
  float mirror[4],ellipse1[10],ellipse2[10];
  float tmp_matrix[16];
  int flag;

  /* activeな(i, j)の平均値を求めるための一時変数 */
  int isum=0, jsum=0;
  int icnt=0, jcnt=0;
  int imin=999999999, imax=-999999999;
  int jmin=999999999, jmax=-999999999;

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
  for(j=0;j<TABLE_NUM_J;j++){
    for(i=0;i<TABLE_NUM_I;i++){
      float begin_vec[3];

      /* フォーカス面設定 */
      /* width */
      pr->ray[i][j].focus_pos[0]=
        ((pr->projector_right- pr->projector_left)*((float)j/((float)TABLE_NUM_J))
         +pr->projector_left)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[1]=
        ((pr->projector_top- pr->projector_bottom)*((float)i/((float)TABLE_NUM_I))
         +pr->projector_bottom)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[2]=-pr->projector_focus;

      /* 有効画素セット */
      pr->ray[i][j].active=1;
      flag=0;
      
      begin_vec[0]=pr->ray[i][j].focus_pos[0]-orig[0];
      begin_vec[1]=pr->ray[i][j].focus_pos[1]-orig[1];
      begin_vec[2]=pr->ray[i][j].focus_pos[2]-orig[2];

      /* 主光線追跡(凸面鏡からスクリーンへ) */
        /* 主光線追跡(プロジェクタから凸面鏡へ) */
        if(store_points(ellipse1,
                        orig,
                        begin_vec,
                        pr->ray[i][j].mir2.refpos,
                        pr->ray[i][j].mir2.refvec)){
          pr->ray[i][j].active=0;
          continue;
        }
          if(store_points_ext(ellipse2,
                              pr->ray[i][j].mir2.refpos,
                              pr->ray[i][j].mir2.refvec,
                              pr->ray[i][j].scr.refpos,
                              pr->ray[i][j].scr.refvec)){
            pr->ray[i][j].active=0;
            continue;
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
            continue;  /* added by mizutani */
        }
      }
      
      isum += i;
      jsum += j;
      icnt++;
      jcnt++;
      if (i < imin){ imin = i; }
      if (i > imax){ imax = i; }
      if (j < jmin){ jmin = j; }
      if (j > jmax){ jmax = j; }
	continue;
    }
  }
  
  CenterActiveI = isum / icnt;
  CenterActiveJ = jsum / jcnt;
  NumBetweenMaxMinActiveI = imax-imin+1; /* i軸におけるアクティブ領域の最大画素数 */
  NumBetweenMaxMinActiveJ = jmax-jmin+1; /* j軸におけるアクティブ領域の最大画素数 */


  return(0);
}

/*********** added by mizutani *******************/

float refdist(float rpos[][TABLE_NUM_J][4], int l, int m, int i, int j, int k){

    if (rpos[i][j][3] == 0){
        /* アクティブでない点ならば，アクティブな領域から離れるほど
           大きい値を返すように適当な値を加える */
        int dj = CenterActiveI - i;
        int di = CenterActiveJ - j;
        return 5000+sqrt((double)(di*di + dj*dj));

    }else{
        return sqrt( (rpos[i][j][0]-mesh_pos[l][m][k][0])*
                       (rpos[i][j][0]-mesh_pos[l][m][k][0])+
                       (rpos[i][j][1]-mesh_pos[l][m][k][1])*
                       (rpos[i][j][1]-mesh_pos[l][m][k][1])+
                       (rpos[i][j][2]-mesh_pos[l][m][k][2])*
                       (rpos[i][j][2]-mesh_pos[l][m][k][2])
               );
    }
}



/***********************************************
* 整数iを要素数nの配列の添字の範囲に丸める
***********************************************/
int int2index(int i, int n){
    if (i < 0){
        return 0;
    }else if (i >= n){
        return n-1;
    }
    return i;
}

/********************************************************************
* (I, J)を中心とする領域から最小値を見つけ，その添字を設定する． 
*
* args:
*   rpos, l, m: refdistの計算に必要なデータ．
*   I, J: 探索の中心点．
*   neighbor: 「近傍」を定義する，中心点からの上下左右の幅
*   imin, jmin: 指定した領域内における最小値の添字（を格納する変数へのポインタ）．
********************************************************************/
void set_mininum_dir(float rpos[][TABLE_NUM_J][4], int l, int m, 
                     int I, int J, int neighbor, int *idir, int *jdir, int k)
{
    float tmp;
    float minr;
    int imin, jmin;

    int i, j;
    int istart, iend;
    int jstart, jend;

    /* iの探索範囲 */
    istart = int2index(I-neighbor, TABLE_NUM_I);
    iend = int2index(I+neighbor, TABLE_NUM_I);

    /* jの探索範囲 */
    jstart = int2index(J-neighbor, TABLE_NUM_J);
    jend = int2index(J+neighbor, TABLE_NUM_J);

    /* (I, J)を中心とする(2neighbor+1)x(2neighbor+1)の領域から最小値を探索する */
    minr = refdist(rpos, l, m, I, J, k);
    imin = I;
    jmin = J;
    for (i=istart; i<=iend; i++){
        for (j=jstart; j<=jend; j++){
            if (rpos[i][j][3]==0){
                /* アクティブでない領域では何もしない */
            }else{
                tmp = refdist(rpos, l, m, i, j, k); 
                if (tmp < minr){
                    minr = tmp;
                    imin = i;
                    jmin = j;
                }
            }
        }
    }

    /* (I, J)からの方向を設定 */
    *idir = imin - I;
    *jdir = jmin - J;
}



/***************************************************************
* 山登り法によるrefdist(i,j)の最小値探索のコア部分．
*
* args:
*   rpos, l, m: refdistの計算に必要なデータ．
*   istart, jstart: 探索の開始点．
*   neighbor,     : 近傍の幅．
*   iscale, jscale: 探索におけるiとjのずらし拡大率．
*                   (i, j)=(1, 1)ならば純粋な山登り法になる．
*   looplimit: 探索回数の上限値．
*   ip, jp: 探索の結果得られたiとj（を格納する変数へのポインタ）．
*
* return:
*   探索した回数．
***************************************************************/
int search_core(float rpos[][TABLE_NUM_J][4], int l, int m, 
                 int istart, int jstart,
				 int neighbor,
                 int iscale, int jscale, 
                 int looplimit,
                 int *ip, int *jp, int kk)
{
    int i, j, k;
    int idir, jdir;
    int itmp, jtmp;

    i = istart;
    j = jstart;

    for (k=0; k<looplimit; k++){

        /* (i, j)を中心とする(2*neighbor+1)x(2*neighbor+1)領域の最小値への方向を取得する */
        set_mininum_dir(rpos, l, m, i, j, neighbor, &idir, &jdir, kk);
        
        /* 最小値発見! */
        if (idir == 0 && jdir == 0){
            break;
        }

        /* 近傍8点の最小値がある方向に向かって(i, j)をずらす */
        itmp = int2index(i+idir*iscale, TABLE_NUM_I);
        jtmp = int2index(j+jdir*jscale, TABLE_NUM_J);


        /* 新しい位置がアクティブでない領域である場合は何もせず， */
        /* 呼び出し元に戻り，一歩戻ってscaleを半分にする          */
        if(rpos[itmp][jtmp][3]==0){
            return 0;
        }else{
            i = itmp;
            j = jtmp;
        }

    }

    *ip = i;
    *jp = j;

    return k;
}

/********************************
* 2つの整数のうち大きい方を返す
********************************/
int check_max(int a, int b){
    return (a > b) ? a : b;
}


/********************************************************************
* 二分探索的な山登り法によるrefdist(i,j)の最小値探索．
*
* Args:
*   rpos, l, m: refdistの計算に必要なデータ．
*   ip, jp: 探索の結果得られたiとj（を格納する変数へのポインタ）．
* Return:
*   得られた最小値．
********************************************************************/
float search_min_refdist(float rpos[][TABLE_NUM_J][4], int l, int m, int *ip, int *jp, int k){
    int i, j;
    int iscale, jscale;   /* iとjのずらし拡大率 */
    int cnt1, cnt2, cnt3; /* 探索回数           */
    int looplimit;        /* 探索回数の上限     */

    /***********************************************************/
    /* (1) まず，2分探索的に探索範囲を絞り込む                 */
    /*     この探索では最小値が見つかるとは限らない．          */
    /***********************************************************/


    /* 最初に探索する(i, j)を探索領域の中心に設定 */
    i = CenterActiveI;
    j = CenterActiveJ;


    /* 最初のずらし幅 */
    iscale = NumBetweenMaxMinActiveI / 4;
    jscale = NumBetweenMaxMinActiveJ / 4;


    /* 最小値がありそうな領域を2分探索的に絞り込む */
    cnt1 = 0;
    while (iscale>=1 || jscale>=1){
        search_core(rpos, l, m,  i, j, 1,  iscale, jscale, 1,  &i, &j, k);
        iscale /= 2;
        jscale /= 2;
        cnt1++;
    }


    /*************************************************/
    /* (2) 次に，少しずつずらしながら最小値を求める  */
    /* （ずらし幅が1＝純粋な山登り法）               */
    /*************************************************/
    looplimit = check_max(NumBetweenMaxMinActiveI, NumBetweenMaxMinActiveJ);
    cnt2 = search_core(rpos, l, m, i, j, 1,  1, 1, looplimit,  &i, &j, k);

    
    /******************************************************/
    /* (3) 最後に，「近傍」の領域を拡大して最小値を求める */
    /*    （近傍領域を拡大した純粋な山登り法）            */
    /******************************************************/
    { 
        int MaxDistance = 3;  /* 極小値間の最大距離の仮定値 */
        looplimit = check_max(NumBetweenMaxMinActiveI, NumBetweenMaxMinActiveJ) / MaxDistance;
        cnt3 = search_core(rpos, l, m, i, j, MaxDistance,  1, 1, looplimit,  &i, &j, k);
    }

    *ip = i;
    *jp = j;

    return refdist(rpos, l, m, i, j, k);
}


/*****************************
* refposition version 2 
******************************/
void refposition2(float (*rpos)[TABLE_NUM_J][4]){
    int l, m;
    int i, j,  k;
    float mindist;
  float dist,tmp[3],front[3],rot_matrix[16];
  int has_active_point;  /* アクティブな領域を含むか否か */

  has_active_point = !(NumBetweenMaxMinActiveI == -1 && NumBetweenMaxMinActiveJ == -1);

    for(l=0; l<HDIV; l++){
      for(m=0; m<VDIV; m++){
        /* 比較する参照点セット */
        tmp[0]=-0.5+(float)l*(1.0/(HDIV-1));
        tmp[1]=-0.5+(float)m*(1.0/(VDIV-1));
        tmp[2]=-0.5;
        //距離を求めて
        dist=sqrt( (tmp[0]*tmp[0])+(tmp[1]*tmp[1])+(tmp[2]*tmp[2]) );

        //球面上に点を配置する
        front[0]= tmp[0]/dist*screen_radius;
        front[1]= tmp[1]/dist*screen_radius;
        front[2]= tmp[2]/dist*screen_radius;

        //左面用に回転
        yrot_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][0]);

        //右面用に回転
        yrot_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][1]);

        //上面用に回転
        xrot_mat(rot_matrix, 90.0);
        yrot_mult_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][2]);

        //下面用に回転
        xrot_mat(rot_matrix, -90.0);
        yrot_mult_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][3]);

        for(k=0;k<4;k++){
          if (has_active_point){
            /* 最小値の探索 */
            mindist = search_min_refdist(rpos, l, m, &i, &j, k);
            
            if(mindist > 20.0){
              mesh_ref[l][m][k][0] = -1;
              mesh_ref[l][m][k][1] = -1;
            }else{
              mesh_ref[l][m][k][0] = i;
              mesh_ref[l][m][k][1] = j;
                }
            
          }else{
            mesh_ref[l][m][k][0] = -1;
            mesh_ref[l][m][k][1] = -1;
          }
        }
      }
    }
}


