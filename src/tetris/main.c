// main.c

#include <stdio.h>
#include <ncurses.h>

int main()
{
    initscr();

    WINDOW *window = newwin(10, 30, 0, 0);

    while (1) {
        mvwprintw(window, 1, 1, "Hello world!");

        wrefresh(window);
    }

    int ch = getch();

    endwin();
    return 0;
}