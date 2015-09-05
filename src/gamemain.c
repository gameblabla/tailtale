/* ---------------------------------------------------------- */
/*  gamemain.c                                                */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   gamemain.c                                           */
/*     ゲーム本体のメインフロー                           */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		ゲーム本体ステップ
    @author		K.Kunikane (rerofumi)
    @since		Sep.20.2005
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "gamemain.h"
#include "common.h"
#include "input.h"
#include "sound.h"
#include "debug.h"
#include "puz_trial.h"

enum {
  TitleInit,
  TitleIn,
  TitleMain,
  TitleStart,
  TitleFree,
  PuzzleInit,
  PuzzleMain,
  PuzzleFree,
  ConfigInit,
  ConfigMain,
  ConfigFree
} MainStep;


TGameMain *TGameMain_Create(TGameScreen *mainscreen)
{
  TGameMain *class;

  class = malloc(sizeof(TGameMain));
  if (class == NULL) return(0);

  class->sound_volume = 96;
  class->control_reverse = 0;

  class->screen = mainscreen;
  class->step = TitleInit;
  class->level = LevelEasy;

  return(class);
}

void TGameMain_Destroy(TGameMain *class)
{
  if (class == NULL) return;
  free(class);
}


int TGameMain_Poll(TGameMain *class,
		    int counter)
{
  int  i;
  int  skip;
  TGameSprite  *sp;
  SDL_Surface  *texkey;

  if (class == NULL) return(FALSE);

  skip = TRUE;


#ifdef __GP2X__
  i = InputJoyKeyTriger(0);
  if ((i & IN_GP_VOLUP) != 0) {
    if (class->sound_volume < 128) {
      class->sound_volume += 16;
      if (class->sound_volume > 128) {
	class->sound_volume = 128;
      }
      SoundVolume(class->sound_volume);
    }
  }
  if ((i & IN_GP_VOLDOWN) != 0) {
    if (class->sound_volume > 0) {
      class->sound_volume -= 16;
      if (class->sound_volume < 0) {
	class->sound_volume = 0;
      }
      SoundVolume(class->sound_volume);
    }
  }
  DEBUGPRINT("VOLUME : %d", class->sound_volume);
#endif

  switch(class->step) {

  case TitleInit:
    TGameScreen_LoadTexture(class->screen, 0, "title_320.bmp");
    texkey = TGameScreen_GetTexture(class->screen, 0);
    SDL_SetColorKey(texkey,
		    (SDL_SRCCOLORKEY | SDL_ACCELERATION_RLE),
		    SDL_MapRGB(texkey->format,
			       0x20, 0x40, 0x80));
    class->bg = TGameScreen_GetSprite(class->screen, 0);
    class->logo = TGameScreen_GetSprite(class->screen, 1);
    class->push = TGameScreen_GetSprite(class->screen, 2);
    class->release = TGameScreen_GetSprite(class->screen, 3);
    class->select_level = TGameScreen_GetSprite(class->screen, 4);
    class->bg->DispSw = TRUE;
    class->bg->x = 0;
    class->bg->y = 0;
    class->bg->w = 320;
    class->bg->h = 240;
    class->bg->tx = 0;
    class->bg->ty = 0;
    class->bg->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->bg->alpha = 255;
    class->logo->DispSw = FALSE;
    class->logo->x = 0;
    class->logo->y = 0;
    class->logo->w = 300;
    class->logo->h = 84;
    class->logo->tx = 0;
    class->logo->ty = 240;
    class->logo->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->logo->alpha = 255;
    class->push->DispSw = FALSE;
    class->push->x = 40;
    class->push->y = 180;
    class->push->w = 240;
    class->push->h = 23;
    class->push->tx = 0;
    class->push->ty = 325;
    class->push->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->push->alpha = 255;
    class->release->DispSw = FALSE;
    class->release->x = 40;
    class->release->y = 210;
    class->release->w = 240;
    class->release->h = 10;
    class->release->tx = 0;
    class->release->ty = 349;
    class->release->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->release->alpha = 255;
    class->select_level->DispSw = FALSE;
    class->select_level->x = 160-40;
    class->select_level->y = 160;
    class->select_level->w = 80;
    class->select_level->h = 18;
    class->select_level->tx = 300;
    class->select_level->ty = 240 + (24 * class->level);
    class->select_level->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->select_level->alpha = 255;
    class->titletimer = 0;
    class->step = TitleIn;
    skip = FALSE;
    break;

  case TitleIn:

    if (class->titletimer == 15*2) SoundSE(1);
    if (class->titletimer > 20*2) {
      class->logo->x = 10;
      class->logo->y = 120 - (int)(sin(3.1415926 * 2.0 * (((float)class->titletimer - 40.0) / 120.0)) * 120.0);
      class->logo->DispSw = TRUE;
    }
    else {
      class->logo->x = 10;
      class->logo->y = 120 - ((40 - class->titletimer) * 10);
      class->logo->DispSw = TRUE;
    }
    class->titletimer = class->titletimer + 1;
    if (class->titletimer == 38*2) {
      SoundSE(6);
    }
    if (class->titletimer == 45*2) {
      SoundMusic(2);
      class->step = TitleMain;
    }
    i = InputJoyKeyTriger(0);
    if ((i & IN_Button1) != 0) {
      SoundMusic(2);
      class->step = TitleMain;
    }
    break;

  case TitleMain:
    class->logo->DispSw = TRUE;
    class->logo->x = 10;
    class->logo->y = 60;
    class->release->DispSw = TRUE;
    class->select_level->DispSw = TRUE;
    if (((class->titletimer / 8) % 2) == 1) {
      class->push->DispSw = TRUE;
    }
    else {
      class->push->DispSw = FALSE;
    }
    class->titletimer = class->titletimer + 1;
    i = InputJoyKeyTriger(0);
    if ((i & (IN_Button1|IN_Button2|IN_Button3|IN_Button4|IN_GP_START)) != 0) {
      SoundSE(2);
      class->titletimer = 0;
      class->step = TitleStart;
    }
    if (i & IN_GP_SELECT) {
      class->step = ConfigInit;
    }

    if ((i & IN_Left) != 0) {
      class->level ^= LevelHard;
    }
    if ((i & IN_Right) != 0) {
      class->level ^= LevelHard;
    }
    class->select_level->ty = 240 + (24 * class->level);
    break;


  case TitleStart:
    class->logo->DispSw = TRUE;
    class->logo->x = 10;
    class->logo->y = 60;
    class->release->DispSw = TRUE;
    if ((class->titletimer % 2) == 1) {
      class->push->DispSw = TRUE;
    }
    else {
      class->push->DispSw = FALSE;
    }
    class->titletimer = class->titletimer + 1;
    if (class->titletimer == 20) {
      SoundMusicStop();
      class->step = TitleFree;
    }
    break;


  case TitleFree:
    srand(counter);
    class->bg->DispSw = FALSE;
    class->logo->DispSw = FALSE;
    class->push->DispSw = FALSE;
    class->release->DispSw = FALSE;
    class->select_level->DispSw = FALSE;
    class->step = PuzzleInit;
    break;


  case PuzzleInit:
    class->puzzle = TPuzzleTrial_Create(class->screen, class->level);
    class->step = PuzzleMain;
    skip = FALSE;
    break;

  case PuzzleMain:
    TPuzzleTrial_GameMain(class->puzzle);
    if (TPuzzleTrial_GameStat(class->puzzle) == FALSE) {
      class->step = PuzzleFree;
    }
    break;

    case PuzzleFree:
      TPuzzleTrial_Destroy(class->puzzle);
      for( i=0; i<SPRITEMAX; i++) 
      {
		sp = TGameScreen_GetSprite(class->screen, i);
		sp->DispSw = FALSE;
      }
      class->step = TitleInit;
      break;

  case ConfigInit:
    TGameScreen_LoadTexture(class->screen, 0, "control.bmp");
    texkey = TGameScreen_GetTexture(class->screen, 0);
    SDL_SetColorKey(texkey,
		    (SDL_SRCCOLORKEY | SDL_ACCELERATION_RLE),
		    SDL_MapRGB(texkey->format,
			       0x00, 0x33, 0x33));
    class->bg = TGameScreen_GetSprite(class->screen, 0);
    class->push = TGameScreen_GetSprite(class->screen, 2);
    class->bg->DispSw = TRUE;
    class->bg->x = 0;
    class->bg->y = 0;
    class->bg->w = 320;
    class->bg->h = 240;
    class->bg->tx = 0;
    class->bg->ty = 0;
    class->bg->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->bg->alpha = 255;
    class->push->DispSw = TRUE;
    class->push->x = 0;
    class->push->y = 48;
    class->push->w = 320;
    class->push->h = 112;
    class->push->tx = 0;
    class->push->ty = 240 + 112*(class->control_reverse ^ 1);
    class->push->Texture = TGameScreen_GetTexture(class->screen, 0);
    class->push->alpha = 255;
    class->logo->DispSw = FALSE;
    class->release->DispSw = FALSE;
    class->select_level->DispSw = FALSE;
    class->control_anime = 0;
    class->control_anime_y = 112*(class->control_reverse ^ 1);
    class->step = ConfigMain;
    break;

  case ConfigMain:
    if (class->control_anime != 0) {
      class->control_anime_y += class->control_anime;
      class->push->ty = 240 + class->control_anime_y;
      if ((class->control_anime_y <= 0) ||
	  (class->control_anime_y >= 112)) {
	class->control_anime = 0;
      }
    }
    else {
      i = InputJoyKeyTriger(0);
      if ((i & (IN_GP_SELECT)) != 0) {
	SoundSE(6);
	class->control_reverse ^= 1;
	if (class->control_reverse == 0) {
	  class->control_anime = 8;
	}
	else {
	  class->control_anime = -8;
	}
      }
      if ((i & (IN_GP_START)) != 0) {
	SoundSE(2);
	class->step = ConfigFree;
      }
    }
    break;

  case ConfigFree:
    InputJoyKeySwap(class->control_reverse);
    class->step = TitleInit;
    break;


  }

  return(skip);
}

