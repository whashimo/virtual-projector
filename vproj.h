/********************************************************/
/*	Virtual Projector header file (vproj.h)         */
/*	needs matrixf.h          			*/
/*	Original coded by whashimo	        	*/
/*	Version. 2001/01/15     	        	*/
/********************************************************/

#define M_PI 3.14159265238979
#define DEG2RAD (M_PI/180.0)

/*�v���W�F�N�^��p*/
#define BACKYARD  0.0
#define LENS      25.0          /* �����Y���a */
#define M_PI 3.14159265238979

#define ZOOM_T_BACK 0.00

#define DIST(x1,y1,z1,x2,y2,z2) sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2))

#define PROJ_LUMEN 4000
#define NUM_I 64
#define NUM_J 80

#define TABLE_NUM_I 768
#define TABLE_NUM_J 1024

typedef struct{
  float refpos[3];
  float refvec[3];
}REFLECT_POS;

typedef struct{
  unsigned char active;
  float focus_pos[3];
  REFLECT_POS mir1;
  REFLECT_POS mir2;
  REFLECT_POS scr;
  REFLECT_POS mir1_lens[4];
  REFLECT_POS mir2_lens[4];
  REFLECT_POS scr_lens[4];
  float area_focus;             /*�t�H�[�J�X�ʒu*/
  float area_kousoku;           /*�����̗�*/
  float area_screen;            /*�X�N���[����̖ʐ�*/
  float area_syoudo;            /*�Ɠx*/
}RAY;/*���̍\���̂ŕ\�������l�͑S�ăv���W�F�N�^���W�n�ɏ]��*/

typedef struct{
  float matrix[16];             /* ���[���h���烍�[�J���ւ̍��W�ϊ� */
  float matrix_inv[16];         /* ���[�J�����烏�[���h�ւ̍��W�ϊ� */
  float param[6];
}MATPARA;                       /*���W�n�̍\����(�z�u�����肷��)*/

typedef struct{
//  RAY ray[NUM_I+1][NUM_J+1];
  RAY (* ray)[NUM_J+1];
  MATPARA screen;               /*�X�N���[�����W�n�̔z�u*/
  MATPARA projector;            /*�v���W�F�N�^���W�n�̔z�u*/
  MATPARA cmirror;              /*�ʖʋ����W�n�̔z�u*/
  MATPARA pmirror;              /*���ʋ����W�n�̔z�u*/
  float matrix_p2s[16];         /*�v���W�F�N�^���W�n->�X�N���[�����W�n�ϊ��s�� */
                                /*Ms-1xMp*/                     
  float projector_top;          /* �v���W�F�N�^�̎��̐�top */ 
  float projector_bottom;       /* �v���W�F�N�^�̎��̐�bottom */ 
  float projector_right;        /* �v���W�F�N�^�̎��̐�right */ 
  float projector_left;         /* �v���W�F�N�^�̎��̐�left */ 
  float projector_focus;        /* �v���W�F�N�^�̏œ_���� */
  float projector_zoomratio;    /*�Y�[����*/
  float cmirror_radius;         /*�ʖʋ��ȗ����a*/ 
  char filename[100];           /*�p�����[�^�t�@�C����*/
  char proj_filename[100];      /*�v���W�F�N�^��p�p�����[�^�t�@�C����*/
  float all_sum;                /*�����̑傫��*/
  int vi;                       /*�L����f��*/
  float lux_min;                /*�Ɠx�ŏ��l*/
  float lux_max;                /*�Ɠx�ő�l*/
  float pix_min;                /*��f�Ԃ̋����ŏ��l*/
  float pix_max;                /*��f�Ԃ̋����ő�l*/
  float view_up;              /*���_�����̍ő�p*/
  float view_low;             /*���_�����̍ő��p*/
  float view_right;           /*���_�����̍ő吅���p�E*/
  float view_left;            /*���_�����̍ő吅���p��*/
}PROJECTION;                    /*���e�n�\����*/  

typedef struct{
  RAY (* ray)[TABLE_NUM_J+1];
  MATPARA screen;               /*�X�N���[�����W�n�̔z�u*/
  MATPARA projector;            /*�v���W�F�N�^���W�n�̔z�u*/
  MATPARA cmirror;              /*�ʖʋ����W�n�̔z�u*/
  MATPARA pmirror;              /*���ʋ����W�n�̔z�u*/
  float matrix_p2s[16];         /*�v���W�F�N�^���W�n->�X�N���[�����W�n�ϊ��s�� */
                                /*Ms-1xMp*/                     
  float projector_top;          /* �v���W�F�N�^�̎��̐� */ 
  float projector_bottom;       /* �v���W�F�N�^�̎��̐� */ 
  float projector_right;        /* �v���W�F�N�^�̎��̐� */ 
  float projector_left;         /* �v���W�F�N�^�̎��̐� */ 
  float projector_focus;        /* �v���W�F�N�^�̏œ_���� */
  float projector_zoomratio;    /*�Y�[����*/
  float cmirror_radius;         /*�ʖʋ��ȗ����a*/ 
  char filename[100];           /*�p�����[�^�t�@�C����*/
  char proj_filename[100];      /*�v���W�F�N�^�p�����[�^�t�@�C����*/
  float all_sum;                /*�����̑傫��*/
  int vi;                       /*�L����f��*/
}T_PROJECTION;                    /*���e�n�\����*/  


#ifdef VPROJ_C /*���d��`��h������,vproj.c�ł͒�`�C����ȊO�ł�extern*/
float lens[4][3]={{+LENS, 0.0, BACKYARD}, /* �����Y�̓����_ */
		  {-LENS, 0.0, BACKYARD},
		  {0.0, -LENS, BACKYARD},
		  {0.0, +LENS, BACKYARD}};
PROJECTION projection1;         /* ��꓊�e�n */
PROJECTION projection2;         /* ��񓊉e�n */
PROJECTION t_projection;         /* �␳�e�[�u���쐬�p */
float screen_radius=900.0;     /*�X�N���[�����a*/
float screen_view=90.0;        /*�X�N���[����������p*/
float screen_up=60.0;           /*�X�N���[���p*/
float screen_down=-60.0;        /*�X�N���[����p*/
float view_pos[3]={0.0, 0.0, -100.0};/*�␳�e�[�u���쐬�p���_��ʒu*/
#else
extern PROJECTION projection1;
extern PROJECTION projection2;
extern float lens[4][3];
extern float screen_radius;
extern float screen_view;
extern float screen_up;
extern float screen_down;
extern float view_pos[3];       // �␳�e�[�u���p�̎��_�ʒu
#endif

float naiseki(float *, float *);
void norm(float *, float *);
void get_ellipse_rot(float *);
void get_ellipse_ext(float *, float *, float, float *);
int surface_line(float *, float *, float *, float *, float *);
int store_points(float *, float *, float *, float *, float *);
int store_points_ext(float *, float *, float *, float *, float *);
void store_mirror_ext(float *, float *, float *, float *, float *);
void calc_points(float *, float *, float *, float *, float *);
int calc_plane_points(float *, float *, float *, float *);
float polygon_surface(float *, float *, float *, float *);

void focus_initialize(PROJECTION *);
int calc_screen(PROJECTION *);
void projector_save(PROJECTION);
void projector_load(PROJECTION *);
void projection_save(PROJECTION);
void projection_load(PROJECTION *);
void projection_default(PROJECTION *);
void projection_calc_save(PROJECTION);
