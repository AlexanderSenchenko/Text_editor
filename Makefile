NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo

all:
	gcc init_text_editor.c -c -g $(NCURSES_CFLAGS)
	gcc main.c -c -g $(NCURSES_CFLAGS)
	gcc main.o init_text_editor.o -o main $(NCURSES_LIBS)
