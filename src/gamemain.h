/* ---------------------------------------------------------- */
/*  gamemain.h                                                */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   gamemain.h                                           */
/*     ゲームの本体フロー                                 */
/*                                                        */
/*--------------------------------------------------------*/

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <SDL/SDL.h>
#include "grp_screen.h"
#include "puz_trial.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

typedef struct {
  int  titletimer;
  SDL_Surface  *tex;
  TGameSprite  *bg, *logo, *push, *release, *select_level;
  TGameScreen  *screen;
  int  sound_volume;
  int  control_reverse;
  int  control_anime;
  int  control_anime_y;
  int  level;
  int  step;
  TPuzzleTrial  *puzzle;
} TGameMain, *PTGameMain;

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

TGameMain *TGameMain_Create(TGameScreen *mainscreen);
void TGameMain_Destroy(TGameMain *class);

int TGameMain_Poll(TGameMain *class,
		   int  counter);


#endif //GAMEMAIN_H
