#include <curses.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <math.h>

void	clear_buff(wchar_t *b, int length)
{
	int	i;

	i = 0;
	while (i < length)
		b[i++] = ' ';
}

#define MOVE_SZ	0.1
#define DIR_SZ	0.5
#define FOV		M_PI_2

int	main(void)
{
	struct winsize	sz;
	wchar_t			*buff;
	char			*map[10];
	float			x;
	float			y;
	float			dir;
	int				c;
	float			t;
	float			tx;
	float			ty;

	if (ioctl(0, TIOCGWINSZ, (char *) &sz) < 0)
		printw("TIOCGWINSZ error");
	if (!(buff = malloc(sz.ws_row * sz.ws_col * sizeof(*buff))))
		return (-1);
	map[0] = "##########";
	map[1] = "#        #";
	map[2] = "#        #";
	map[3] = "#        #";
	map[4] = "#        #";
	map[5] = "#        #";
	map[6] = "#        #";
	map[7] = "#        #";
	map[8] = "#        #";
	map[9] = "##########";
	setlocale(LC_ALL, "");
	initscr();
	timeout(0);
	noecho();
	x = 1;
	y = 1;
	dir = 0;
	while ((c = getch()) != 27 || (c = getch()) != -1)
	{
		clear_buff(buff, sz.ws_col * sz.ws_row);
		if (c == 'z')
			y -= y > 0 ? MOVE_SZ : 0;
		if (c == 's')
			y += y < 10 ? MOVE_SZ : 0;
		if (c == 'q')
			x -= x > 0 ? MOVE_SZ : 0;
		if (c == 'd')
			x += x < 10 ? MOVE_SZ : 0;
		if (c == 'a')
			dir -= DIR_SZ;
		if (c == 'e')
			dir += DIR_SZ;
		if (dir < 0)
			dir += 2 * M_PI;
		if (dir > 2 * M_PI)
			dir -= 2 * M_PI;
		mvprintw(0, 0, "%S", buff);
		c = -1;
		while (++c < 10)
			mvprintw(c, 0, "%s", map[c]);
		mvprintw((int)y, (int)x, "X");
		tx = cos(dir);
		ty = sin(dir);
		t = 0;
		while ((t += 0.1) < 10)
			mvprintw(y + t * ty, x + t * tx, "%d", (int)t);
		refresh();
	}
	endwin();
	return (0);
}
