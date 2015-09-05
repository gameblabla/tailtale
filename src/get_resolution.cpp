#include <SDL/SDL.h>
extern "C" {
#include "get_resolution.h"
}

#ifdef SYLLABLE
#include <gui/desktop.h>
#endif

extern struct scaling screen_scale;

void Get_Resolution(void)
{
#ifdef SDLGFX_SCALING
	#ifdef SYLLABLE

	os::Desktop cDesktop;
	os::IPoint point = cDesktop.GetResolution();

	//to use a certain value
	screen_scale.w_display = point.x;  //the width of the screen
	screen_scale.h_display = point.y; //the height of the screen
	#else
	const SDL_VideoInfo* info = SDL_GetVideoInfo(); //<-- calls SDL_GetVideoInfo();

	screen_scale.w_display = info->current_w;
	screen_scale.h_display = info->current_h; 
	#endif
#endif
}
