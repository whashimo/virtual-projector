#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glui.h"
#include "vproj.h"

extern GLUI *glui;
GLUI *glui_proj1,*glui_proj2;

extern int toggle_lens;
extern int toggle_focus;
extern int toggle_dispose;
extern int toggle_view;
extern int toggle_real;
extern int toggle_calc;
extern int toggle_four;
extern int toggle_projection1;
extern int toggle_projection2;
extern int toggle_cmirror;
extern int toggle_pmirror;

void exit_prog(int);
void RedisplayBoth(void);
void copy_mirror_data(int);

extern int StatusWindow;

void control_cb( int control )
{
  //プロジェクタ１の更新　１
  //プロジェクタ２の更新　２
  //フォーカス面の初期化　４
  //プロジェクタの表示切替８
  //プロジェクタ１セーブ　１６
  //プロジェクタ２セーブ　３２
  //プロジェクタ１ロード　６４
  //プロジェクタ１ロード　１２８

  if(control&0x40){
    projector_load(&projection1);
    projection_load(&projection1);
    focus_initialize(&projection1);
    calc_screen(&projection1);
  }

  if(control&0x80){
    projector_load(&projection2);
    projection_load(&projection2);
    focus_initialize(&projection2);
    calc_screen(&projection2);
  }

  //プロジェクタパラメータのコピー
  if(control&0x07){
    projection2.projector_bottom   =projection1.projector_bottom;
    projection2.projector_top      =projection1.projector_top;
    projection2.projector_left     =projection1.projector_left;
    projection2.projector_right    =projection1.projector_right;
    projection2.projector_focus    =projection1.projector_focus;
    projection2.projector_zoomratio=projection1.projector_zoomratio;
  }

  // フォーカス面が初期化された場合
  if(control&0x04){
    focus_initialize(&projection1);
    focus_initialize(&projection2);
  }

  if(control&0x03){
    projection2.screen.param[0]  =projection1.screen.param[0];
    projection2.screen.param[1]  =projection1.screen.param[1];
    projection2.screen.param[2]  =projection1.screen.param[2];
    projection2.screen.param[3]  =projection1.screen.param[3];
    projection2.screen.param[4]  =projection1.screen.param[4];
    projection2.screen.param[5]  =projection1.screen.param[5];
  }
  //プロジェクタ１のパラメータが変更された場合
  if(control&0x01)
    calc_screen(&projection1);

  //プロジェクタ２のパラメータが変更された場合
  if(control&0x02)
    calc_screen(&projection2);

  if(control&0x08){
    if(toggle_projection1)
      glui_proj1->show();
    else
      glui_proj1->hide();

    if(toggle_projection2)
      glui_proj2->show();
    else
      glui_proj2->hide();
  }
  if(control&0x10){
    projector_save(projection1);
    projection_save(projection1);
  }
  if(control&0x20){
    projector_save(projection2);
    projection_save(projection2);
  }

  if(toggle_calc){
    void table_calc(int);
    if(control&0x01)
      table_calc(0x01);
    //if(control&0x02)
    //  table_calc(0x02);
  }

  glui->sync_live();
  glui_proj1->sync_live();
  glui_proj2->sync_live();
}

void glui_projector1(void)
{
  // projector1 parameter
  glui_proj1 = GLUI_Master.create_glui( "Projector1" );

  // projector1 position
  GLUI_Panel *panel_proj_pos = glui_proj1->add_rollout( "Projector Position", true );
  GLUI_Spinner *spinner_pro1x = glui_proj1->add_spinner_to_panel(panel_proj_pos, "X", GLUI_SPINNER_FLOAT, &projection1.projector.param[0], 1, control_cb);
  spinner_pro1x->set_speed(10.0);
  GLUI_Spinner *spinner_pro1y = glui_proj1->add_spinner_to_panel(panel_proj_pos, "Y", GLUI_SPINNER_FLOAT, &projection1.projector.param[1], 1, control_cb);
  spinner_pro1y->set_speed(10.0);
  GLUI_Spinner *spinner_pro1z = glui_proj1->add_spinner_to_panel(panel_proj_pos, "Z", GLUI_SPINNER_FLOAT, &projection1.projector.param[2], 1, control_cb);
  spinner_pro1z->set_speed(10.0);

  glui_proj1->add_column_to_panel(panel_proj_pos, false );

  GLUI_Translation *trans_pro1x =
    glui_proj1->add_translation_to_panel(panel_proj_pos, "X", GLUI_TRANSLATION_X, &projection1.projector.param[0], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_proj_pos, false );
  GLUI_Translation *trans_pro1y =
    glui_proj1->add_translation_to_panel(panel_proj_pos, "Y", GLUI_TRANSLATION_Y, &projection1.projector.param[1], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_proj_pos, false );
  GLUI_Translation *trans_pro1z =
    glui_proj1->add_translation_to_panel(panel_proj_pos, "Z", GLUI_TRANSLATION_Z, &projection1.projector.param[2], 1, control_cb);

  GLUI_Panel *panel_proj_ori = glui_proj1->add_rollout( "Projector Orientation", true );
  GLUI_Spinner *spinner_pro1rx = glui_proj1->add_spinner_to_panel(panel_proj_ori, "rX", GLUI_SPINNER_FLOAT, &projection1.projector.param[3], 1, control_cb);
  spinner_pro1rx->set_speed(1.0);
  GLUI_Spinner *spinner_pro1ry = glui_proj1->add_spinner_to_panel(panel_proj_ori, "rY", GLUI_SPINNER_FLOAT, &projection1.projector.param[4], 1, control_cb);
  spinner_pro1ry->set_speed(1.0);
  GLUI_Spinner *spinner_pro1rz = glui_proj1->add_spinner_to_panel(panel_proj_ori, "rZ", GLUI_SPINNER_FLOAT, &projection1.projector.param[5], 1, control_cb);
  spinner_pro1rz->set_speed(1.0);

  glui_proj1->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1rx = glui_proj1->add_translation_to_panel(panel_proj_ori, "rX", GLUI_TRANSLATION_X, &projection1.projector.param[3], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1ry = glui_proj1->add_translation_to_panel(panel_proj_ori, "rY", GLUI_TRANSLATION_Y, &projection1.projector.param[4], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1rz = glui_proj1->add_translation_to_panel(panel_proj_ori, "rZ", GLUI_TRANSLATION_Z, &projection1.projector.param[5], 1, control_cb);

  ////////////////////////////////////////////////////////////////

  GLUI_Panel *panel_pmir_pos = glui_proj1->add_rollout( "Plain Mirror Position", false );
  GLUI_Spinner *spinner_pmirx = glui_proj1->add_spinner_to_panel(panel_pmir_pos, "X", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[0], 1, control_cb);
  spinner_pmirx->set_speed(10.0);
  GLUI_Spinner *spinner_pmiry = glui_proj1->add_spinner_to_panel(panel_pmir_pos, "Y", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[1], 1, control_cb);
  spinner_pmiry->set_speed(10.0);
  GLUI_Spinner *spinner_pmirz = glui_proj1->add_spinner_to_panel(panel_pmir_pos, "Z", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[2], 1, control_cb);
  spinner_pmirz->set_speed(10.0);

  glui_proj1->add_column_to_panel(panel_pmir_pos, false );

  GLUI_Translation *trans_pmirx =
    glui_proj1->add_translation_to_panel(panel_pmir_pos, "X", GLUI_TRANSLATION_X, &projection1.pmirror.param[0], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_pmir_pos, false );
  GLUI_Translation *trans_pmiry =
    glui_proj1->add_translation_to_panel(panel_pmir_pos, "Y", GLUI_TRANSLATION_Y, &projection1.pmirror.param[1], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_pmir_pos, false );
  GLUI_Translation *trans_pmirz =
    glui_proj1->add_translation_to_panel(panel_pmir_pos, "Z", GLUI_TRANSLATION_Z, &projection1.pmirror.param[2], 1, control_cb);

  GLUI_Panel *panel_pmir_ori = glui_proj1->add_rollout( "Plain Mirror Orientation", false );
  GLUI_Spinner *spinner_pmirrx = glui_proj1->add_spinner_to_panel(panel_pmir_ori, "rX", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[3], 1, control_cb);
  spinner_pmirrx->set_speed(1.0);
  GLUI_Spinner *spinner_pmirry = glui_proj1->add_spinner_to_panel(panel_pmir_ori, "rY", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[4], 1, control_cb);
  spinner_pmirry->set_speed(1.0);
  GLUI_Spinner *spinner_pmirrz = glui_proj1->add_spinner_to_panel(panel_pmir_ori, "rZ", GLUI_SPINNER_FLOAT, &projection1.pmirror.param[5], 1, control_cb);
  spinner_pmirrz->set_speed(1.0);

  glui_proj1->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirrx = glui_proj1->add_translation_to_panel(panel_pmir_ori, "rX", GLUI_TRANSLATION_X, &projection1.pmirror.param[3], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirry = glui_proj1->add_translation_to_panel(panel_pmir_ori, "rY", GLUI_TRANSLATION_Y, &projection1.pmirror.param[4], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirrz = glui_proj1->add_translation_to_panel(panel_pmir_ori, "rZ", GLUI_TRANSLATION_Z, &projection1.pmirror.param[5], 1, control_cb);

  ////////////////////////////////////////////////////////////////

  GLUI_Panel *panel_cmir_pos = glui_proj1->add_rollout( "Convex Mirror Position", false );

  GLUI_Spinner *spinner_cmir_radius = glui_proj1->add_spinner_to_panel(panel_cmir_pos, "Radius", GLUI_SPINNER_FLOAT, &projection1.cmirror_radius, 1, control_cb);
  spinner_cmir_radius->set_float_limits(0.0, 500.0);

  GLUI_Spinner *spinner_cmirx = glui_proj1->add_spinner_to_panel(panel_cmir_pos, "X", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[0], 1, control_cb);
  spinner_cmirx->set_speed(10.0);
  GLUI_Spinner *spinner_cmiry = glui_proj1->add_spinner_to_panel(panel_cmir_pos, "Y", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[1], 1, control_cb);
  spinner_cmiry->set_speed(10.0);
  GLUI_Spinner *spinner_cmirz = glui_proj1->add_spinner_to_panel(panel_cmir_pos, "Z", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[2], 1, control_cb);
  spinner_cmirz->set_speed(10.0);

  glui_proj1->add_column_to_panel(panel_cmir_pos, false );
  GLUI_Translation *trans_cmirx =
    glui_proj1->add_translation_to_panel(panel_cmir_pos, "X", GLUI_TRANSLATION_X, &projection1.cmirror.param[0], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_cmir_pos, false );
  GLUI_Translation *trans_cmiry =
    glui_proj1->add_translation_to_panel(panel_cmir_pos, "Y", GLUI_TRANSLATION_Y, &projection1.cmirror.param[1], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_cmir_pos, false );
  GLUI_Translation *trans_cmirz =
    glui_proj1->add_translation_to_panel(panel_cmir_pos, "Z", GLUI_TRANSLATION_Z, &projection1.cmirror.param[2], 1, control_cb);

  GLUI_Panel *panel_cmir_ori = glui_proj1->add_rollout( "Convex Mirror Orientation", false );
  GLUI_Spinner *spinner_cmirrx = glui_proj1->add_spinner_to_panel(panel_cmir_ori, "rX", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[3], 1, control_cb);
  spinner_cmirrx->set_speed(1.0);
  GLUI_Spinner *spinner_cmirry = glui_proj1->add_spinner_to_panel(panel_cmir_ori, "rY", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[4], 1, control_cb);
  spinner_cmirry->set_speed(1.0);
  GLUI_Spinner *spinner_cmirrz = glui_proj1->add_spinner_to_panel(panel_cmir_ori, "rZ", GLUI_SPINNER_FLOAT, &projection1.cmirror.param[5], 1, control_cb);
  spinner_cmirrz->set_speed(1.0);

  glui_proj1->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirrx = glui_proj1->add_translation_to_panel(panel_cmir_ori, "rX", GLUI_TRANSLATION_X, &projection1.cmirror.param[3], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirry = glui_proj1->add_translation_to_panel(panel_cmir_ori, "rY", GLUI_TRANSLATION_Y, &projection1.cmirror.param[4], 1, control_cb);
  glui_proj1->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirrz = glui_proj1->add_translation_to_panel(panel_cmir_ori, "rZ", GLUI_TRANSLATION_Z, &projection1.cmirror.param[5], 1, control_cb);

  
  ////////////////////////////////////////////////////////////////

  GLUI_Panel *panel_proj_para = glui_proj1->add_rollout( "Projection Parameter", true );

  GLUI_EditText *vi_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Active Pix", GLUI_EDITTEXT_INT, &projection1.vi);
  vi_edittext->disable();

  static float ratio_vi=(float)projection1.vi/((NUM_I+1)*(NUM_J+1))*100.0;
  GLUI_EditText *ratio_vi_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Ratio[%]", GLUI_EDITTEXT_FLOAT, &ratio_vi);
  ratio_vi_edittext->disable();

  GLUI_EditText *lux_min_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Lux min[lx]", GLUI_EDITTEXT_FLOAT, &projection1.lux_min);
  lux_min_edittext->disable();

  GLUI_EditText *pix_min_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Pix min[mm]", GLUI_EDITTEXT_FLOAT, &projection1.pix_min);
  pix_min_edittext->disable();
  
  GLUI_EditText *view_dn_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "View low[deg]", GLUI_EDITTEXT_FLOAT, &projection1.view_low);
  view_dn_edittext->disable();

  GLUI_EditText *view_lt_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "View left[deg]", GLUI_EDITTEXT_FLOAT, &projection1.view_left);
  view_lt_edittext->disable();

  glui_proj1->add_column_to_panel(panel_proj_para, false );
  
  static int total_vi=(NUM_I+1)*(NUM_J+1);
  GLUI_EditText *total_vi_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Total Pix", GLUI_EDITTEXT_INT, &total_vi);
  total_vi_edittext->disable();

  static float pix_diag=(float)projection1.all_sum/projection1.vi;
  GLUI_EditText *pix_diag_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Ave.Diag[pix]", GLUI_EDITTEXT_FLOAT, &pix_diag);
  pix_diag_edittext->disable();

  GLUI_EditText *lux_max_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Lux max[lx]", GLUI_EDITTEXT_FLOAT, &projection1.lux_max);
  lux_max_edittext->disable();

  GLUI_EditText *pix_max_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "Pix max[mm]", GLUI_EDITTEXT_FLOAT, &projection1.pix_max);
  pix_max_edittext->disable();

  GLUI_EditText *view_up_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "View up[deg]", GLUI_EDITTEXT_FLOAT, &projection1.view_up);
  view_up_edittext->disable();

  GLUI_EditText *view_rg_edittext =
    glui_proj1->add_edittext_to_panel(panel_proj_para, "View right[deg]", GLUI_EDITTEXT_FLOAT, &projection1.view_right);
  view_rg_edittext->disable();

  void table_calc(int);

  GLUI_Panel *panel_proj1 = glui_proj1->add_rollout( "Data Handling", false );
  glui_proj1->add_edittext_to_panel(panel_proj1, "Status filename", GLUI_EDITTEXT_TEXT, projection1.filename);
  glui_proj1->add_button_to_panel(panel_proj1, "Save", 16, control_cb);
  glui_proj1->add_button_to_panel(panel_proj1, "Load", 64, control_cb);
  glui_proj1->add_button_to_panel(panel_proj1, "Calc", 1, table_calc );
  glui_proj1->add_edittext_to_panel(panel_proj1, "Projector filename", GLUI_EDITTEXT_TEXT, projection1.proj_filename);
}

void glui_projector2(void)
{
  glui_proj2 = GLUI_Master.create_glui( "Projector2" );

  // projector parameter
  GLUI_Panel *panel_proj_pos = glui_proj2->add_rollout( "Projector Position", true );
  GLUI_Spinner *spinner_pro1x = glui_proj2->add_spinner_to_panel(panel_proj_pos, "X", GLUI_SPINNER_FLOAT, &projection2.projector.param[0], 2, control_cb);
  spinner_pro1x->set_speed(10.0);  
  GLUI_Spinner *spinner_pro1y = glui_proj2->add_spinner_to_panel(panel_proj_pos, "Y", GLUI_SPINNER_FLOAT, &projection2.projector.param[1], 2, control_cb);
  spinner_pro1y->set_speed(10.0);  
  GLUI_Spinner *spinner_pro1z = glui_proj2->add_spinner_to_panel(panel_proj_pos, "Z", GLUI_SPINNER_FLOAT, &projection2.projector.param[2], 2, control_cb);
  spinner_pro1z->set_speed(10.0);  
  
  glui_proj2->add_column_to_panel(panel_proj_pos, false );

  GLUI_Translation *trans_pro1x = 
    glui_proj2->add_translation_to_panel(panel_proj_pos, "X", GLUI_TRANSLATION_X, &projection2.projector.param[0], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_proj_pos, false );
  GLUI_Translation *trans_pro1y = 
    glui_proj2->add_translation_to_panel(panel_proj_pos, "Y", GLUI_TRANSLATION_Y, &projection2.projector.param[1], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_proj_pos, false );
  GLUI_Translation *trans_pro1z = 
    glui_proj2->add_translation_to_panel(panel_proj_pos, "Z", GLUI_TRANSLATION_Z, &projection2.projector.param[2], 2, control_cb);
  
  GLUI_Panel *panel_proj_ori = glui_proj2->add_rollout( "Projector Orientation", true );
  GLUI_Spinner *spinner_pro1rx = glui_proj2->add_spinner_to_panel(panel_proj_ori, "rX", GLUI_SPINNER_FLOAT, &projection2.projector.param[3], 2, control_cb);
  spinner_pro1rx->set_speed(1.0);  
  GLUI_Spinner *spinner_pro1ry = glui_proj2->add_spinner_to_panel(panel_proj_ori, "rY", GLUI_SPINNER_FLOAT, &projection2.projector.param[4], 2, control_cb);
  spinner_pro1ry->set_speed(1.0);  
  GLUI_Spinner *spinner_pro1rz = glui_proj2->add_spinner_to_panel(panel_proj_ori, "rZ", GLUI_SPINNER_FLOAT, &projection2.projector.param[5], 2, control_cb);
  spinner_pro1rz->set_speed(1.0);  

  glui_proj2->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1rx = glui_proj2->add_translation_to_panel(panel_proj_ori, "rX", GLUI_TRANSLATION_X, &projection2.projector.param[3], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1ry = glui_proj2->add_translation_to_panel(panel_proj_ori, "rY", GLUI_TRANSLATION_Y, &projection2.projector.param[4], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_proj_ori, false );
  GLUI_Translation *trans_pro1rz = glui_proj2->add_translation_to_panel(panel_proj_ori, "rZ", GLUI_TRANSLATION_Z, &projection2.projector.param[5], 2, control_cb);

  ////////////////////////////////////////////////////////////////

  GLUI_Panel *panel_pmir_pos = glui_proj2->add_rollout( "Plain Mirror Position", false );
  GLUI_Spinner *spinner_pmirx = glui_proj2->add_spinner_to_panel(panel_pmir_pos, "X", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[0], 2, control_cb);
  spinner_pmirx->set_speed(10.0);
  GLUI_Spinner *spinner_pmiry = glui_proj2->add_spinner_to_panel(panel_pmir_pos, "Y", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[1], 2, control_cb);
  spinner_pmiry->set_speed(10.0);
  GLUI_Spinner *spinner_pmirz = glui_proj2->add_spinner_to_panel(panel_pmir_pos, "Z", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[2], 2, control_cb);
  spinner_pmirz->set_speed(10.0);

  glui_proj2->add_column_to_panel(panel_pmir_pos, false );

  GLUI_Translation *trans_pmirx =
    glui_proj2->add_translation_to_panel(panel_pmir_pos, "X", GLUI_TRANSLATION_X, &projection2.pmirror.param[0], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_pmir_pos, false );
  GLUI_Translation *trans_pmiry =
    glui_proj2->add_translation_to_panel(panel_pmir_pos, "Y", GLUI_TRANSLATION_Y, &projection2.pmirror.param[1], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_pmir_pos, false );
  GLUI_Translation *trans_pmirz =
    glui_proj2->add_translation_to_panel(panel_pmir_pos, "Z", GLUI_TRANSLATION_Z, &projection2.pmirror.param[2], 2, control_cb);

  GLUI_Panel *panel_pmir_ori = glui_proj2->add_rollout( "Plain Mirror Orientation", false );
  GLUI_Spinner *spinner_pmirrx = glui_proj2->add_spinner_to_panel(panel_pmir_ori, "rX", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[3], 2, control_cb);
  spinner_pmirrx->set_speed(1.0);
  GLUI_Spinner *spinner_pmirry = glui_proj2->add_spinner_to_panel(panel_pmir_ori, "rY", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[4], 2, control_cb);
  spinner_pmirry->set_speed(1.0);
  GLUI_Spinner *spinner_pmirrz = glui_proj2->add_spinner_to_panel(panel_pmir_ori, "rZ", GLUI_SPINNER_FLOAT, &projection2.pmirror.param[5], 2, control_cb);
  spinner_pmirrz->set_speed(1.0);

  glui_proj2->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirrx = glui_proj2->add_translation_to_panel(panel_pmir_ori, "rX", GLUI_TRANSLATION_X, &projection2.pmirror.param[3], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirry = glui_proj2->add_translation_to_panel(panel_pmir_ori, "rY", GLUI_TRANSLATION_Y, &projection2.pmirror.param[4], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_pmir_ori, false );
  GLUI_Translation *trans_pmirrz = glui_proj2->add_translation_to_panel(panel_pmir_ori, "rZ", GLUI_TRANSLATION_Z, &projection2.pmirror.param[5], 2, control_cb);

  ////////////////////////////////////////////////////////////////

  GLUI_Panel *panel_cmir_pos = glui_proj2->add_rollout( "Convex Mirror Position", false );

  GLUI_Spinner *spinner_cmir_radius = glui_proj2->add_spinner_to_panel(panel_cmir_pos, "Radius", GLUI_SPINNER_FLOAT, &projection2.cmirror_radius, 2, control_cb);
  spinner_cmir_radius->set_float_limits(0.0, 500.0);

  GLUI_Spinner *spinner_cmirx = glui_proj2->add_spinner_to_panel(panel_cmir_pos, "X", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[0], 2, control_cb);
  spinner_cmirx->set_speed(10.0);
  GLUI_Spinner *spinner_cmiry = glui_proj2->add_spinner_to_panel(panel_cmir_pos, "Y", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[1], 2, control_cb);
  spinner_cmiry->set_speed(10.0);
  GLUI_Spinner *spinner_cmirz = glui_proj2->add_spinner_to_panel(panel_cmir_pos, "Z", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[2], 2, control_cb);
  spinner_cmirz->set_speed(10.0);

  glui_proj2->add_column_to_panel(panel_cmir_pos, false );

  GLUI_Translation *trans_cmirx =
    glui_proj2->add_translation_to_panel(panel_cmir_pos, "X", GLUI_TRANSLATION_X, &projection2.cmirror.param[0], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_cmir_pos, false );
  GLUI_Translation *trans_cmiry =
    glui_proj2->add_translation_to_panel(panel_cmir_pos, "Y", GLUI_TRANSLATION_Y, &projection2.cmirror.param[1], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_cmir_pos, false );
  GLUI_Translation *trans_cmirz =
    glui_proj2->add_translation_to_panel(panel_cmir_pos, "Z", GLUI_TRANSLATION_Z, &projection2.cmirror.param[2], 2, control_cb);

  GLUI_Panel *panel_cmir_ori = glui_proj2->add_rollout( "Convex Mirror Orientation", false );
  GLUI_Spinner *spinner_cmirrx = glui_proj2->add_spinner_to_panel(panel_cmir_ori, "rX", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[3], 2, control_cb);
  spinner_cmirrx->set_speed(1.0);
  GLUI_Spinner *spinner_cmirry = glui_proj2->add_spinner_to_panel(panel_cmir_ori, "rY", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[4], 2, control_cb);
  spinner_cmirry->set_speed(1.0);
  GLUI_Spinner *spinner_cmirrz = glui_proj2->add_spinner_to_panel(panel_cmir_ori, "rZ", GLUI_SPINNER_FLOAT, &projection2.cmirror.param[5], 2, control_cb);
  spinner_cmirrz->set_speed(1.0);

  glui_proj2->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirrx = glui_proj2->add_translation_to_panel(panel_cmir_ori, "rX", GLUI_TRANSLATION_X, &projection2.cmirror.param[3], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirry = glui_proj2->add_translation_to_panel(panel_cmir_ori, "rY", GLUI_TRANSLATION_Y, &projection2.cmirror.param[4], 2, control_cb);
  glui_proj2->add_column_to_panel(panel_cmir_ori, false );
  GLUI_Translation *trans_cmirrz = glui_proj2->add_translation_to_panel(panel_cmir_ori, "rZ", GLUI_TRANSLATION_Z, &projection2.cmirror.param[5], 2, control_cb);

  ////////////////////////////////////////////////////////////////
  GLUI_Panel *panel_proj_para = glui_proj2->add_rollout( "Projection Parameter", true );

  GLUI_EditText *vi_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Active Pix", GLUI_EDITTEXT_INT, &projection1.vi);
  vi_edittext->disable();

  static float ratio_vi=(float)projection2.vi/((NUM_I+1)*(NUM_J+1))*100.0;
  GLUI_EditText *ratio_vi_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Ratio[%]", GLUI_EDITTEXT_FLOAT, &ratio_vi);
  ratio_vi_edittext->disable();

  GLUI_EditText *lux_min_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Lux min[lx]", GLUI_EDITTEXT_FLOAT, &projection2.lux_min);
  lux_min_edittext->disable();

  GLUI_EditText *pix_min_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Pix min[mm]", GLUI_EDITTEXT_FLOAT, &projection2.pix_min);
  pix_min_edittext->disable();
  
  GLUI_EditText *view_dn_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "View low[deg]", GLUI_EDITTEXT_FLOAT, &projection2.view_low);
  view_dn_edittext->disable();

  GLUI_EditText *view_lt_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "View left[deg]", GLUI_EDITTEXT_FLOAT, &projection2.view_left);
  view_lt_edittext->disable();

  glui_proj2->add_column_to_panel(panel_proj_para, false );
  
  static int total_vi=(NUM_I+1)*(NUM_J+1);
  GLUI_EditText *total_vi_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Total Pix", GLUI_EDITTEXT_INT, &total_vi);
  total_vi_edittext->disable();

  static float pix_diag=(float)projection2.all_sum/projection2.vi;
  GLUI_EditText *pix_diag_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Ave.Diag[pix]", GLUI_EDITTEXT_FLOAT, &pix_diag);
  pix_diag_edittext->disable();

  GLUI_EditText *lux_max_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Lux max[lx]", GLUI_EDITTEXT_FLOAT, &projection2.lux_max);
  lux_max_edittext->disable();

  GLUI_EditText *pix_max_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "Pix max[mm]", GLUI_EDITTEXT_FLOAT, &projection2.pix_max);
  pix_max_edittext->disable();
  
  GLUI_EditText *view_up_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "View up[deg]", GLUI_EDITTEXT_FLOAT, &projection2.view_up);
  view_up_edittext->disable();

  GLUI_EditText *view_rg_edittext =
    glui_proj2->add_edittext_to_panel(panel_proj_para, "View right[deg]", GLUI_EDITTEXT_FLOAT, &projection2.view_right);
  view_rg_edittext->disable();

  void table_calc(int);

  GLUI_Panel *panel_proj2 = glui_proj2->add_rollout( "Data Handling", false );
  glui_proj2->add_edittext_to_panel(panel_proj2, "Status file", GLUI_EDITTEXT_TEXT, projection1.filename);
  glui_proj2->add_button_to_panel(panel_proj2, "Save", 32, control_cb);
  glui_proj2->add_button_to_panel(panel_proj2, "Load", 128, control_cb);
  glui_proj2->add_button_to_panel(panel_proj2, "Calc", 2, table_calc );
  glui_proj2->add_edittext_to_panel(panel_proj2, "Projector file", GLUI_EDITTEXT_TEXT, projection2.proj_filename);
 
  glui_proj2->hide();
}


void glui_main(void)
{
  glui = GLUI_Master.create_glui("Virtual Projector",0,700,0);
  glui_projector1();
  glui_projector2();

  glui->add_statictext( "Virtual Projector 2015" );
  glui->add_separator();

  GLUI_Rollout *options_sel_proj = glui->add_rollout( "Projector Select",true);
  glui->add_checkbox_to_panel( options_sel_proj, "projector1", &toggle_projection1, 8, control_cb );

  glui->add_column_to_panel(options_sel_proj, false );

  glui->add_checkbox_to_panel( options_sel_proj, "projector2", &toggle_projection2, 8, control_cb );
  
  GLUI_Panel *panel_scrn = glui->add_rollout( "Screen Param", false );
  
  GLUI_Spinner *spinner_scr_radius = glui->add_spinner_to_panel(panel_scrn, "Screen Radius", GLUI_SPINNER_FLOAT, &screen_radius, 3, control_cb);
  spinner_scr_radius->set_speed(1.0);  
  GLUI_Spinner *spinner_scr_view = glui->add_spinner_to_panel(panel_scrn,"Screen H View", GLUI_SPINNER_FLOAT, &screen_view, 3, control_cb);
  spinner_scr_view->set_speed(10.0);  
  spinner_scr_view->set_float_limits(0.0, 180.0);

  GLUI_Spinner *spinner_scr_up = glui->add_spinner_to_panel(panel_scrn,"Screen UP", GLUI_SPINNER_FLOAT, &screen_up, 3, control_cb);
  spinner_scr_up->set_speed(10.0);  
  spinner_scr_up->set_float_limits(-90.0, 90.0);
  GLUI_Spinner *spinner_scr_down = glui->add_spinner_to_panel(panel_scrn,"Screen DOWN", GLUI_SPINNER_FLOAT, &screen_down, 3, control_cb);
  spinner_scr_down->set_speed(10.0);  
  spinner_scr_down->set_float_limits(-90.0, 90.0);

  GLUI_Panel *panel_prov = glui->add_rollout( "Projector View", false );

  GLUI_Spinner *spinner_proj_btm = glui->add_spinner_to_panel(panel_prov, "Projector bottom", GLUI_SPINNER_FLOAT, &projection1.projector_bottom, 7, control_cb);
  spinner_proj_btm->set_speed(10.0);  

  GLUI_Spinner *spinner_proj_top = glui->add_spinner_to_panel(panel_prov,"Projector top", GLUI_SPINNER_FLOAT, &projection1.projector_top, 7, control_cb);
  spinner_proj_top->set_speed(10.0);  

  GLUI_Spinner *spinner_proj_lft = glui->add_spinner_to_panel(panel_prov,"Projector left", GLUI_SPINNER_FLOAT, &projection1.projector_left, 7, control_cb);
  spinner_proj_lft->set_speed(10.0);  

  GLUI_Spinner *spinner_proj_rgt = glui->add_spinner_to_panel(panel_prov,"Projector right", GLUI_SPINNER_FLOAT, &projection1.projector_right, 7, control_cb);
  spinner_proj_rgt->set_speed(10.0);  

  GLUI_Spinner *spinner_proj_fcs = glui->add_spinner_to_panel(panel_prov,"Projector focus", GLUI_SPINNER_FLOAT, &projection1.projector_focus, 7, control_cb);
  spinner_proj_fcs->set_speed(10.0);  

  GLUI_Spinner *spinner_proj_zom = glui->add_spinner_to_panel(panel_prov,"Projector zoom", GLUI_SPINNER_FLOAT, &projection1.projector_zoomratio, 7, control_cb);
  spinner_proj_zom->set_speed(1.0);  
  spinner_proj_zom->set_float_limits(0.8, 1.2);
  
  GLUI_Panel *panel_scrn_pos = glui->add_rollout( "Screen Position", false );
  GLUI_Spinner *spinner_scrx = glui->add_spinner_to_panel(panel_scrn_pos, "X", GLUI_SPINNER_FLOAT, &projection1.screen.param[0], 3, control_cb);
  spinner_scrx->set_speed(10.0);  
  GLUI_Spinner *spinner_scry = glui->add_spinner_to_panel(panel_scrn_pos, "Y", GLUI_SPINNER_FLOAT, &projection1.screen.param[1], 3, control_cb);
  spinner_scry->set_speed(10.0);  
  GLUI_Spinner *spinner_scrz = glui->add_spinner_to_panel(panel_scrn_pos, "Z", GLUI_SPINNER_FLOAT, &projection1.screen.param[2], 3, control_cb);
  spinner_scrz->set_speed(10.0);  
  
  glui->add_column_to_panel(panel_scrn_pos, false );
  GLUI_Translation *trans_scrx = 
    glui->add_translation_to_panel(panel_scrn_pos, "X", GLUI_TRANSLATION_X, &projection1.screen.param[0], 3, control_cb);
  glui->add_column_to_panel(panel_scrn_pos, false );
  GLUI_Translation *trans_scry = 
    glui->add_translation_to_panel(panel_scrn_pos, "Y", GLUI_TRANSLATION_Y, &projection1.screen.param[1], 3, control_cb);
  glui->add_column_to_panel(panel_scrn_pos, false );
  GLUI_Translation *trans_scrz = 
    glui->add_translation_to_panel(panel_scrn_pos, "Z", GLUI_TRANSLATION_Z, &projection1.screen.param[2], 3, control_cb);

  GLUI_Panel *panel_scrn_ori = glui->add_rollout( "Screen Orientation", false );
  GLUI_Spinner *spinner_scrrx = glui->add_spinner_to_panel(panel_scrn_ori, "rX", GLUI_SPINNER_FLOAT, &projection1.screen.param[3], 3, control_cb);
  spinner_scrrx->set_speed(1.0);  
  GLUI_Spinner *spinner_scrry = glui->add_spinner_to_panel(panel_scrn_ori, "rY", GLUI_SPINNER_FLOAT, &projection1.screen.param[4], 3, control_cb);
  spinner_scrry->set_speed(1.0);  
  GLUI_Spinner *spinner_scrrz = glui->add_spinner_to_panel(panel_scrn_ori, "rZ", GLUI_SPINNER_FLOAT, &projection1.screen.param[5], 3, control_cb);
  spinner_scrrz->set_speed(1.0);  

  glui->add_column_to_panel(panel_scrn_ori, false );
  GLUI_Translation *trans_scrrx = glui->add_translation_to_panel(panel_scrn_ori, "rX", GLUI_TRANSLATION_X, &projection1.screen.param[3], 3, control_cb);
  glui->add_column_to_panel(panel_scrn_ori, false );
  GLUI_Translation *trans_scrry = glui->add_translation_to_panel(panel_scrn_ori, "rY", GLUI_TRANSLATION_Y, &projection1.screen.param[4], 3, control_cb);
  glui->add_column_to_panel(panel_scrn_ori, false );
  GLUI_Translation *trans_scrrz = glui->add_translation_to_panel(panel_scrn_ori, "rZ", GLUI_TRANSLATION_Z, &projection1.screen.param[5], 3, control_cb);

  GLUI_Panel *panel_view_pos = glui->add_rollout( "Viewpoint position", false );
  GLUI_Spinner *spinner_vpx = glui->add_spinner_to_panel(panel_view_pos, "X", GLUI_SPINNER_FLOAT, &view_pos[0], 3, control_cb);
  spinner_vpx->set_speed(1.0);  
  GLUI_Spinner *spinner_vpy = glui->add_spinner_to_panel(panel_view_pos, "Y", GLUI_SPINNER_FLOAT, &view_pos[1], 3, control_cb);
  spinner_vpy->set_speed(1.0);  
  GLUI_Spinner *spinner_vpz = glui->add_spinner_to_panel(panel_view_pos, "Z", GLUI_SPINNER_FLOAT, &view_pos[2], 3, control_cb);
  spinner_vpz->set_speed(1.0);  

  glui->add_column_to_panel(panel_view_pos, false );
  GLUI_Translation *trans_vpx = glui->add_translation_to_panel(panel_view_pos, "X", GLUI_TRANSLATION_X, &view_pos[0], 3, control_cb);
  glui->add_column_to_panel(panel_view_pos, false );
  GLUI_Translation *trans_vpy = glui->add_translation_to_panel(panel_view_pos, "Y", GLUI_TRANSLATION_Y, &view_pos[1], 3, control_cb);
  glui->add_column_to_panel(panel_view_pos, false );
  GLUI_Translation *trans_vpz = glui->add_translation_to_panel(panel_view_pos, "Z", GLUI_TRANSLATION_Z, &view_pos[2], 3, control_cb);
  
  glui->add_separator();

  GLUI_Panel *panel_chkbox = glui->add_rollout( "Toggle Param", false );
  
  //first column
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_pmirror", &toggle_pmirror );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_projection1", &toggle_projection1 );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_lens", &toggle_lens );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_dispose", &toggle_dispose );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_real", &toggle_real );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_four", &toggle_four );

  glui->add_column_to_panel(panel_chkbox, false );
  //second column
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_cmirror", &toggle_cmirror );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_projection2", &toggle_projection2 );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_focus", &toggle_focus );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_view", &toggle_view );
  glui->add_checkbox_to_panel(panel_chkbox, "toggle_calc", &toggle_calc );

  glui->add_button( "Mirror Data Copy", 0, copy_mirror_data);
  glui->add_button( "Quit", 0, exit_prog );
  
  GLUI_Master.set_glutIdleFunc(RedisplayBoth); 
  StatusWindow=glui->get_glut_window_id();
}
