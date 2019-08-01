#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include "init_text_editor.h"

#define ROW_MENU 4

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

void init_text_editor()
{
	WINDOW* win_menu;
	WINDOW* sub_win_menu;

	WINDOW* win_text;
	WINDOW* sub_win_text;

	int act;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	curs_set(0);
	refresh();

	if (init_win_text(&win_text, &sub_win_text) == -1) {
		move(1, 1);
		printw("Win text not open");
	}

	if (init_win_menu(&win_menu, &sub_win_menu) == -1) {
		move(2, 1);
		printw("Win menu not open");
	}
	
	refresh();

	// getch();

	keypad(sub_win_menu, TRUE);

	do {
		act = wgetch(sub_win_menu);

		if (act == KEY_F(1)) {
			wmove(sub_win_menu, 1, 0);
			wprintw(sub_win_menu, "Open file");
			
			refresh();
		}
	} while (act != KEY_F(4));

	delwin(sub_win_menu);
	delwin(win_menu);

	delwin(sub_win_text);
	delwin(win_text);

	endwin();
}

static int init_win_text(WINDOW** win_text, WINDOW** sub_win_text)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);

	int row_text = size.ws_row - ROW_MENU;
	int col_text = size.ws_col;

	*win_text = newwin(row_text, col_text, 0, 0);
	if (*win_text == NULL)
		return -1;

	box(*win_text, '|', '-');

	*sub_win_text = derwin(*win_text, row_text - 2, col_text - 2, 1, 1);

	wmove(*sub_win_text, 0, 0);
	wprintw(*sub_win_text, "Text field");

	wrefresh(*win_text);

	return 0;
}


static int init_win_menu(WINDOW** win_menu, WINDOW** sub_win_menu)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);

	int col_menu = size.ws_col;

	*win_menu = newwin(ROW_MENU, col_menu, size.ws_row - ROW_MENU, 0);
	if (*win_menu == NULL)
		return -1;

	box(*win_menu, '|', '-');

	*sub_win_menu = derwin(*win_menu, ROW_MENU - 2, col_menu - 2, 1, 1);

	wmove(*sub_win_menu, 0, 0);
	wprintw(*sub_win_menu, "F1 - open | F2 - save | F3 - close | F10 - exit");

	wrefresh(*win_menu);

	return 0;
}

