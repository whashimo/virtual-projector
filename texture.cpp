/********************************************************
  Texture handling program
  whashimo@intron.kz.tsukuba.ac.jp
  テクスチャファイルとしてTGA(24bit)を使用
  texture.c
       first version           2002/02/05             
 ********************************************************/
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <malloc.h>
#include "texture.h"

void open_tga_texture(char *filename, TEXTURE *tex, int w, int h)
{
  FILE *fp;
  int x, z;
  
  glGenTextures(1,(GLuint*)&tex->tex_id);
  glBindTexture(GL_TEXTURE_2D, tex->tex_id);
  tex->tex_img=(unsigned char *)malloc(sizeof(unsigned char)*3*w*h);
  
  /* texture file open */
  if((fp=fopen(filename, "rb"))==NULL){
    fprintf(stderr, "%s cannot open\n\r\r", filename);
    return;
  }
  fseek(fp, 18, SEEK_SET);
  for(x=0;x<h;x++){
    for(z=0;z<w;z++){
      *(tex->tex_img+x*w*3+z*3+2)=fgetc(fp);
      *(tex->tex_img+x*w*3+z*3+1)=fgetc(fp);
      *(tex->tex_img+x*w*3+z*3+0)=fgetc(fp);
    }
  }
  fclose(fp);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
               tex->tex_img);

  	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void open_tga_texture_alpha(char *filename, TEXTURE *tex, int w, int h)
{
  FILE *fp;
  int x, z;
  
  glGenTextures(1,(GLuint*)&tex->tex_id);
  glBindTexture(GL_TEXTURE_2D, tex->tex_id);
  tex->tex_img=(unsigned char *)malloc(sizeof(unsigned char)*4*w*h);
  
  /* texture file open */
  if((fp=fopen(filename, "rb"))==NULL){
    fprintf(stderr, "%s cannot open\n\r\r", filename);
    return;
  }
  fseek(fp, 18, SEEK_SET);
  for(x=0;x<h;x++){
    for(z=0;z<w;z++){
      *(tex->tex_img+x*w*4+z*4+2)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+1)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+0)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+3)=fgetc(fp);
    }
  }
  fclose(fp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               tex->tex_img);
//  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void white_alpha(TEXTURE *tex, int w, int h)
{
  int x, z;

  for(x=0;x<h;x++){
    for(z=0;z<w;z++){
		if(*(tex->tex_img+x*w*4+z*4+2) == 255 &&
			*(tex->tex_img+x*w*4+z*4+1) == 255 &&
			*(tex->tex_img+x*w*4+z*4+0) == 255){
                *(tex->tex_img+x*w*4+z*4+3) = 0;
			}
		else
			*(tex->tex_img+x*w*4+z*4+3) = 180;
	}
  }

}
void open_tga_texture_alpha_white(char *filename, TEXTURE *tex, int w, int h)
{
  FILE *fp;
  int x, z, num = 0;
  
  glGenTextures(1,(GLuint*)&tex->tex_id);
  glBindTexture(GL_TEXTURE_2D, tex->tex_id);
  tex->tex_img=(unsigned char *)malloc(sizeof(unsigned char)*4*w*h);
  
  /* texture file open */
  if((fp=fopen(filename, "rb"))==NULL){
    fprintf(stderr, "%s cannot open\n\r\r", filename);
    return;
  }
  fseek(fp, 18, SEEK_SET);
  for(x=0;x<h;x++){
    for(z=0;z<w;z++){
      *(tex->tex_img+x*w*4+z*4+2)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+1)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+0)=fgetc(fp);
      *(tex->tex_img+x*w*4+z*4+3)=fgetc(fp);

		if(*(tex->tex_img+x*w*4+z*4+2) == 0 &&
			*(tex->tex_img+x*w*4+z*4+1) == 0 &&
			*(tex->tex_img+x*w*4+z*4+0) == 0){
               num++;
		}

    }
  }
  fclose(fp);

  white_alpha(tex, w, h);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               tex->tex_img);
//  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}



void delete_texture(TEXTURE *tex)
{
  free(tex->tex_img);
  glDeleteLists(1, tex->tex_list);
  glDeleteTextures(1, (GLuint*)&tex->tex_id);
}

#define TEX_WIDTH 8
#define TEX_HEIGHT 8

//市松模様の床を作る
void checker_texture(TEXTURE *tex)
{
  int i, j, c;
  unsigned char teximage[TEX_WIDTH][TEX_HEIGHT][4];

  for (i=0;i<TEX_HEIGHT;i++) {
    for (j=0;j<TEX_WIDTH;j++) {
      c = ( ((i&0x01)==0)^((j&0x01)==0) );
      teximage[i][j][0]=teximage[i][j][1]=teximage[i][j][2]=48+c*64;
      teximage[i][j][3]=255;
    }
  }
  glGenTextures(1,(GLuint*)&tex->tex_id);
  glBindTexture(GL_TEXTURE_2D, tex->tex_id);
  
//  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT,
	0, GL_RGBA, GL_UNSIGNED_BYTE, teximage);
}

