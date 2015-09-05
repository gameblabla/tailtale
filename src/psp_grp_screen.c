/* ---------------------------------------------------------- */
/*  psp_grp_screen.c                                          */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   grp_screen.c                                         */
/*     画面管理クラス                                     */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: debug.pp,v 1.3 2002/07/11 17:21:47 rero2 Exp $ */


/*------------------------------------------------------------- */
/** @file
    @brief		描画スクリーン管理
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <pspkernel.h>
#include <pspdisplay.h>

#include <pspgu.h>
#include <pspgum.h>

#include <stdlib.h>
#include <malloc.h>

#include "grp_screen.h"
#include "grp_table.h"

#include "debug.h"


/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* --- PSP固定値 */
#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)

/* change this if you change to another screenmode */
#define PIXEL_SIZE (4)

/* zbuffer seems to be 16-bit? */
#define ZBUF_SIZE (BUF_WIDTH * SCR_HEIGHT * 2)

/* 大きなスプライトを描画するときの分割サイズ */
#define SLICE_SIZE  64

/* --- GU 描画のためのパケットサイズ */
#define PACKET_SIZE  262144
//static  unsigned int __attribute__((aligned(16))) packet[262144];


struct SpriteVertex
{
  unsigned short u, v;
  unsigned int color;
  short x,y,z;
};

/*-------------------------------*/
/* local function                */
/*-------------------------------*/

/* --- スプライトを一枚スクリーンに貼り付ける */
void Render(TGameScreen *class, TGameSprite *spr);
void RenderZoomRot(TGameScreen *class, TGameSprite *spr);


/* -------------------------------------------------------------- */
/* --- スクリーン管理クラス                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- コンストラクタ・デストラクタ         */
TGameScreen *TGameScreen_Create(int width, int height, int depth)
{
  int  i;

  TGameScreen *class;

  /* --- インスタンスの生成 */
  class = malloc(sizeof(TGameScreen));
  if (class == 0) {
    return(0);
  }
  class->packet = (void*)memalign(16, sizeof(unsigned int) * PACKET_SIZE);
  if (class->packet == 0) {
    free(class);
    return(0);
  }

  /* ----- work memo */
  class->Screen = 0;
  class->Width = width;
  class->Height = height;
  class->Depth = depth;

  /* ----- GU initialise */
  sceGuInit();

  sceGuStart(GU_DIRECT, class->packet);
  if (depth == 16) {
    sceGuDrawBuffer(GU_PSM_5551, (void*)0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)0x44000, BUF_WIDTH);
    sceGuDepthBuffer((void*)0x88000, BUF_WIDTH);
  }
  else {
    sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
    sceGuDepthBuffer((void*)0x110000, BUF_WIDTH);
  }
  sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
  sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
  sceGuDepthRange(0xc350,0x2710);
  sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
  sceGuEnable(GU_SCISSOR_TEST);
  //sceGuAlphaFunc(GU_ALWAYS,0,0xff);
  //sceGuEnable(GU_ALPHA_TEST);
  //sceGuDepthFunc(GU_GEQUAL);
  //sceGuEnable(GU_DEPTH_TEST);
  sceGuFrontFace(GU_CW);
  sceGuShadeModel(GU_SMOOTH);
  sceGuEnable(GU_CULL_FACE);
  sceGuEnable(GU_BLEND);
  sceGuEnable(GU_TEXTURE_2D);
  sceGuFinish();
  sceGuSync(0,0);

  sceDisplayWaitVblankStart();
  sceGuDisplay(GU_TRUE);

  class->current_buffer_ptr = 0;

  /* --- テクスチャマネージャの初期化 */
  class->Texture = TGameTexture_Create();
  /* --- スプライトマネージャの初期化 */
  for(i=0; i<SPRITEMAX; i++) {
    class->Sprites[i] = TGameSprite_Create();
  }

  /* ----- デバッグプリントの初期化 */
#ifdef DEBUG
  TDebugInit(class, width, height, depth);
#endif

  /* --- ポインタを返して終了 */
  return(class);
}

void TGameScreen_Destroy(TGameScreen *class)
{
  int  i;

  if (class) {
    /* ----- テクスチャの解放 */
    TGameTexture_Destroy(class->Texture);
    /* ----- スプライトの解放 */
    for(i=0; i<SPRITEMAX; i++) {
      TGameSprite_Destroy(class->Sprites[i]);
    }
    /* ----- デバッグも終了 */
#ifdef DEBUG
    TDebugFree();
#endif
    /* ----- インスタンスの破棄 */
    free(class);
  }
}

/* ---------------------------------------- */
/* --- ウィンドウタイトルの設定             */
void TGameScreen_SetWMName(TGameScreen *class, char *name)
{
}


/* ---------------------------------------- */
/* --- フレームタイミングによる全描画       */
void TGameScreen_DispScreen(TGameScreen *class)
{
  int  i;
  int  texture_cache;
  void  *texture_cache_ptr;

  texture_cache = -1;
  texture_cache_ptr = 0;

  /* --- スプライト描画 */
  for(i=0; i<SPRITEMAX; i++) {
    if ((class->Sprites[i]->DispSw == TRUE) &&
	(class->Sprites[i]->Texture != NULL)) {
      /* --- テクスチャの転送が必要なら */
      if ((class->Sprites[i]->TextureId != texture_cache) ||
	  (class->Sprites[i]->Texture != texture_cache_ptr)) {
	texture_cache = class->Sprites[i]->TextureId;
	texture_cache_ptr = class->Sprites[i]->Texture;
	/* - テクスチャ転送コマンド */
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	if (class->Sprites[i]->Texture->format->BitsPerPixel == 16) {
	  sceGuTexMode(GU_PSM_5551,0,0,0);
	}
	else {
	  sceGuTexMode(GU_PSM_8888,0,0,0);
	}
	sceGuTexImage(0,
		      class->Sprites[i]->Texture->w,
		      class->Sprites[i]->Texture->h,
		      class->Sprites[i]->Texture->w,
		      class->Sprites[i]->Texture->pixels);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
      }
      /* --- スプライトの描画 */
      if ((class->Sprites[i]->zoomx == 1.0) &&
	  (class->Sprites[i]->zoomy == 1.0) &&
	  (class->Sprites[i]->rotation_z == 0.0)) {
	/* 回転拡大縮小なし、単純矩形コピー */
	Render(class, class->Sprites[i]);
      }
      else {
	/* 回転拡大縮小あり */
	RenderZoomRot(class, class->Sprites[i]);
      }
    }
  }
}

void TGameScreen_RefreshScreen(TGameScreen *class)
{
  /* - パケット描画終了待ち */
  sceGuSync(0, 0);

  /* --- VSync and swap frame buffer */
  sceDisplayWaitVblankStart();
  class->current_buffer_ptr = (unsigned char *)sceGuSwapBuffers();

  /* --- スクリーンアップデート */
  sceGuStart(GU_DIRECT, class->packet);

  /* --- 画面クリア */
  sceGuClearColor(0xff601010);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);


  /* -- 画面更新 */
  TGameScreen_DispScreen(class);

  /* -- 開発デバッグフォント */
#ifdef DEBUG
  TDebugDisp(class);
#endif

  /* - パケット積み、ここまで */
  sceGuFinish();

}


/* ---------------------------------------- */
/* --- テクスチャをロードする               */

void TGameScreen_LoadTexture(TGameScreen *class,
			     int num,
			     char *filename)
{
  TGameTexture_Load(class->Texture, num, filename, 0, TRUE);
}

void TGameScreen_LoadTexturePure(TGameScreen *class,
				 int num,
				 char *filename)
{
  TGameTexture_Load(class->Texture, num, filename, 0, FALSE);
}


/* ---------------------------------------- */
/* --- スプライトを渡す                     */
TGameSprite *TGameScreen_GetSprite(TGameScreen *class, int id)
{
  if (id < 0) return(0);
  if (id > SPRITEMAX) return(0);

  return(class->Sprites[id]);
}


/* ---------------------------------------- */
/* --- テクスチャを渡す                     */
SDL_Surface *TGameScreen_GetTexture(TGameScreen *class, int id)
{
  return(TGameTexture_GetTexture(class->Texture, id));
}


/* ---------------------------------------- */
/* --- スプライトの表示(画面転送)           */
void Render(TGameScreen *class, TGameSprite *spr)
{
  struct SpriteVertex* vertices;
  int  seg, pos, w_size, w_rest;
  int  i;
  unsigned int  blendlevel;

  /* --- 表示スイッチ */
  if (spr->DispSw == FALSE) return;
  if (spr->Texture == NULL) return;

  /* --- 半透明合成値 */
  blendlevel = ((spr->alpha & 0xff) << 24) | 0xffffff;

  /* --- スプライトを一枚描画する */
  /* ある程度の大きさを持ったスプライトは短冊状に分割しないと */
  /* とてつもなく遅くなる */
  seg = (spr->w / SLICE_SIZE) + 1;
  vertices = (struct SpriteVertex*)sceGuGetMemory(seg * 2 * sizeof(struct SpriteVertex));
  pos = 0;
  w_rest = spr->w;
  for(i=0; i<seg; i++) {
    if ((i+1) < seg) {
      w_size = SLICE_SIZE;
      w_rest -= SLICE_SIZE;
    }
    else {
      w_size = w_rest;
    }
    vertices[i*2].u = spr->tx + pos;
    vertices[i*2].v = spr->ty;
    vertices[i*2].color = blendlevel;
    vertices[i*2].x = spr->x + pos;
    vertices[i*2].y = spr->y;
    vertices[i*2].z = 0;
    vertices[i*2+1].u = spr->tx + pos + w_size;
    vertices[i*2+1].v = spr->ty + spr->h;
    vertices[i*2+1].color = blendlevel;
    vertices[i*2+1].x = spr->x + pos + w_size;
    vertices[i*2+1].y = spr->y + spr->h;
    vertices[i*2+1].z = 0;
    pos += SLICE_SIZE;
  }
  /* --- 描画リクエスト */
  sceGuDrawArray(GU_SPRITES,
		 GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D,
		 (seg * 2), 0, vertices);
}


/* ---------------------------------------- */
/* --- スプライトの表示(回転拡大縮小あり) */
void RenderZoomRot(TGameScreen *class, TGameSprite *spr)
{
  struct SpriteVertex* vertices;
  int  seg, pos, w_size, w_rest;
  int  i, j;
  unsigned int  blendlevel;
  int  rot;
  float  center_x, center_y;
  float  fx, fy, rx, ry;

  /* --- 表示スイッチ */
  if (spr->DispSw == FALSE) return;
  if (spr->Texture == NULL) return;

  /* --- 回転拡大前処理 */
  rot = (int)spr->rotation_z;
  rot %= 360;
  rot = 359 - rot;

  /* --- 半透明合成値 */
  blendlevel = ((spr->alpha & 0xff) << 24) | 0xffffff;

  /* --- スプライトを一枚描画する */
  /* ある程度の大きさを持ったスプライトは短冊状に分割しないと */
  /* とてつもなく遅くなる */
  seg = (spr->w / SLICE_SIZE) + 1;
  vertices = (struct SpriteVertex*)sceGuGetMemory(seg * 4 * sizeof(struct SpriteVertex));
  pos = 0;
  w_rest = spr->w;
  for(i=0; i<seg; i++) {
    if ((i+1) < seg) {
      w_size = SLICE_SIZE;
      w_rest -= SLICE_SIZE;
    }
    else {
      w_size = w_rest;
    }
    vertices[i*4].u = spr->tx + pos;
    vertices[i*4].v = spr->ty;
    vertices[i*4].color = blendlevel;
    vertices[i*4].x = spr->x + pos;
    vertices[i*4].y = spr->y;
    vertices[i*4].z = 0;
    vertices[i*4+1].u = spr->tx + pos + w_size;
    vertices[i*4+1].v = spr->ty;
    vertices[i*4+1].color = blendlevel;
    vertices[i*4+1].x = spr->x + pos + w_size;
    vertices[i*4+1].y = spr->y;
    vertices[i*4+1].z = 0;
    vertices[i*4+2].u = spr->tx + pos;
    vertices[i*4+2].v = spr->ty + spr->h;
    vertices[i*4+2].color = blendlevel;
    vertices[i*4+2].x = spr->x + pos;
    vertices[i*4+2].y = spr->y + spr->h;
    vertices[i*4+2].z = 0;
    vertices[i*4+3].u = spr->tx + pos + w_size;
    vertices[i*4+3].v = spr->ty + spr->h;
    vertices[i*4+3].color = blendlevel;
    vertices[i*4+3].x = spr->x + pos + w_size;
    vertices[i*4+3].y = spr->y + spr->h;
    vertices[i*4+3].z = 0;
    pos += SLICE_SIZE;
    /* --- 回転拡大処理 */
    center_x = (float)spr->x + ((float)spr->w / 2);
    center_y = (float)spr->y + ((float)spr->h / 2);
    for(j=0; j<4; j++) {
      fx = (float)vertices[i*4+j].x - center_x;
      fy = (float)vertices[i*4+j].y - center_y;
      rx = ((fx * sprite_cos[rot]) - (fy * sprite_sin[rot])) * spr->zoomx;
      ry = ((fx * sprite_sin[rot]) + (fy * sprite_cos[rot])) * spr->zoomy;
      rx += center_x;
      ry += center_y;
      vertices[i*4+j].x = (short)rx;
      vertices[i*4+j].y = (short)ry;
    }
  }
  /* --- 描画リクエスト */
  sceGuDrawArray(GU_TRIANGLE_STRIP,
		 GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D,
		 (seg * 4), 0, vertices);

}


