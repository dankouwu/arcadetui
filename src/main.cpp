#include <iostream>
#include <ncurses.h>
#include <vector>
#include <wchar.h>

#include "tic_tac_toe.hpp"

#define BLACK 1
#define RED 2
#define GREEN 3
#define YELLOW 4
#define BLUE 5
#define MAGENTA 6
#define CYAN 7
#define WHITE 8

int termHeight, termWidth;

const std::vector<std::string> title = {
    "                                                ",
    " █████╗ ██████╗  ██████╗ █████╗ ██████╗ ███████╗",
    "██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝",
    "███████║██████╔╝██║     ███████║██║  ██║█████╗  ",
    "██╔══██║██╔══██╗██║     ██╔══██║██║  ██║██╔══╝  ",
    "██║  ██║██║  ██║╚██████╗██║  ██║██████╔╝███████╗",
    "╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═════╝ ╚══════╝",
    "                                                ",
    "            ████████╗██╗   ██╗██╗               ",
    "            ╚══██╔══╝██║   ██║██║               ",
    "               ██║   ██║   ██║██║               ",
    "               ██║   ██║   ██║██║               ",
    "               ██║   ╚██████╔╝██║               ",
    "               ╚═╝    ╚═════╝ ╚═╝               ",
};

void printTitle(const std::vector<std::string>& lines, int startY, int termWidth) {
    setlocale(LC_ALL, "");

    for (int i = 0; i < lines.size(); i++) {
        size_t bufSize = lines[i].length() + 1;
        wchar_t* wstr = new wchar_t[bufSize];
        mbstowcs(wstr, lines[i].c_str(), bufSize);
        int displayWidth = wcswidth(wstr, bufSize);
        if (displayWidth < 0) displayWidth = lines[i].length();
        int startX = (termWidth - displayWidth) / 2;
        if (startX < 0) startX = 0;
        mvprintw(startY + i, startX, "%s", lines[i].c_str());
        delete[] wstr;
    }
    refresh();
}

void drawTitleScreen() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    if (has_colors()) {
        start_color();
    }

    init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);

    int choice = 0;
    std::string options[] = {"Play Tic Tac Toe", "Exit"};
    int numOptions = sizeof(options) / sizeof(options[0]);

    while (true) {
        clear();
        getmaxyx(stdscr, termHeight, termWidth);
        int titleHeight = title.size();
        int menuHeight = numOptions;
        int guidanceHeight = 1;
        int spacing = 2;
        int totalContentHeight = titleHeight + spacing + menuHeight + spacing + guidanceHeight;
        int startY = (termHeight - totalContentHeight) / 2;
        if (startY < 0) startY = 0;
        printTitle(title, startY, termWidth);
        int menuY = startY + titleHeight + spacing;

        for (int i = 0; i < numOptions; i++) {
            if (i == choice)
                attron(A_REVERSE);
            int optionX = (termWidth - options[i].size()) / 2;
            mvprintw(menuY + i, optionX, "%s", options[i].c_str());
            attroff(A_REVERSE);
        }

        std::string guide = "Use arrow keys to navigate, Enter to select.";
        mvprintw(termHeight-2, (termWidth - guide.size()) / 2, "%s", guide.c_str());
        refresh();

        int key = getch();
        if (key == KEY_UP) {
            choice = (choice - 1 + numOptions) % numOptions;
        } else if (key == KEY_DOWN) {
            choice = (choice + 1) % numOptions;
        } else if (key == KEY_RESIZE) {
            getmaxyx(stdscr, termHeight, termWidth);
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
    drawTitleScreen();
    return 0;
}
