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

wchar_t	getch_shade(float dist)
{
	if (dist < 1)
		return (L'@');
	if (dist < 2)
		return (L'#');
	if (dist < 3)
		return (L'O');
	if (dist < 4)
		return (L'o');
	if (dist < 5)
		return (L':');
	if (dist < 6)
		return (L'"');
	if (dist < 7)
		return (L',');
	if (dist < 8)
		return (L'.');
	if (dist < 9)
		return (L'.');
	return (L' ') ;
}

float	map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void	fill_col(wchar_t *buff, int row, int col, float dist, int n)
{
	int		i;
	float	height;

	i = -1;
	while (++i < row)
	{
		height = map(dist, 0, 10, row / 1.8, row / 8);
		if (i > (row - height) / 2 && i < row - (row - height) / 2)
			buff[i * col + n] = getch_shade(dist);
		else
			buff[i * col + n] = ' ';
	}
}

float	get_ray(float dir, int n, int steps)
{
	return (dir + (FOV * n / steps) - (FOV / 2.0));
}


void	get_dir(float dir, float *tx, float *ty)
{
	*tx = cos(dir);
	*ty = sin(dir);
}

void	move_player(float dir, float *posx, float *posy, float length)
{
	float	tx;
	float	ty;

	get_dir(dir, &tx, &ty);
	*posx += length * tx;
	*posy += length * ty;
}

int		main(void)
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
	map[5] = "#   ######";
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
		if (c == 'z')
			move_player(dir, &x, &y, MOVE_SZ);
		if (c == 's')
			move_player(dir, &x, &y, -MOVE_SZ);
		if (c == 'q')
			move_player(dir + M_PI_2, &x, &y, -MOVE_SZ);
		if (c == 'd')
			move_player(dir - M_PI_2, &x, &y, -MOVE_SZ);
		if (c == 'a')
			dir -= DIR_SZ;
		if (c == 'e')
			dir += DIR_SZ;
		if (dir < 0)
			dir += 2 * M_PI;
		if (dir > 2 * M_PI)
			dir -= 2 * M_PI;
		c = -1;
		while (++c < sz.ws_col)
		{
			get_dir(get_ray(dir, c, sz.ws_col), &tx, &ty);
			t = 0;
			while ((t += 0.1) < 10)
				if (y + t * ty >= 0 && y + t * ty < 10
						&& x + t * tx >= 0 && x + t * tx < 10
						&& map[(int)(y + t * ty)][(int)(x + t * tx)] == '#')
				{
					fill_col(buff, sz.ws_row, sz.ws_col, t, c);
					break;
				}
		}
		mvprintw(0, 0, "%S", buff);
		c = -1;
		while (++c < 10)
			mvprintw(c, 0, "%s", map[c]);
		mvprintw((int)y, (int)x, "X");
		refresh();
	}
	endwin();
	return (0);
}
