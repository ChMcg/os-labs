#ifndef _GETCH_CPP
#define _GETCH_CPP

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#undef getch

char getch(bool nonblock = false)
{
	char sym = 0;
	int old_flags = 0;

	
	termios old_t, new_t;
	tcgetattr( STDIN_FILENO, &old_t );
	old_flags = fcntl( STDIN_FILENO, F_GETFL, 0 );

	if(nonblock)
		fcntl( STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);

	new_t = old_t;
	new_t.c_lflag &= ~( ICANON | ECHO);
	tcsetattr( STDIN_FILENO, TCSANOW, &new_t);
	sym = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &old_t);

	if(nonblock)
		fcntl( STDIN_FILENO, F_SETFL, old_flags);
	
	return sym;
}


#endif // _GETCH_CPP