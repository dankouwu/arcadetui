#include <iostream>
#include <ncurses.h>
#include "tic_tac_toe.hpp"

void showTitleScreen() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    int choice = 0;
    std::string options[] = {"Play Tic Tac Toe", "Exit"};
    int numOptions = sizeof(options) / sizeof(options[0]);

    while (true) {
        clear();
        mvprintw(2, 5, "Welcome to TUI Arcade!");
        mvprintw(4, 5, "Use arrow keys to navigate, Enter to select.");

        for (int i = 0; i < numOptions; i++) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(6 + i, 5, options[i].c_str());
            attroff(A_REVERSE);
        }

        int key = getch();
        if (key == KEY_UP) {
            choice = (choice - 1 + numOptions) % numOptions;
        } else if (key == KEY_DOWN) {
            choice = (choice + 1) % numOptions;
        } else if (key == '\n' || key == '\r') {
            if (choice == 0) {
                clear();
                playTicTacToe();
            } else {
                break;
            }
        }
    }
    endwin();
}

int main() {
    showTitleScreen();
    return 0;
}
