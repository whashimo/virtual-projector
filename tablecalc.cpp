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

/* �i�q�_�� */
#define HDIV 11                 /* �㉺11�{�@10���� */
#define VDIV 11                 /* ���E11�{�@10���� */

TEXTURE lena2;
extern int toggle_real;

int max_vertex[4];
int max_patch[4];
float dsk_pos[4][300][2];
float pix_pos[4][300][2];
int patch[4][500][6];

int mesh_ref[HDIV][VDIV][4][2];//��ԋ߂��_��I��
float mesh_pos[HDIV][VDIV][4][3];//�S�̎��̐ς̏ꏊ�i�X�N���[����j
float mesh_pos_st[HDIV][VDIV][4][2];//�i�[�����e�N�X�`���_��������

/* �A�N�e�B�u�ȗ̈�Ɋւ��铝�v��� */
int CenterActiveI = -1;   /* �A�N�e�B�u�̈�̕���i���W�l */
int CenterActiveJ = -1;   /* �A�N�e�B�u�̈�̕���j���W�l */
int NumBetweenMaxMinActiveI = -1;  /* i�������ɂ�����A�N�e�B�u�̈�̍ő��f�� */
int NumBetweenMaxMinActiveJ = -1;  /* j�������ɂ�����A�N�e�B�u�̈�̍ő��f�� */

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
      �ȉ��ł�
    glTexCoord2f(pix_pos[patch[i][1]][0],pix_pos[patch[i][1]][1]);
    glVertex2f(dsk_pos[patch[i][1]][0],dsk_pos[patch[i][1]][1]);
     �Ƃ��Ȃ����Ƃ�����Ă���D
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
  /* �e�N�X�`���i�q�_�� */
  fscanf(fp, "%d", max_vertex+k);
  printf("initialize table.dat %d\n ",max_vertex[k]);


  /* �Ή��e�[�u���ǂݍ��� */
  for(i=0;i< max_vertex[k];i++){
    fscanf(fp, "%f %f %f %f",
           dsk_pos[k][i]  ,/* width->x */
           dsk_pos[k][i]+1,/* height->y */
           pix_pos[k][i]  , /* s */
           pix_pos[k][i]+1);/* t */

  }
  printf("initialize table data completely %d\n", max_vertex[k]);

  /* �p�b�`�� */
  fscanf(fp, "%d", max_patch+k);
  /* �Ή��e�[�u���ǂݍ��� */
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
  
  /* �z����L�[�v */
  t_projection.ray = (RAY (*)[TABLE_NUM_J+1])malloc(
      sizeof(RAY) * (TABLE_NUM_I+1) * (TABLE_NUM_J+1));

  /* �z����L�[�v */
  raypos=(float (*)[TABLE_NUM_J][4])malloc(sizeof(float)*(TABLE_NUM_I)*(TABLE_NUM_J)*4);

  if( raypos == NULL ){
    fprintf(stderr,"Cannot allocate dynamic memory\n");
    return;
  }

  //�e�[�u���v�Z�p�̔z��Ƀp�����[�^���R�s�[����
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
  /* �S��f���̌����ǐՌv�Z */
  t_calc_screen(&t_projection);

  /* �v���W�F�N�^���W�n->�X�N���[�����W�n�ϊ��s�� */
  multi_mat(t_projection.screen.matrix_inv,
	    t_projection.projector.matrix, 
	    scr_matrix, 4);

  /* �z��̌v�Z���ʂ��R�s�[ */
  for(i=0;i<TABLE_NUM_I;i++)
    for(j=0;j<TABLE_NUM_J;j++){
      multi_vec(scr_matrix, t_projection.ray[i][j].scr.refpos,
		raypos[i][j]);
      raypos[i][j][3]=t_projection.ray[i][j].active;
    }

  /* �z����J�� */
  free(t_projection.ray);
  /* �z��Q�� */
  refposition2(raypos);
  
  /* �␳�e�[�u���쐬 */
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
    /* �␳�p�e�[�u���쐬 */
    if((fp_table=fopen(filename_add,"w"))==NULL){
      fprintf(stderr, "Cannot open %s\n", filename_add);
      exit(1);
    }

    /* OBJ�p�e�[�u���쐬 */
    if((fp_obj=fopen(filename_obj,"w"))==NULL){
      fprintf(stderr, "Cannot open %s\n", filename_obj);
      exit(1);
    }

    fprintf(fp_table,"%d\n", (VDIV*HDIV)); /* ���_�� */
    fprintf(fp_obj , "mtllib none.mtl\n");//OBJ�p�擪�s

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

    //OBJ�t�@�C���p�p�b�`�f�[�^��������
    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){

#ifdef TEXTURE_MAX_EFFICIENT
        //�����ōő�ŏ������炩���ߌv�Z���Ă���
        //���ʏ�ɒ���t�������̐ς̃f�[�^����]
        multi_vec(projection1.screen.matrix, mesh_pos[i][j][k], __tmp);
        /* �����_����]��,����(z��������)�ɂ����Ă��� */
        multi_vec(rot_matrix, __tmp, _tmp);

        if(mesh_ref[i][j][k][0]>0){
          if(view_left>_tmp[0]-trans_view[0])
            view_left=_tmp[0]-trans_view[0];//���i�ŏ��j
          if(view_right<_tmp[0]-trans_view[0])
            view_right=_tmp[0]-trans_view[0];//�E�i�ő�j
          if(view_bottom>_tmp[1]-trans_view[1])
            view_bottom=_tmp[1]-trans_view[1];//���i�ŏ��j
          if(view_top<_tmp[1]-trans_view[1])
            view_top=_tmp[1]-trans_view[1];//��i�ő�j
          if(view_near<_tmp[2]-trans_view[2])
            view_near=_tmp[2]-trans_view[2];//�ł��߂��ʒu�iz���ő�j
        }
#endif

        fprintf(fp_obj,"v %f 0 %f\n",
                /* x value ������_ (NUM_J), [-1.0:1.0]*/
                (float)mesh_ref[i][j][k][1]/((float)TABLE_NUM_J/2.0)-1.0,
                /* y value ������_ (NUM_I), [-1.0:1.0] */
                (float)mesh_ref[i][j][k][0]/((float)TABLE_NUM_I/2.0)-1.0);
      }
    }

    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){
        //���ʏ�ɒ���t�������̐ς̃f�[�^����]
        multi_vec(projection1.screen.matrix, mesh_pos[i][j][k], __tmp);
        /* �����_����]��,����(z��������)�ɂ����Ă��� */
        multi_vec(rot_matrix, __tmp, _tmp);
        //�e�N�X�`���������I�Ɏg�p����ꍇ

#ifdef TEXTURE_MAX_EFFICIENT
        mesh_pos_st[i][j][k][0]=          ((view_near/(_tmp[2]-trans_view[2])*(_tmp[0]-trans_view[0])-view_left))/
            (view_right - view_left);
        mesh_pos_st[i][j][k][1]=          ((view_near/(_tmp[2]-trans_view[2])*(_tmp[1]-trans_view[1])-view_bottom))/
            (view_top - view_bottom);
#else
        //�L���[�u�}�b�s���O�Ȃ̂ŁCviewport�̔䗦��C�ӂɕς��邱�Ƃ��ł��Ȃ�
        mesh_pos_st[i][j][k][0]=-0.5/(_tmp[2])*(_tmp[0])+0.5;
        mesh_pos_st[i][j][k][1]=-0.5/(_tmp[2])*(_tmp[1])+0.5;
#endif
        
        fprintf(fp_table,"%f %f %f %f\n",
                /* x value ������_ (NUM_J), [0:1024]*/
                (float)mesh_ref[i][j][k][1],
                /* y value ������_ (NUM_I), [0:768] */
                (float)mesh_ref[i][j][k][0],
                /* S value [0.0:1.0] */
                mesh_pos_st[i][j][k][0],
                /* T value */
                mesh_pos_st[i][j][k][1]);

        fprintf(fp_obj,"vt %f %f\n",
                /* S value [0.0:1.0] */
                mesh_pos_st[i][j][k][0],
                /* T value */
                mesh_pos_st[i][j][k][1]);//OBJ�t�@�C���p��������
      }
    }


    /* normal��񏑂����� */
    for(i=0;i<HDIV;i++){
      for(j=0;j<VDIV;j++){
        fprintf(fp_obj,"vn 0 -1 0\n");
      }
    }
    fprintf(fp_obj, "g 0\n");
    fprintf(fp_obj, "usemtl MTL0\n");
    mesh_count=0;

    /* �p�b�`�̍쐬���[�`�� */
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

    /* �p�b�`�̍쐬���[�`�� */
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

          //OBJ:�l�p�`�����ǎO�p�`�ŕ\��
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

    /* �B�����p�Q�ƃe�[�u���쐬 */
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

  /* �ܐ��o���`�F�b�N */
  for(l=0;l<HDIV;l++){
    for(m=0;m<VDIV;m++){
      mindist=999999.99f;
      /* ��r����Q�Ɠ_�Z�b�g */
      tmp[0]=-0.5+(float)l*(1.0/(HDIV-1));
      tmp[1]=-0.5+(float)m*(1.0/(VDIV-1));
      tmp[2]=-0.5;

      //���������߂�
      dist=sqrt( (tmp[0]*tmp[0])+(tmp[1]*tmp[1])+(tmp[2]*tmp[2]) );

      //���ʏ�ɓ_��z�u����
      front[0]= tmp[0]/dist*screen_radius;
      front[1]= tmp[1]/dist*screen_radius;
      front[2]= tmp[2]/dist*screen_radius;

        //���ʗp�ɉ�]
        yrot_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][0]);

        //�E�ʗp�ɉ�]
        yrot_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][1]);

        //��ʗp�ɉ�]
        xrot_mat(rot_matrix, 90.0);
        yrot_mult_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][2]);

        //���ʗp�ɉ�]
        xrot_mat(rot_matrix, -90.0);
        yrot_mult_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][3]);

      for(k=0;k<4;k++){
        for(j=0;j<TABLE_NUM_J;j++)
          for(i=0;i<TABLE_NUM_I;i++){
            if(rpos[i][j][3]==0.0)//�A�N�e�B�u�łȂ�
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
        //������Ȃ�
        if(mindist>20.0)
          mesh_ref[l][m][k][0]=mesh_ref[l][m][k][1]=-1;

      }
    }
  }
}

/* �����ǐՖ{�� */
int t_calc_screen(T_PROJECTION *pr)
{
  int i,j;
  /* �v���W�F�N�^�������_ */
  float orig[3];
  float mirror[4],ellipse1[10],ellipse2[10];
  float tmp_matrix[16];
  int flag;

  /* active��(i, j)�̕��ϒl�����߂邽�߂̈ꎞ�ϐ� */
  int isum=0, jsum=0;
  int icnt=0, jcnt=0;
  int imin=999999999, imax=-999999999;
  int jmin=999999999, jmax=-999999999;

  /* �X�N���[���̈ʒu�p�����s��Ms�ɕϊ� */
  hand_mat(pr->screen.param, pr->screen.matrix);
  /* �v���W�F�N�^�̈ʒu�p�����s��Mp�ɕϊ� */ 
  hand_mat(pr->projector.param, pr->projector.matrix);
  /* ���ʋ��̈ʒu�p�����s��Mh�ɕϊ� */
  hand_mat(pr->pmirror.param, pr->pmirror.matrix);
  /* �ʖʋ��̈ʒu�p�����s��Mc�ɕϊ� */
  hand_mat(pr->cmirror.param, pr->cmirror.matrix);

  /* �X�N���[���̈ʒu�p���̋t�s��Ms-1�Z�o */
  inv_homoge_mat(pr->screen.matrix, pr->screen.matrix_inv);
  /* �v���W�F�N�^�̈ʒu�p���̋t�s��Mp-1�Z�o */
  inv_homoge_mat(pr->projector.matrix, pr->projector.matrix_inv);
  /* ���ʋ��̈ʒu�p���̋t�s��Mh-1�Z�o */
  inv_homoge_mat(pr->pmirror.matrix, pr->pmirror.matrix_inv);
  /* �ʖʋ��̈ʒu�p���̋t�s��Mc-1�Z�o */
  inv_homoge_mat(pr->cmirror.matrix, pr->cmirror.matrix_inv);

  /* ���ʋ��̎�z=0���v���W�F�N�^���W�n�ɕϊ����� */
  /* Mh-1xMp �̂��������Ƃ���z�̒l */
  multi_mat(pr->pmirror.matrix_inv, pr->projector.matrix, tmp_matrix, 4);
  mirror[0]=tmp_matrix[8];
  mirror[1]=tmp_matrix[9];
  mirror[2]=tmp_matrix[10];
  mirror[3]=tmp_matrix[11];

  /* �v���W�F�N�^���W�n->�X�N���[�����W�n�ϊ��s�� Ms-1xMp */
  multi_mat(pr->screen.matrix_inv, pr->projector.matrix, pr->matrix_p2s, 4);

  /* �ʖʋ��̒��S�ʒu���v���W�F�N�^���W�n�ɕϊ����� */
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

  /* �X�N���[���̒��S�ʒu���v���W�F�N�^���W�n�ɕϊ����� */
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

  /* �v���W�F�N�^���_�̓Y�[���ɂ���đO��Ɉړ����� */
  orig[0]=0.0;
  orig[1]=0.0;
  orig[2]=0.0;
  /*ZOOM_T_BACK+(ZOOM_T_BACK+ZOOM_W_BACK)*(pr->projector_zoomratio-1.0);*/

  pr->all_sum=0.0;
  pr->vi=0;
  for(j=0;j<TABLE_NUM_J;j++){
    for(i=0;i<TABLE_NUM_I;i++){
      float begin_vec[3];

      /* �t�H�[�J�X�ʐݒ� */
      /* width */
      pr->ray[i][j].focus_pos[0]=
        ((pr->projector_right- pr->projector_left)*((float)j/((float)TABLE_NUM_J))
         +pr->projector_left)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[1]=
        ((pr->projector_top- pr->projector_bottom)*((float)i/((float)TABLE_NUM_I))
         +pr->projector_bottom)*pr->projector_zoomratio;
      pr->ray[i][j].focus_pos[2]=-pr->projector_focus;

      /* �L����f�Z�b�g */
      pr->ray[i][j].active=1;
      flag=0;
      
      begin_vec[0]=pr->ray[i][j].focus_pos[0]-orig[0];
      begin_vec[1]=pr->ray[i][j].focus_pos[1]-orig[1];
      begin_vec[2]=pr->ray[i][j].focus_pos[2]-orig[2];

      /* ������ǐ�(�ʖʋ�����X�N���[����) */
        /* ������ǐ�(�v���W�F�N�^����ʖʋ���) */
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


      /* �X�N���[�����`�F�b�N */
      /* �X�N���[���̌`�ɂ������Ă��邩�ǂ������`�F�b�N */
      {
	float _tmp[3];
	float alpha, beta;
        /* �����ǐՂ̌��ʂ��v���W�F�N�^���W�n����X�N���[�����W�n�ɕϊ� */
        multi_vec(pr->matrix_p2s, pr->ray[i][j].scr.refpos, _tmp);
	beta=asin(_tmp[1]/screen_radius);
	alpha=atan2(_tmp[0],-_tmp[2]);

	if(alpha < -(screen_view+1.0)/180.0*M_PI ||
	   alpha > (screen_view+1.0)/180.0*M_PI){
	  pr->ray[i][j].active=0;
	  continue;
	}
	if(beta > screen_up/180.0*M_PI || beta < screen_down/180.0*M_PI){
	  /*�߂��ق��̓_���X�N���[���ɂ������Ă��Ȃ��ꍇ*/
	  /*�����ق��̓_�ɂ��Ē��ׂ�*/
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
  NumBetweenMaxMinActiveI = imax-imin+1; /* i���ɂ�����A�N�e�B�u�̈�̍ő��f�� */
  NumBetweenMaxMinActiveJ = jmax-jmin+1; /* j���ɂ�����A�N�e�B�u�̈�̍ő��f�� */


  return(0);
}

/*********** added by mizutani *******************/

float refdist(float rpos[][TABLE_NUM_J][4], int l, int m, int i, int j, int k){

    if (rpos[i][j][3] == 0){
        /* �A�N�e�B�u�łȂ��_�Ȃ�΁C�A�N�e�B�u�ȗ̈悩�痣���ق�
           �傫���l��Ԃ��悤�ɓK���Ȓl�������� */
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
* ����i��v�f��n�̔z��̓Y���͈̔͂Ɋۂ߂�
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
* (I, J)�𒆐S�Ƃ���̈悩��ŏ��l�������C���̓Y����ݒ肷��D 
*
* args:
*   rpos, l, m: refdist�̌v�Z�ɕK�v�ȃf�[�^�D
*   I, J: �T���̒��S�_�D
*   neighbor: �u�ߖT�v���`����C���S�_����̏㉺���E�̕�
*   imin, jmin: �w�肵���̈���ɂ�����ŏ��l�̓Y���i���i�[����ϐ��ւ̃|�C���^�j�D
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

    /* i�̒T���͈� */
    istart = int2index(I-neighbor, TABLE_NUM_I);
    iend = int2index(I+neighbor, TABLE_NUM_I);

    /* j�̒T���͈� */
    jstart = int2index(J-neighbor, TABLE_NUM_J);
    jend = int2index(J+neighbor, TABLE_NUM_J);

    /* (I, J)�𒆐S�Ƃ���(2neighbor+1)x(2neighbor+1)�̗̈悩��ŏ��l��T������ */
    minr = refdist(rpos, l, m, I, J, k);
    imin = I;
    jmin = J;
    for (i=istart; i<=iend; i++){
        for (j=jstart; j<=jend; j++){
            if (rpos[i][j][3]==0){
                /* �A�N�e�B�u�łȂ��̈�ł͉������Ȃ� */
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

    /* (I, J)����̕�����ݒ� */
    *idir = imin - I;
    *jdir = jmin - J;
}



/***************************************************************
* �R�o��@�ɂ��refdist(i,j)�̍ŏ��l�T���̃R�A�����D
*
* args:
*   rpos, l, m: refdist�̌v�Z�ɕK�v�ȃf�[�^�D
*   istart, jstart: �T���̊J�n�_�D
*   neighbor,     : �ߖT�̕��D
*   iscale, jscale: �T���ɂ�����i��j�̂��炵�g�嗦�D
*                   (i, j)=(1, 1)�Ȃ�Ώ����ȎR�o��@�ɂȂ�D
*   looplimit: �T���񐔂̏���l�D
*   ip, jp: �T���̌��ʓ���ꂽi��j�i���i�[����ϐ��ւ̃|�C���^�j�D
*
* return:
*   �T�������񐔁D
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

        /* (i, j)�𒆐S�Ƃ���(2*neighbor+1)x(2*neighbor+1)�̈�̍ŏ��l�ւ̕������擾���� */
        set_mininum_dir(rpos, l, m, i, j, neighbor, &idir, &jdir, kk);
        
        /* �ŏ��l����! */
        if (idir == 0 && jdir == 0){
            break;
        }

        /* �ߖT8�_�̍ŏ��l����������Ɍ�������(i, j)�����炷 */
        itmp = int2index(i+idir*iscale, TABLE_NUM_I);
        jtmp = int2index(j+jdir*jscale, TABLE_NUM_J);


        /* �V�����ʒu���A�N�e�B�u�łȂ��̈�ł���ꍇ�͉��������C */
        /* �Ăяo�����ɖ߂�C����߂���scale�𔼕��ɂ���          */
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
* 2�̐����̂����傫������Ԃ�
********************************/
int check_max(int a, int b){
    return (a > b) ? a : b;
}


/********************************************************************
* �񕪒T���I�ȎR�o��@�ɂ��refdist(i,j)�̍ŏ��l�T���D
*
* Args:
*   rpos, l, m: refdist�̌v�Z�ɕK�v�ȃf�[�^�D
*   ip, jp: �T���̌��ʓ���ꂽi��j�i���i�[����ϐ��ւ̃|�C���^�j�D
* Return:
*   ����ꂽ�ŏ��l�D
********************************************************************/
float search_min_refdist(float rpos[][TABLE_NUM_J][4], int l, int m, int *ip, int *jp, int k){
    int i, j;
    int iscale, jscale;   /* i��j�̂��炵�g�嗦 */
    int cnt1, cnt2, cnt3; /* �T����           */
    int looplimit;        /* �T���񐔂̏��     */

    /***********************************************************/
    /* (1) �܂��C2���T���I�ɒT���͈͂��i�荞��                 */
    /*     ���̒T���ł͍ŏ��l��������Ƃ͌���Ȃ��D          */
    /***********************************************************/


    /* �ŏ��ɒT������(i, j)��T���̈�̒��S�ɐݒ� */
    i = CenterActiveI;
    j = CenterActiveJ;


    /* �ŏ��̂��炵�� */
    iscale = NumBetweenMaxMinActiveI / 4;
    jscale = NumBetweenMaxMinActiveJ / 4;


    /* �ŏ��l�����肻���ȗ̈��2���T���I�ɍi�荞�� */
    cnt1 = 0;
    while (iscale>=1 || jscale>=1){
        search_core(rpos, l, m,  i, j, 1,  iscale, jscale, 1,  &i, &j, k);
        iscale /= 2;
        jscale /= 2;
        cnt1++;
    }


    /*************************************************/
    /* (2) ���ɁC���������炵�Ȃ���ŏ��l�����߂�  */
    /* �i���炵����1�������ȎR�o��@�j               */
    /*************************************************/
    looplimit = check_max(NumBetweenMaxMinActiveI, NumBetweenMaxMinActiveJ);
    cnt2 = search_core(rpos, l, m, i, j, 1,  1, 1, looplimit,  &i, &j, k);

    
    /******************************************************/
    /* (3) �Ō�ɁC�u�ߖT�v�̗̈���g�債�čŏ��l�����߂� */
    /*    �i�ߖT�̈���g�債�������ȎR�o��@�j            */
    /******************************************************/
    { 
        int MaxDistance = 3;  /* �ɏ��l�Ԃ̍ő勗���̉���l */
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
  int has_active_point;  /* �A�N�e�B�u�ȗ̈���܂ނ��ۂ� */

  has_active_point = !(NumBetweenMaxMinActiveI == -1 && NumBetweenMaxMinActiveJ == -1);

    for(l=0; l<HDIV; l++){
      for(m=0; m<VDIV; m++){
        /* ��r����Q�Ɠ_�Z�b�g */
        tmp[0]=-0.5+(float)l*(1.0/(HDIV-1));
        tmp[1]=-0.5+(float)m*(1.0/(VDIV-1));
        tmp[2]=-0.5;
        //���������߂�
        dist=sqrt( (tmp[0]*tmp[0])+(tmp[1]*tmp[1])+(tmp[2]*tmp[2]) );

        //���ʏ�ɓ_��z�u����
        front[0]= tmp[0]/dist*screen_radius;
        front[1]= tmp[1]/dist*screen_radius;
        front[2]= tmp[2]/dist*screen_radius;

        //���ʗp�ɉ�]
        yrot_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][0]);

        //�E�ʗp�ɉ�]
        yrot_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][1]);

        //��ʗp�ɉ�]
        xrot_mat(rot_matrix, 90.0);
        yrot_mult_mat(rot_matrix, 45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][2]);

        //���ʗp�ɉ�]
        xrot_mat(rot_matrix, -90.0);
        yrot_mult_mat(rot_matrix, -45.0);
        multi_vec(rot_matrix, front, mesh_pos[l][m][3]);

        for(k=0;k<4;k++){
          if (has_active_point){
            /* �ŏ��l�̒T�� */
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


