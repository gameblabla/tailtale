/* ---------------------------------------------------------- */
/*  puz_base.c                                                */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   puz_base.c                                           */
/*     ƒXƒvƒ‰ƒCƒgŠÇ—\‘¢‘Ì                               */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		ƒXƒvƒ‰ƒCƒgŠÇ—
    @author		K.Kunikane (rerofumi)
    @since		Sep.23.2005
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/* --- ¥²¡¼¥à¤ÎÆñ°×ÅÙÀßÄê */
enum {
  LevelEasy = 0,
  LevelNormal,
  LevelHard
} GameLevel;

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdlib.h>

#include "debug.h"
#include "sound.h"
#include "puz_base.h"

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/*-------------------------------*/
/* local function                */
/*-------------------------------*/

void SetBlock(TPuzzleBase *class);
Block *GetBlock(TPuzzleBase *class);
int  GetBlockColor(TPuzzleBase *class);
void  PopupNext(TPuzzleBase *class);
int  PopupWork(TPuzzleBase *class);
int  FieldHeight(TPuzzleBase *class);
int  MoveWork(TPuzzleBase *class);
int  DropRequest(TPuzzleBase *class);
int  DropWork(TPuzzleBase *class);
int  LineCheck(TPuzzleBase *class);
int  LineWork(TPuzzleBase *class);
int  LineCount(TPuzzleBase *class,
	       int x, int y,
	       int dx, int dy,
	       int layer);

/*-------------------------------*/
/* data table                    */
/*-------------------------------*/

int  SwapTable[5] = { 0, 6, 4, 0, 0 };
int  BlockColorSet[7] = { 0, 5, 1, 3, 4, 2, 6 };

/* --- “ïˆÕ“xİ’è */
int  LevelColor_hard[30] = {
  3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
  5, 5, 5, 5, 5, 6, 6, 6, 7, 7  };

int  LevelColor_normal[30] = {
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 5, 5, 5, 5, 5, 5, 5, 6, 6  };

int  LevelColor_easy[30] = {
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 5, 5, 5  };


int  LevelSpeed_easy[30] = {
  240, 220, 220, 200, 200,
  200, 200, 240, 220, 220,
  200, 200, 200, 180, 180,
  160, 160, 200, 200, 180,
  180, 160, 160, 140, 180,
  160, 140, 140, 130, 120 };

int  LevelSpeed_normal[30] = {
  220, 220, 200, 200, 200,
  180, 180, 220, 220, 200,
  200, 180, 180, 180, 180,
  160, 160, 200, 180, 180,
  180, 160, 140, 120, 180,
  140, 140, 120, 120, 100 };

int  LevelSpeed_hard[30] = {
  220, 200, 180, 160, 160,
  140, 140, 200, 200, 180,
  180, 180, 160, 160, 140,
  140, 120, 180, 160, 140,
  140, 120, 120, 100, 140,
  120, 120, 100, 100,  80 };

int  LevelColor_veryhard[30] = {
  3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
  5, 5, 5, 5, 5, 6, 6, 6, 7, 7  };

int  LevelSpeed_veryhard[30] = {
  220, 180, 180, 160, 160,
  160, 140, 200, 180, 160,
  160, 160, 140, 140, 140,
  120, 100, 160, 140, 120,
  120, 100, 100,  80, 140,
  120, 120, 100, 100,  80 };



int  LevelBlock[30] = {
  0,  30,  60, 100, 150,
  200, 250, 300, 350, 400,
  500, 550, 600, 650, 700,
  900, 950,1000,1050,1100,
  1300,1350,1400,1450,1500,
  1800,1950,2100,2300,2500 };



/* -------------------------------------------------------------- */
/* --- ƒeƒNƒXƒ`ƒƒŠÇ—ƒNƒ‰ƒX                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- ƒRƒ“ƒXƒgƒ‰ƒNƒ^EƒfƒXƒgƒ‰ƒNƒ^         */
TPuzzleBase *TPuzzleBase_Create(int difficult)
{
  TPuzzleBase *class;
  int  i;

  /* --- ƒCƒ“ƒXƒ^ƒ“ƒX‚ÌŠm•Û */
  class = malloc(sizeof(TPuzzleBase));
  /* --- Šm•Û‚Å‚«‚½‚ç‰Šú‰» */
  if (class) {
    for(i=0; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
      class->Field[i] = 0;
    }
  }

  class->Difficult = difficult;

  /* --- ƒCƒ“ƒXƒ^ƒ“ƒX‚ğ“n‚µ‚ÄI—¹ */
  return(class);
}

void  TPuzzleBase_Destroy(TPuzzleBase *class)
{
  /* --- ‘¼‚ÉŒãˆ—‚ª‚ ‚è‚Ü‚µ‚½‚ç */
  /* --- ƒCƒ“ƒXƒ^ƒ“ƒX‚Ì‰ğ•ú */
  free(class);
}



/* ---------------------------------------- */
/* --- ƒ[ƒN‚Ì‘S‰Šú‰»                     */
/* ---------------------------------------- */
void TPuzzleBase_GameInit(TPuzzleBase *class, int col)
{
  int  i;

  /* --- ƒtƒB[ƒ‹ƒh”z—ñƒNƒŠƒA */
  for(i=0; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    class->Field[i] = 0;
  }
  /* --- ƒuƒƒbƒNî•ñ‰Šú‰» */
  for(i=0; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    class->Item[i].Color = 0;
    class->Item[i].LineCheck = FALSE;
    class->Item[i].LineBlock = FALSE;
    class->Item[i].LineTimer = 0;
    class->Item[i].PopupTimer = 0;
    class->Item[i].PopupOffset = 0;
    class->Item[i].DropCheck = FALSE;
    class->Item[i].DropTimer = 0;
    class->Item[i].DropOffset = 0;
    class->Item[i].SwapSide = SWAP_NONE;
    class->Item[i].SwapTimer = 0;
    class->Item[i].SwapOffsetX = 0;
    class->Item[i].SwapOffsetY = 0;
  }
  class->GameStep = STEP_PAUSE;
  class->Animation = FALSE;
  class->CharaColor = col;
  class->Level = 1;
  class->ColorNum = 3;
  class->NextInterval = 150;
  class->NextTimer = 0;
  class->EraseBlock = 0;
  class->EraseScore = 0;
  class->Score = 0;
  class->Combo = 1;
  class->GameOver = FALSE;
  TPuzzleBase_GameLevel(class, class->Level);
  /* --- ƒuƒƒbƒNƒJƒ‰[ */
  for(i=0; i<7; i++) {
    class->BlockColor[i] = BlockColorSet[i];
  }
  /* --- ‰ŠúƒuƒƒbƒN */
  SetBlock(class);
  /* --- ƒJ[ƒ\ƒ‹ˆÊ’u */
  class->UA.X = 4;
  class->UA.Y = 4;
  class->UA.SwapTimer = 0;
  class->UA.SwapOffsetX = 0;
  class->UA.SwapOffsetY = 0;
  class->UA.PopupTimer = 0;
  class->UA.PopupOffset = 0;
}


/* ---------------------------------------- */
/* --- ƒpƒYƒ‹ƒQ[ƒ€ƒf[ƒ^ƒx[ƒXƒƒCƒ“       */
/* ---------------------------------------- */
void TPuzzleBase_GameExec(TPuzzleBase *class)
{
  int  watch;

  switch(class->GameStep) {
    /* --- æ‚Éi‚Ü‚È‚¢’â~ó‘Ô */
  case STEP_PAUSE:
    class->NextTimer = class->NextInterval;
    break;

    /* --- ’Êí‘Ò‚¿ó‚¯ó‘Ô */
  case STEP_NORMAL:
    class->Animation = FALSE;
    class->Combo = 1;
    if (PopupWork(class) == TRUE) {
      class->GameStep = STEP_LINECHECK;
    }
    break;

    /* --- ƒ†[ƒU[‚É‚æ‚éƒuƒƒbƒNˆÚ“®—v‹ */
  case STEP_SWAP:
    PopupWork(class);
    if (MoveWork(class) == FALSE) {
      if (DropRequest(class) ==TRUE) {
	class->GameStep = STEP_DROPWORK;
      }
      else {
	watch = LineCheck(class);
	if (watch > 0) {
	  class->EraseBlock = class->EraseBlock + watch; 
	  class->EraseScore = ((watch - 3) * 10 * class->Combo) * 4;
	  class->Score = class->Score + class->EraseScore;
	  SoundSE(5);
	  class->GameStep = STEP_LINEFLASH;
	}
	else {
	  class->GameStep = STEP_DROPCHECK;
	}
      }
    }
    break;

    /* --- “®‚©‚µ‚½Œã—‰º‚ğ”º‚¤‚© */
  case STEP_DROPCHECK:
    if (DropRequest(class) == TRUE) {
      class->GameStep = STEP_DROPWORK;
    }
    else {
      class->GameStep = STEP_LINECHECK;
    }
    break;

    /* --- —‰º’†ˆ— */
  case STEP_DROPWORK:
    class->Animation = TRUE;
    PopupWork(class);
    if (DropWork(class) == FALSE) {
      class->GameStep = STEP_LINECHECK;
    }
    break;

    /* --- ‘µ‚Á‚½ƒuƒƒbƒN‚ª‚ ‚é‚©‚Ç‚¤‚©‚Ìƒ`ƒFƒbƒN */
  case STEP_LINECHECK:
    watch = LineCheck(class);
    if (watch > 0) {
      class->EraseBlock = class->EraseBlock + watch;
      class->EraseScore = ((watch - 3) * 10 * class->Combo) * 4;
      class->Score = class->Score + class->EraseScore;
      SoundSE(5);
      class->GameStep = STEP_LINEFLASH;
    }
    else {
      class->GameStep = STEP_NORMAL;
    }
    break;

    /* --- ƒ‰ƒCƒ“‘µ‚¢‚ÌƒuƒƒbƒN“_–ÅƒAƒjƒ[ƒVƒ‡ƒ“ */
  case STEP_LINEFLASH:
    class->Animation = TRUE;
    PopupWork(class);
    if (LineWork(class) == FALSE) {
      class->Combo = class->Combo + 1;
      class->GameStep = STEP_DROPCHECK;
    }
    break;

    /* --- I—¹ */
  case STEP_END:
    break;

  default:
    break;
  }

}


/* ---------------------------------------- */
/* --- ƒQ[ƒ€ƒXƒeƒbƒv‚Ìw’è                 */
/* ---------------------------------------- */
void TPuzzleBase_GamePause(TPuzzleBase *class, int  mode)
{
  class->GameStep = mode;
}


/* ---------------------------------------- */
/* --- ƒQ[ƒ€ƒŒƒxƒ‹‚Ìw’è                   */
/* ---------------------------------------- */
void TPuzzleBase_GameLevel(TPuzzleBase *class, int lset)
{
  int  l;

  /* -- ƒŒƒxƒ‹‚É‡‚í‚¹‚Ä“ïˆÕ“xİ’è */
  l = lset - 1;
  if (l < 0) {
    l = 0;
  }
  if (l > 29) {
    l = 29;
  }
  /* •Ï‰»‚·‚é‚Ì‚Í‚±‚Ì“ñ‚Â */
  switch(class->Difficult) {
  case LevelEasy:
    class->ColorNum = LevelColor_easy[l];
    class->NextInterval = LevelSpeed_easy[l] * 2;
    break;

  case LevelNormal:
    class->ColorNum = LevelColor_normal[l];
    class->NextInterval = LevelSpeed_normal[l] * 2;
    break;

  case LevelHard:
    class->ColorNum = LevelColor_hard[l];
    class->NextInterval = LevelSpeed_hard[l] * 2;
    break;
  }
}
  

/* ---------------------------------------- */
/* --- Œ»İ‚ÌƒQ[ƒ€ƒŒƒxƒ‹                   */
/*     ƒŒƒxƒ‹•ÏX‚Å TRUE                    */
/* ---------------------------------------- */
int  TPuzzleBase_LevelCheck(TPuzzleBase *class)
{
  int  i, lv;
  int  ret;

  ret = FALSE;
  lv = 0;
  for(i=0; i<30; i++) {
    if (LevelBlock[i] < class->EraseBlock) {
      lv = i;
    }
  }
  if (lv == class->Level) {
    class->Level = class->Level + 1;
    ret = TRUE;
  }
  return(ret);
}


/* ---------------------------------------- */
/* --- ƒuƒƒbƒN‚Ì“ü‚ê‘Ö‚¦‚ğs‚¤             */
/* ---------------------------------------- */
int  TPuzzleBase_MoveRequest(TPuzzleBase *class,
			     int posx, int posy,
			     int dir)
{
  Block  *master, *target;

  /* --- ƒJ[ƒ\ƒ‹ˆÊ’u‚ÉƒuƒƒbƒN‚ª–³‚¢‚È‚çƒXƒLƒbƒv */
  if (class->Field[posx + (posy * FIELD_WIDTH)] == 0)
    return(FALSE);

  /* --- •Ç•ûŒü‚ÖˆÚ“®‚µ‚æ‚¤‚Æ‚µ‚Ä‚¢‚½‚çƒGƒ‰[ */
  if ((posx == 0) && (dir == MOVE_LEFT)) 
    return(FALSE);

  if ((posx == (FIELD_WIDTH - 1)) && (dir == MOVE_RIGHT))
    return(FALSE);

  if ((posy == 1) && (dir == MOVE_DOWN))
    return(FALSE);

  if ((posy == (FIELD_HEIGHT - 1)) && (dir == MOVE_UP))
    return(FALSE);

  if ((class->Field[posx + ((posy - 1) * FIELD_WIDTH)] == 0) &&
      (dir == MOVE_UP))
    return(FALSE);

  /* ---------------------------------------------- */
  /* --- ˆÚ“®ƒŠƒNƒGƒXƒg  */
  if (dir == MOVE_UP) {
    master = class->Field[posx + (posy * FIELD_WIDTH)];
    target = class->Field[posx + ((posy + 1) * FIELD_WIDTH)];
    if (target != 0) {
      class->Field[posx + ((posy + 1) * FIELD_WIDTH)] = master;
      class->Field[posx + (posy * FIELD_WIDTH)] = target;
      master->SwapSide = SWAP_MASTER;
      master->SwapTimer = 3;
      master->SwapOffsetX = 0;
      master->SwapOffsetY = -BLOCK_HEIGHT;
      master->LineCheck = TRUE;
      if (target != 0) {
        target->SwapSide = SWAP_SLAVE;
        target->SwapTimer = 3;
        target->SwapOffsetX = 0;
        target->SwapOffsetY = -BLOCK_HEIGHT;
        target->LineCheck = TRUE;
      }
      class->UA.SwapTimer = 3;
      class->UA.SwapOffsetX = 0;
      class->UA.SwapOffsetY = BLOCK_HEIGHT;
    }
  }
  if (dir == MOVE_DOWN) {
    master = class->Field[posx + (posy * FIELD_WIDTH)];
    target = class->Field[posx + ((posy - 1) * FIELD_WIDTH)];
    class->Field[posx + ((posy - 1) * FIELD_WIDTH)] = master;
    class->Field[posx + (posy * FIELD_WIDTH)] = target;
    master->SwapSide = SWAP_MASTER;
    master->SwapTimer = 3;
    master->SwapOffsetX = 0;
    master->SwapOffsetY = -BLOCK_HEIGHT;
    master->LineCheck = TRUE;
    if (target != 0) {
      target->SwapSide = SWAP_SLAVE;
      target->SwapTimer = 3;
      target->SwapOffsetX = 0;
      target->SwapOffsetY = BLOCK_HEIGHT;
      target->LineCheck = TRUE;
    }
    class->UA.SwapTimer = 3;
    class->UA.SwapOffsetX = 0;
    class->UA.SwapOffsetY = -BLOCK_HEIGHT;
  }
  if (dir == MOVE_LEFT) {
    master = class->Field[posx + (posy * FIELD_WIDTH)];
    target = class->Field[posx - 1 + (posy * FIELD_WIDTH)];
    class->Field[posx - 1 + (posy * FIELD_WIDTH)] = master;
    class->Field[posx + (posy * FIELD_WIDTH)] = target;
    master->SwapSide = SWAP_MASTER;
    master->SwapTimer = 3;
    master->SwapOffsetX = BLOCK_WIDTH;
    master->SwapOffsetY = 0;
    master->LineCheck = TRUE;
    if (target != 0) {
      target->SwapSide = SWAP_SLAVE;
      target->SwapTimer = 3;
      target->SwapOffsetX = -BLOCK_WIDTH;
      target->SwapOffsetY = 0;
      target->LineCheck = TRUE;
    }
    class->UA.SwapTimer = 3;
    class->UA.SwapOffsetX = BLOCK_WIDTH;
    class->UA.SwapOffsetY = 0;
  }
  if (dir == MOVE_RIGHT) {
    master = class->Field[posx + (posy * FIELD_WIDTH)];
    target = class->Field[posx + 1 + (posy * FIELD_WIDTH)];
    class->Field[posx + 1 + (posy * FIELD_WIDTH)] = master;
    class->Field[posx + (posy * FIELD_WIDTH)] = target;
    master->SwapSide = SWAP_MASTER;
    master->SwapTimer = 3;
    master->SwapOffsetX = -BLOCK_WIDTH;
    master->SwapOffsetY = 0;
    master->LineCheck = TRUE;
    if (target != 0) {
      target->SwapSide = SWAP_SLAVE;
      target->SwapTimer = 3;
      target->SwapOffsetX = BLOCK_WIDTH;
      target->SwapOffsetY = 0;
      target->LineCheck = TRUE;
    }
    class->UA.SwapTimer = 3;
    class->UA.SwapOffsetX = -BLOCK_WIDTH;
    class->UA.SwapOffsetY = 0;
  }

  /* - ˆÚ“®Š®—¹ */
  class->GameStep = STEP_SWAP;

  return(TRUE);
}




/* -------------------------------------------------------------- */
/* --- ƒNƒ‰ƒXƒƒ\ƒbƒh                                             */
/* -------------------------------------------------------------- */


/* ---------------------------------------- */
/* --- ƒuƒƒbƒN‰Šúƒpƒ^[ƒ“                 */
/* ---------------------------------------- */
void SetBlock(TPuzzleBase *class)
{
  int  i;

  /* --- ƒtƒB[ƒ‹ƒh”z—ñƒNƒŠƒA */
  for(i=0; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    class->Field[i] = 0;
  }
  /* --- ƒXƒ^[ƒg‚ÉF‚ª‘µ‚Á‚Ä‚¢‚È‚¢‚æ‚¤‚É */
  /* == æ‚èŠ¸‚¦‚¸Œã‰ñ‚µ (^^; */
  /* --- ‰ŠúƒuƒƒbƒN‚ğ’u‚­ */
  for(i=0; i<(FIELD_WIDTH * 4); i++) {
    class->Field[i] = GetBlock(class);
    class->Field[i]->Color = GetBlockColor(class) + 1;
    class->Field[i]->LineCheck = TRUE;
    class->Field[i]->LineBlock = FALSE;
  }
}


/* ---------------------------------------- */
/* --- ‹ó‚¢‚Ä‚¢‚éƒuƒƒbƒN‚ğ•Ô‚·             */
/* ---------------------------------------- */
Block *GetBlock(TPuzzleBase *class)
{
  Block *b;
  int  i;

  b = 0;
  for(i=0; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    if (class->Item[i].Color == 0) {
      b = &(class->Item[i]);
      break;
    }
  }
  return(b);
}


/* ---------------------------------------- */
/* --- “ïˆÕ“x‚É‘Î‚µ“KØ‚ÈƒuƒƒbƒNƒJƒ‰[‚ğİ’è */
/* ---------------------------------------- */
int  GetBlockColor(TPuzzleBase *class)
{
  int  c;

  /* - ƒuƒƒbƒNƒJƒ‰[‚Íƒ‰ƒ“ƒ_ƒ€ */
  /* === ‚¢‚¸‚ê‚ÍƒAƒCƒeƒ€¬‚º‚Ì’²®‚È‚Ç */
  c = rand() % class->ColorNum;
  /* - ‚¨‚í‚è */
  return(class->BlockColor[c]);
}


/* ---------------------------------------- */
/* --- ƒlƒNƒXƒgƒuƒƒbƒN‚Ì”—‚èã‚ª‚è‚ÆŸ€”õ */
/* ---------------------------------------- */
void  PopupNext(TPuzzleBase *class)
{
  int  i, pos;

  /* --- ƒtƒB[ƒ‹ƒh“àƒuƒƒbƒN‚Ì‚¿ã‚° */
  for(i=0; i<(FIELD_WIDTH * (FIELD_HEIGHT - 1)); i++) {
    pos = FIELD_WIDTH * FIELD_HEIGHT - 1 - i;
    class->Field[pos] = class->Field[pos - FIELD_WIDTH];
    if (class->Field[pos] != 0) {
      class->Field[pos]->PopupTimer = 4;
      class->Field[pos]->PopupOffset = 24;
    }
  }
  /* --- ƒlƒNƒXƒgƒuƒƒbƒN‚Ì€”õ */
  for(i=0; i<FIELD_WIDTH; i++) {
    class->Field[i] = 0;
    class->Field[i] = GetBlock(class);
    class->Field[i]->Color = GetBlockColor(class) + 1;
    class->Field[i]->PopupTimer = 4;
    class->Field[i]->PopupOffset = BLOCK_HEIGHT;
    class->Field[i]->LineCheck = TRUE;
  }
  /* --- ‚Â‚©‚İƒAƒNƒVƒ‡ƒ“‚Ì“r’†‚Å‚ ‚Á‚½‚çƒJ[ƒ\ƒ‹‚àˆÚ“® */
  if (class->UA.HaveBlock == TRUE) {
    class->UA.Y = class->UA.Y + 1;
    class->UA.PopupTimer = 4;
    class->UA.PopupOffset = BLOCK_HEIGHT;
  }
}


/* -------------------------------------------- */
/* --- ƒlƒNƒXƒg‚Ì”—‚èã‚ª‚èƒJƒEƒ“ƒg‚ÆƒIƒtƒZƒbƒg */
/* -------------------------------------------- */
int  PopupWork(TPuzzleBase *class)
{
  int  r, i;

  r = FALSE;
  /* --- ‚¹‚èã‚ª‚èƒIƒtƒZƒbƒg */
  for(i=0; i<ALL_BLOCK; i++) {
    if (class->Item[i].Color != 0) {
      if (class->Item[i].PopupTimer != 0) {
        class->Item[i].PopupTimer = class->Item[i].PopupTimer - 1;
        class->Item[i].PopupOffset = class->Item[i].PopupOffset - 6;
        if ((class->Item[i].PopupOffset < 0) ||
	    (class->Item[i].PopupTimer == 0)) {
          class->Item[i].PopupOffset = 0;
          r = TRUE;
	}
      }
    }
  }
  if (class->UA.PopupTimer > 0) {
    class->UA.PopupTimer = class->UA.PopupTimer - 1;
    class->UA.PopupOffset = class->UA.PopupOffset - 6;
    if ((class->UA.PopupOffset < 0) ||
	(class->UA.PopupTimer == 0)) {
      class->UA.PopupOffset = 0;
    }
  }
  /* --- ƒlƒNƒXƒg”—‚èã‚ª‚è‚Ü‚Å‚Ìtimer */
  if (class->NextTimer > 0) {
    class->NextTimer = class->NextTimer - 1;
  }
  if ((class->NextTimer == 0) && 
      (class->GameStep == STEP_NORMAL)) {
    class->NextTimer = class->NextInterval;
    /* -- ƒQ[ƒ€ƒI[ƒo[”»’è */
    if (FieldHeight(class) == 0) {
      class->GameOver = TRUE;
    }
    else {
      /* -- ƒlƒNƒXƒg”—‚èã‚ª‚è */
      PopupNext(class);
    }
  }
  /* -- SE */
  if (class->NextTimer == 10) {
    SoundSE(8);
  }
  /* --- ‚¹‚è‚ ‚ª‚èˆ—I—¹ */
  return(r);
}


/* -------------------------------------------- */
/* --- ƒtƒB[ƒ‹ƒh“à‚ÌÅ‚ˆÊƒuƒƒbƒN‚ÌˆÊ’u‚ğ•Ô‚· */
/* -------------------------------------------- */
int  FieldHeight(TPuzzleBase *class)
{
  int  i, j, r;
  int  hit;

  r = 0;
  for(i=0; i<FIELD_HEIGHT; i++) {
    hit = FALSE;
    for(j=0; j<FIELD_WIDTH; j++) {
      if (class->Field[(((FIELD_HEIGHT - 1) - i) * FIELD_WIDTH) + j] != 0) {
        hit = TRUE;
      }
    }
    if (hit == TRUE) {
      r = i;
      break;
    }
  }
  /* - –ß‚è’l */
  return(r);
}


/* -------------------------------------------- */
/* --- ƒtƒB[ƒ‹ƒh“à‚ÌÅ‚ˆÊƒuƒƒbƒN‚ÌˆÊ’u‚ğ•Ô‚· */
/* -------------------------------------------- */
int  MoveWork(TPuzzleBase *class)
{
  int  i;
  int  working;

  working = FALSE;
  /* --- ƒtƒB[ƒ‹ƒh“àƒuƒƒbƒN‚ÌˆÚ“® */
  for(i=FIELD_WIDTH; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    if (class->Field[i] != 0) {
      if (class->Field[i]->SwapTimer > 0) {
        class->Field[i]->SwapTimer = class->Field[i]->SwapTimer - 1;
        if (class->Field[i]->SwapTimer == 0) {
          class->Field[i]->SwapSide = SWAP_NONE;
          class->Field[i]->SwapOffsetX = 0;
          class->Field[i]->SwapOffsetY = 0;
	}
        else {
          class->Field[i]->SwapOffsetX = class->Field[i]->SwapOffsetX % 2;
          class->Field[i]->SwapOffsetY = class->Field[i]->SwapOffsetY % 2;
          if (class->Field[i]->SwapSide == SWAP_SLAVE) {
            class->Field[i]->SwapOffsetY = class->Field[i]->SwapOffsetY + SwapTable[class->Field[i]->SwapTimer];
	  }
	  working = TRUE;
	}
      }
    }
  }
  /* --- ƒJ[ƒ\ƒ‹‚ÌˆÚ“® */
  if (class->UA.SwapTimer > 0) {
    class->UA.SwapTimer = class->UA.SwapTimer - 1;
    if (class->UA.SwapTimer == 0) {
      class->UA.SwapOffsetX = 0;
      class->UA.SwapOffsetY = 0;
    }
    else {
      class->UA.SwapOffsetX = class->UA.SwapOffsetX % 2;
      class->UA.SwapOffsetY = class->UA.SwapOffsetY % 2;
    }
  }
  /* - ‚¨‚µ‚Ü‚¢ */
  return(working);
}


/* -------------------------------------------- */
/* --- —‰ºƒuƒƒbƒN‚Ì”»’è‚Æ—‰ºƒŠƒNƒGƒXƒg       */
/* -------------------------------------------- */
int  DropRequest(TPuzzleBase *class)
{
  int i;
  int position, t, working;
  Block *b;

  working = FALSE;
  for(i=FIELD_WIDTH; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    if (class->Field[i] != 0) {
      if (class->Field[i - FIELD_WIDTH] == 0) {
        working = TRUE;
        /* -- ‰º‚É–³‚¢‚Ì‚Å‚Ç‚±‚Ü‚Å—‚¿‚é‚©ƒT[ƒ` */
        position = i - FIELD_WIDTH;
        t = 0;
	do {
          if (class->Field[position] != 0) break;
          position = position - FIELD_WIDTH;
          t = t + 1;
        } while(!(position < FIELD_WIDTH));
        /* -- —‰ºİ’è */
        position = position + FIELD_WIDTH;
	b = class->Field[i];
	b->LineCheck = TRUE;
	b->DropCheck = TRUE;
	b->DropTimer = 2 * t;
	b->DropOffset = 24 * t;
        class->Field[position] = b;
        class->Field[i] = 0;
      }
    }
  }
  return(working);
}


/* -------------------------------------------- */
/* --- ƒuƒƒbƒN‚Ì—‰º                           */
/* -------------------------------------------- */
int  DropWork(TPuzzleBase *class)
{
  int  i, working;

  working = FALSE;
  for(i=FIELD_WIDTH; i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    if (class->Field[i] != 0) {
      if (class->Field[i]->DropTimer > 0) {
        working = TRUE;
        class->Field[i]->DropTimer = class->Field[i]->DropTimer - 1;
        if (class->Field[i]->DropTimer == 0) {
          class->Field[i]->DropOffset = 0;
	}
        else {
          class->Field[i]->DropOffset = class->Field[i]->DropOffset - 12;
	}
      }
    }
  }
  /* - ‚¨‚µ‚Ü‚¢ */
  return(working);
}


/* -------------------------------------------- */
/* --- ƒuƒƒbƒN‘µ‚¢‚Ìƒ`ƒFƒbƒN                   */
/* -------------------------------------------- */
int  LineCheck(TPuzzleBase *class)
{
  int  i, j;
  int  working;

  working = 0;
  for(j=1; j<FIELD_HEIGHT; j++) {
    for(i=0; i<FIELD_WIDTH; i++) {
      if (class->Field[(j * FIELD_WIDTH) + i] != 0) {
        if (class->Field[(j * FIELD_WIDTH) + i]->LineCheck == TRUE) {
          class->Field[(j * FIELD_WIDTH) + i]->LineCheck = FALSE;
          working = working + LineCount(class, i, j, 1, 0, 0);
          working = working + LineCount(class, i, j, 0, 1, 0);
	}
      }
    }
  }
  /* --- ‚¨‚µ‚Ü‚¢ */
  return(working);
}


/* -------------------------------------------- */
/* --- ƒuƒƒbƒN‚ª‘µ‚Á‚ÄÁ‚¦‚éƒGƒtƒFƒNƒg         */
/* -------------------------------------------- */
int  LineWork(TPuzzleBase *class)
{
  int  i, working, sound;

  working = FALSE;
  sound = FALSE;
  for(i=FIELD_WIDTH;i<(FIELD_WIDTH * FIELD_HEIGHT); i++) {
    if (class->Field[i] != 0) {
      if (class->Field[i]->LineTimer > 0) {
        if (class->Field[i]->LineTimer == 10) {
          sound = TRUE;
	}
	working = TRUE;
	class->Field[i]->LineTimer = class->Field[i]->LineTimer - 1;
	if (class->Field[i]->LineTimer == 0) {
	  class->Field[i]->Color = 0;
	  class->Field[i]->LineBlock = FALSE;
	  class->Field[i]->LineCheck = FALSE;
	  class->Field[i] = 0;
	}
      }
    }
  }
  if (sound == TRUE) {
    SoundSE(6);
  }
  /* - ‚¨‚µ‚Ü‚¢ */
  return(working);
}


/* -------------------------------------------- */
/* --- ƒuƒƒbƒN‘µ‚¢‚ÌƒJƒEƒ“ƒg                   */
/* -------------------------------------------- */
int  LineCount(TPuzzleBase *class,
	       int x, int y,
	       int dx, int dy,
	       int layer)
{
  int  col, nextcol;
  int  score, ready;

  score = 0;
  ready = 0;
  col = class->Field[x + (y * FIELD_WIDTH)]->Color;
  if (class->Field[x + (y * FIELD_WIDTH)]->LineBlock == TRUE) {
    ready = ready + 1;
  }
  do {
    if (layer == 2) {
      if (class->Field[x + (y * FIELD_WIDTH)] != 0) {
	class->Field[x + (y * FIELD_WIDTH)]->LineBlock = TRUE;
	class->Field[x + (y * FIELD_WIDTH)]->LineTimer = 15;
      }
    }
    x = x + dx;
    y = y + dy;
    if ((x < 0) ||
	(x >= FIELD_WIDTH) ||
	(y < 1) ||
	(y >= FIELD_HEIGHT)) {
      nextcol = 0;
    }
    else {
      if (class->Field[x + (y * FIELD_WIDTH)] != 0) {
        nextcol = class->Field[x + (y * FIELD_WIDTH)]->Color;
      }
      else {
        nextcol = 0;
      }
    }
    score = score + 1;
    if (col == nextcol) {
      if (class->Field[x + (y * FIELD_WIDTH)] != 0) {
        if (class->Field[x + (y * FIELD_WIDTH)]->LineBlock == TRUE) {
          ready = ready + 1;
	}
      }
    }
  } while(!((x < 0) ||
	  (x >= FIELD_WIDTH) ||
	  (y < 1) ||
	  (y >= FIELD_HEIGHT) ||
	  (col != nextcol)));
  x = x - dx;
  y = y - dy;

  /* --- ŸƒŒƒCƒ„‚Ö */
  switch(layer) {
  case 0:
    score = LineCount(class, x, y, -dx, -dy, 1);
    break;
    
  case 1:
    if ((score >= LINE_LENGTH) && (score > ready)) {
      score = LineCount(class, x, y, -dx, -dy, 2);
    }
    else {
      score = 0;
    }
    break;
    
  case 2:
    score = score - ready;
    break;

  }

  return(score);
}

