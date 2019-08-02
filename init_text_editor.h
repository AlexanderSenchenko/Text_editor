#ifndef INIT_TEXT_EDITOR_H
#define INIT_TEXT_EDITOR_H

void sig_winch(int);

void init_text_editor();
static int read_file(int, WINDOW*, WINDOW*);
static int open_file(WINDOW*, WINDOW*);
static int init_win_text(WINDOW**, WINDOW**);
static int init_win_menu(WINDOW**, WINDOW**);

#endif
