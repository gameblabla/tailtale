--- Action puzzle game "Tail-Tale" for GP2X
- Release by fumi2kick - Jan.13.2006

Source release

* Update

  - Jan.13.2006
    add: Volume control enabled
    change: BGM file convert to OGG
    add: press LR assign to drug
    add: Exit game when 'L + R + JoyClick' pressed
    add: stick <-> button swap mode
    add: press SELECT in title, jump to Key assign config


* build
  for GP2X
    % make gp

  for PC
    % make

  require Libs.
    SDL
    SDL_image
    SDL_mixer
    zlib
    libpng

* source code

  system
    bootmain.*
    debug.*
    input.*
    sound.*
    grp_screen.*
    grp_texture.*
    grp_sprite.*
    psp_debug.c
    psp_grp_screen.c

  game
    gamemain.*
    puz_trial.*
    puz_disp.*
    puz_base.*

* to play

data folder files put on same directory with binary.

* credit

  released by fumi2kick
  programing and other : rerofumi <rero2@yuumu.org>
  original graphic : pyonkey-matsuo 
  distribution : http://www.fumi2kick.com/
