#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include "init_text_editor.h"

#define ROW_MENU 3

#define SIZE_FILE_NAME 25

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
	int fd;
	int i;
	int f_file_open = 0;
	int x, y;
	
	off_t pos;

	ssize_t size_write;

	chtype ch;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	noecho();
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

	keypad(sub_win_text, TRUE);

	do {	
		act = wgetch(sub_win_text);

		if ((f_file_open == 0) && (act == KEY_F(1))) {
			fd = open_file(win_text, sub_win_text);
			if (fd != -1) {
				wclear(sub_win_text);
				wrefresh(sub_win_text);

				f_file_open = 1;

				read_file(fd, win_text, sub_win_text);
			}
		} else if (f_file_open == 1) {
			getyx(sub_win_text, y, x);

			switch (act) {
				case KEY_F(2):
					pos = lseek(fd, 0, SEEK_SET);
					if (pos == (off_t) -1) {
						break;
					}

					int max_x, max_y;
					int curr_x = x;
					int curr_y = y;

					getmaxyx(sub_win_text, max_y, max_x);

					for (y = 0; y < max_y; ++y) {
						for (x = 0; x < max_x; ++x) {
							ch = mvwinch(sub_win_text, y, x);

							size_write = write(fd, &ch, 1);
							if (size_write == -1) {
								break;
							} else if (size_write != 1) {
								break;
							}
						}
					}

					wmove(sub_win_text, curr_y, curr_x);

					break;
				case KEY_F(3):
					close(fd);

					noecho();
					curs_set(FALSE);
					wclear(sub_win_text);
					wrefresh(sub_win_text);

					f_file_open = 0;
					break;
				case KEY_UP:
					wmove(sub_win_text, y - 1, x);
					break;
				case KEY_DOWN:
					wmove(sub_win_text, y + 1, x);
					break;
				case KEY_RIGHT:
					wmove(sub_win_text, y, x + 1);
					break;
				case KEY_LEFT:
					wmove(sub_win_text, y, x - 1);
					break;
				case KEY_BACKSPACE:
					wprintw(sub_win_text, " ");
					wmove(sub_win_text, y, x);
					wrefresh(sub_win_text);
					break;
				default:
					break;
			}
		}

		wrefresh(sub_win_text);
	} while (act != KEY_F(4));

	delwin(sub_win_menu);
	delwin(win_menu);

	delwin(sub_win_text);
	delwin(win_text);

	endwin();
}

static int read_file(int fd, WINDOW* win_text, WINDOW* sub_win_text)
{
	ssize_t size;
	char* buff;
	int size_buff;
	if ((buff = malloc(sizeof(char) * size_buff)) == NULL) {
		return -1;
	}

	if ((size = read(fd, buff, size_buff)) == -1) {
		return -1;
	}

	wmove(sub_win_text, 0, 0);
	wprintw(sub_win_text, "%s", buff);

	wmove(sub_win_text, 0, 0);

	free(buff);
	return 0;
}

static int open_file(WINDOW* win_text, WINDOW* sub_win_text)
{
	int fd;
	int i;
	char file_name[SIZE_FILE_NAME + 1];

	echo();
	curs_set(TRUE);
	wrefresh(sub_win_text);

	wmove(sub_win_text, 0, 0);
	wprintw(sub_win_text, "Input file name: ");
	wgetnstr(sub_win_text, file_name, SIZE_FILE_NAME);

	fd = open(file_name, O_RDWR);
	if (fd == -1) {
		noecho();
		curs_set(FALSE);
		wmove(sub_win_text, 1, 0);
		wprintw(sub_win_text, "Error: file not found\n");
		wprintw(sub_win_text, "Press any key to continue...");
		wrefresh(sub_win_text);
		
		// getch();

		wclear(sub_win_text);
		wrefresh(sub_win_text);
		return -1;
	}
	
	return fd;
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

	// wmove(*sub_win_text, 0, 0);
	// wprintw(*sub_win_text, "Text field");

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
	wprintw(*sub_win_menu, "F1 - open | F2 - save | F3 - close | F4 - exit\n");

	wrefresh(*win_menu);

	return 0;
}

