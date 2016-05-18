/********************************************************/
/*	Virtual Projector header file (vproj.h)         */
/*	needs matrixf.h          			*/
/*	Original coded by whashimo	        	*/
/*	Version. 2001/01/15     	        	*/
/********************************************************/

#define M_PI 3.14159265238979
#define DEG2RAD (M_PI/180.0)

/*プロジェクタ画角*/
#define BACKYARD  0.0
#define LENS      25.0          /* レンズ半径 */
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
  float area_focus;             /*フォーカス位置*/
  float area_kousoku;           /*光束の量*/
  float area_screen;            /*スクリーン上の面積*/
  float area_syoudo;            /*照度*/
}RAY;/*この構造体で表現される値は全てプロジェクタ座標系に従う*/

typedef struct{
  float matrix[16];             /* ワールドからローカルへの座標変換 */
  float matrix_inv[16];         /* ローカルからワールドへの座標変換 */
  float param[6];
}MATPARA;                       /*座標系の構造体(配置を決定する)*/

typedef struct{
//  RAY ray[NUM_I+1][NUM_J+1];
  RAY (* ray)[NUM_J+1];
  MATPARA screen;               /*スクリーン座標系の配置*/
  MATPARA projector;            /*プロジェクタ座標系の配置*/
  MATPARA cmirror;              /*凸面鏡座標系の配置*/
  MATPARA pmirror;              /*平面鏡座標系の配置*/
  float matrix_p2s[16];         /*プロジェクタ座標系->スクリーン座標系変換行列 */
                                /*Ms-1xMp*/                     
  float projector_top;          /* プロジェクタの視体積top */ 
  float projector_bottom;       /* プロジェクタの視体積bottom */ 
  float projector_right;        /* プロジェクタの視体積right */ 
  float projector_left;         /* プロジェクタの視体積left */ 
  float projector_focus;        /* プロジェクタの焦点距離 */
  float projector_zoomratio;    /*ズーム率*/
  float cmirror_radius;         /*凸面鏡曲率半径*/ 
  char filename[100];           /*パラメータファイル名*/
  char proj_filename[100];      /*プロジェクタ画角パラメータファイル名*/
  float all_sum;                /*結像の大きさ*/
  int vi;                       /*有効画素数*/
  float lux_min;                /*照度最小値*/
  float lux_max;                /*照度最大値*/
  float pix_min;                /*画素間の距離最小値*/
  float pix_max;                /*画素間の距離最大値*/
  float view_up;              /*視点基準からの最大仰角*/
  float view_low;             /*視点基準からの最大俯角*/
  float view_right;           /*視点基準からの最大水平角右*/
  float view_left;            /*視点基準からの最大水平角左*/
}PROJECTION;                    /*投影系構造体*/  

typedef struct{
  RAY (* ray)[TABLE_NUM_J+1];
  MATPARA screen;               /*スクリーン座標系の配置*/
  MATPARA projector;            /*プロジェクタ座標系の配置*/
  MATPARA cmirror;              /*凸面鏡座標系の配置*/
  MATPARA pmirror;              /*平面鏡座標系の配置*/
  float matrix_p2s[16];         /*プロジェクタ座標系->スクリーン座標系変換行列 */
                                /*Ms-1xMp*/                     
  float projector_top;          /* プロジェクタの視体積 */ 
  float projector_bottom;       /* プロジェクタの視体積 */ 
  float projector_right;        /* プロジェクタの視体積 */ 
  float projector_left;         /* プロジェクタの視体積 */ 
  float projector_focus;        /* プロジェクタの焦点距離 */
  float projector_zoomratio;    /*ズーム率*/
  float cmirror_radius;         /*凸面鏡曲率半径*/ 
  char filename[100];           /*パラメータファイル名*/
  char proj_filename[100];      /*プロジェクタパラメータファイル名*/
  float all_sum;                /*結像の大きさ*/
  int vi;                       /*有効画素数*/
}T_PROJECTION;                    /*投影系構造体*/  


#ifdef VPROJ_C /*多重定義を防ぐため,vproj.cでは定義，それ以外ではextern*/
float lens[4][3]={{+LENS, 0.0, BACKYARD}, /* レンズの特徴点 */
		  {-LENS, 0.0, BACKYARD},
		  {0.0, -LENS, BACKYARD},
		  {0.0, +LENS, BACKYARD}};
PROJECTION projection1;         /* 第一投影系 */
PROJECTION projection2;         /* 第二投影系 */
PROJECTION t_projection;         /* 補正テーブル作成用 */
float screen_radius=900.0;     /*スクリーン半径*/
float screen_view=90.0;        /*スクリーン水平視野角*/
float screen_up=60.0;           /*スクリーン仰角*/
float screen_down=-60.0;        /*スクリーン俯角*/
float view_pos[3]={0.0, 0.0, -100.0};/*補正テーブル作成用視点基準位置*/
#else
extern PROJECTION projection1;
extern PROJECTION projection2;
extern float lens[4][3];
extern float screen_radius;
extern float screen_view;
extern float screen_up;
extern float screen_down;
extern float view_pos[3];       // 補正テーブル用の視点位置
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
