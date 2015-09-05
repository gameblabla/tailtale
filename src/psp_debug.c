/* ---------------------------------------------------------- */
/*  debug.c                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   debug.c                                             */
/*     デバッグ用チェックルーチン                         */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: debug.pp,v 1.3 2002/07/11 17:21:47 rero2 Exp $ */


/*------------------------------------------------------------- */
/** @file
    @brief		デバッグチェックルーチン
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <pspgu.h>
#include <pspgum.h>

#include "debug.h"
#include "grp_screen.h"

/*-------------------------------*/
/* GU packet                     */
/*-------------------------------*/

struct DebugVertex
{
  unsigned short u, v;
  unsigned int color;
  short x,y,z;
};

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* ----- デバッグルーチンが有効か否か(ASCIIが準備出来てるか) */
int  UseDebug;

/* ----- 画面の大きさ */
int  debug_width;
int  debug_height;

/* ----- アスキーフォントテクスチャー */
SDL_Surface *AsciiFont;

/* ----- アスキープレーン */
SDL_Surface *AsciiPlane;

/* ----- 表示文字列 */
char StockString[MessageMax][128];
int  StockNum;

/* ----- アスキーテクスチャー名 */
char *TextureName = "ASCII.BMP";

/* --- デバッグフォント文字列 */
char  debug_line[128];


/* -------------------------------------------------------------- */
/* --- デバッグ用チェックルーチン                                 */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- メッセージのライン表示    */
void  print_msg(char *mes, int disp_x, int disp_y)
{
  int  i, l;
  int  c;
  struct DebugVertex* vertices;

  l = strlen(mes);
  vertices = (struct DebugVertex*)sceGuGetMemory(l * 2 * sizeof(struct DebugVertex));
  for(i=0; i<l; i++) {
    c = mes[i];
    vertices[i*2].u = (c % 16) * 8;
    vertices[i*2].v = (c / 16) * 8;
    vertices[i*2].color = 0;
    vertices[i*2].x = disp_x;
    vertices[i*2].y = disp_y;
    vertices[i*2].z = 0;
    vertices[i*2+1].u = (c % 16) * 8 + 8;
    vertices[i*2+1].v = (c / 16) * 8 + 8;
    vertices[i*2+1].color = 0;
    vertices[i*2+1].x = disp_x + 8;
    vertices[i*2+1].y = disp_y + 8;
    vertices[i*2+1].z = 0;
    disp_x = disp_x + 8;
  }
  sceGuDrawArray(GU_SPRITES,
		 GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D,
		 l*2, 0, vertices);
}



/* ---------------------------------------- */
/* --- デバッグフェイス初期化               */
/* ---------------------------------------- */
void  TDebugInit(TGameScreen *screen,
		 int Width, int Height, int Depth)
{
  SDL_Surface *plane;
  void *nonalign;
  int  msize, i;
  unsigned long *pix;

  debug_width = Width;
  debug_height = Height;
  StockNum = 0;
  UseDebug = 1;
  plane = SDL_LoadBMP(TextureName);
  if (plane == NULL) {
    UseDebug = 0;
    return;
  }
  AsciiPlane = SDL_CreateRGBSurface(SDL_SWSURFACE,
				    Width, Height, 32,
				    DRmask, DGmask, DBmask, DAmask);
  if (AsciiPlane == NULL) {
    UseDebug = 0;
  }
  AsciiFont = SDL_ConvertSurface(plane, AsciiPlane->format, SDL_SWSURFACE);
  if (AsciiFont == NULL) {
    UseDebug = 0;
  }
  nonalign = AsciiFont->pixels;
  msize = (AsciiFont->w * AsciiFont->h) * AsciiFont->format->BytesPerPixel;
  AsciiFont->pixels = (void*)memalign(16, msize);
  memcpy(AsciiFont->pixels, nonalign, msize);
  free(nonalign);
  SDL_SetColorKey(AsciiFont, SDL_SRCCOLORKEY, 0x000000);
  SDL_FreeSurface(plane);
  /* --- ColorKey イコールα値じゃないようなので */
  /*    自前でα値設定 */
  pix = (unsigned long*)AsciiFont->pixels;
  msize = (AsciiFont->w * AsciiFont->h);
  for(i=0; i<msize; i++) {
    if (((*pix) & 0xffffff) == 0x007d00) {
      *pix &= 0x00ffffff;
    }
    else {
      *pix |= 0xff000000;
    }
    pix++;
  }
}


/* ---------------------------------------- */
/* --- デバッグフェイス解放                 */
/* ---------------------------------------- */
void  TDebugFree()
{
  SDL_FreeSurface(AsciiPlane);
  SDL_FreeSurface(AsciiFont);
  UseDebug = 0;
}


/* ---------------------------------------- */
/* --- デバッグフェイスの定期描画           */
/* ---------------------------------------- */
void  TDebugDisp(TGameScreen *screen)
{
  int  disp_x, disp_y;
  int  i;
  void *pixels;

  /* --- 準備が出来ていなかったら回避 */
  if (UseDebug == 0) {
    return;
  }

  /* --- アスキープレーンをスクリーンに */
  if (StockNum > 0) {
    pixels = AsciiFont->pixels;

    /* --- テクスチャの設定 */
    sceGuEnable(GU_TEXTURE_2D);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuTexMode(GU_PSM_8888,0,0,0);
    sceGuTexImage(0, 128, 64, 128, pixels);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexScale(1.0f,1.0f);
    sceGuTexOffset(0.0f,0.0f);
    sceGuAmbientColor(0xffffffff);
    //sceGuTexSync();
    sceGuColor(0xffffffff);

    /* --- 表示キューに積まれている分だけ表示 */
    disp_x = 0;
    disp_y = 0;
    for(i=0; i<StockNum; i++) {
      print_msg(StockString[i], disp_x, disp_y);
      disp_y = disp_y + 8;
    }

    /* --- 次のためにクリア */
    //SDL_FillRect(AsciiPlane, 0, 0x00000000);
  }
  StockNum = 0;  
}


/* ---------------------------------------- */
/* --- デバッグメッセージのキューイング     */
/* ---------------------------------------- */
void  TDebugPrint(char *mes)
{
  if (StockNum < MessageMax) {
    strncpy(StockString[StockNum], mes, 127);
    StockNum = StockNum + 1;
  }
}


/* ---------------------------------------- */
/* --- 数字を文字列に変換                   */
/* ---------------------------------------- */
void  IntToStr(int num, char *buf)
{
#ifndef NO_SNPRINTF
  snprintf(buf, 127, "%d", num);
#endif
}


/* ---------------------------------------- */
/* --- 16進数表示用                         */
/* ---------------------------------------- */
void  IntToHex(int num, int length, char *buf)
{
#ifndef NO_SNPRINTF
  snprintf(buf, 127, "%x", num);
#endif
}

