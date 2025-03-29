#include <iostream>
#include <ncurses.h>
#include <vector>

#include "tic_tac_toe.hpp"

// Title screen ASCII art
const std::vector<std::string> title = {
    " █████╗ ██████╗  ██████╗ █████╗ ██████╗ ███████╗████████╗██╗   ██╗██╗",
    "██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║   ██║██║",
    "███████║██████╔╝██║     ███████║██║  ██║█████╗     ██║   ██║   ██║██║",
    "██╔══██║██╔══██╗██║     ██╔══██║██║  ██║██╔══╝     ██║   ██║   ██║██║",
    "██║  ██║██║  ██║╚██████╗██║  ██║██████╔╝███████╗   ██║   ╚██████╔╝██║",
    "╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═════╝ ╚══════╝   ╚═╝    ╚═════╝ ╚═╝"
};

void drawTitleScreen() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    int choice = 0;
    std::string options[] = {"Play Tic Tac Toe", "Exit"};
    int numOptions = sizeof(options) / sizeof(options[0]);

    // Game loop for title screen
    while (true) {
        clear();

        // Display the title (ASCII art)
        for (int i = 0; i < title.size(); i++) {
            mvprintw(i, 5, title[i].c_str());
        }

        // Instructions
        mvprintw(title.size() + 2, 5, "Use arrow keys to navigate, Enter to select.");

        // Display menu options
        for (int i = 0; i < numOptions; i++) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(title.size() + 4 + i, 5, options[i].c_str());
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
                playTicTacToe();  // Transition to Tic Tac Toe game
            } else {
                break;  // Exit the program
            }
        }
    }

    endwin();  // End ncurses mode
}

int main() {
    drawTitleScreen();  // Call the title screen function
    return 0;
}
