#ifdef NSPIRE
#define SDL_ACCELERATION_RLE 0
#else
#define SDL_ACCELERATION_RLE SDL_RLEACCEL
#endif

#ifdef NSPIRE
#define START_BUTTON SDLK_RETURN
#define UP_BUTTON SDLK_UP
#define LEFT_BUTTON SDLK_LEFT
#define RIGHT_BUTTON SDLK_RIGHT
#define DOWN_BUTTON SDLK_DOWN
#define A_BUTTON SDLK_LCTRL
#define B_BUTTON SDLK_LSHIFT
#define C_BUTTON SDLK_BACKSPACE
#else
#define START_BUTTON SDLK_SPACE
#define UP_BUTTON SDLK_UP
#define LEFT_BUTTON SDLK_LEFT
#define RIGHT_BUTTON SDLK_RIGHT
#define DOWN_BUTTON SDLK_DOWN
#define A_BUTTON SDLK_x
#define B_BUTTON SDLK_z
#define C_BUTTON SDLK_c
#endif


#if defined(NOTPSP)

#if !defined(NSPIRE) 
	#undef SLEEP_UNIX
#elif !defined(KOLIBRI) 
	#undef SLEEP_UNIX
#elif !defined(WIN32)
	#undef SLEEP_UNIX
#else
	#define SLEEP_UNIX
#endif

#endif


