/********************************************************
  texture.h
 ********************************************************/

typedef struct{
  unsigned char *tex_img;
  int tex_id;
  int tex_list;
}TEXTURE;

/* テクスチャファイルを開く */
/* open_tga_texture(char *filename, TEXTURE *texture, int width, int height),
filename:TGAファイル名
texture:  テクスチャ構造体
width:   テクスチャWidth
height:  テクスチャHeight */
#ifdef __cplusplus
extern "C"{
#endif

void open_tga_texture(char *, TEXTURE *, int , int );
/* アルファチャンネル付き画像の場合 */
void open_tga_texture_alpha(char *, TEXTURE *, int , int );

/* テクスチャをメモリから削除する */
/* delete_texture(TEXTURE *texture)
texture:  テクスチャ構造体 */

void delete_texture(TEXTURE *);

void checker_texture(TEXTURE *);

#ifdef __cplusplus
}
#endif