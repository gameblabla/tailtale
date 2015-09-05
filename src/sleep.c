void msleep(unsigned char milisec)
{
	#ifdef UNIX
		struct timespec req={0};
		time_t sec=(unsigned short)(milisec/1000);

		milisec=milisec-(sec*1000);
		req.tv_sec=sec;
		req.tv_nsec=milisec*1000000L;

		while(nanosleep(&req,&req)==-1)
		continue;
	#else
		SDL_Delay(milisec);
	#endif
}
