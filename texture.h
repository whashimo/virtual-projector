/********************************************************
  texture.h
 ********************************************************/

typedef struct{
  unsigned char *tex_img;
  int tex_id;
  int tex_list;
}TEXTURE;

/* �e�N�X�`���t�@�C�����J�� */
/* open_tga_texture(char *filename, TEXTURE *texture, int width, int height),
filename:TGA�t�@�C����
texture:  �e�N�X�`���\����
width:   �e�N�X�`��Width
height:  �e�N�X�`��Height */
#ifdef __cplusplus
extern "C"{
#endif

void open_tga_texture(char *, TEXTURE *, int , int );
/* �A���t�@�`�����l���t���摜�̏ꍇ */
void open_tga_texture_alpha(char *, TEXTURE *, int , int );

/* �e�N�X�`��������������폜���� */
/* delete_texture(TEXTURE *texture)
texture:  �e�N�X�`���\���� */

void delete_texture(TEXTURE *);

void checker_texture(TEXTURE *);

#ifdef __cplusplus
}
#endif