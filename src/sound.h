/* ---------------------------------------------------------- */
/*  sound.h                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   sound.h                                              */
/*     音声マネージャ                                     */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id:  $ */


/*------------------------------------------------------------- */
/** @file
    @brief		音声マネージャ
    @author		K.Kunikane (rerofumi)
    @since		Sep.04.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/


#ifndef SOUND_H
#define SOUND_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#define BGMMAX  32
#define SEMAX   64

/* --- VOUME の最大値 */
#ifdef __GP2X__
#define VOLUMEDEFAULT  96
#else
#define VOLUMEDEFAULT  MIX_MAX_VOLUME
#endif

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

void  SoundInit(void);
void  SoundFree(void);
void  SoundMusic(int req);
void  SoundMusicOneshot(int req);
void  SoundSE(int req);
void  SoundMusicStop(void);
void  SoundSEStop(void);
void  SoundVolume(int value);

#endif //SOUND_H

