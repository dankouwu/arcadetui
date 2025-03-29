#include "tic_tac_toe.hpp"
#include <ncurses.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
int cursorX = 0, cursorY = 0;
bool isXTurn = true;

void drawBoard() {
    clear();
    mvprintw(1, 5, "TIC TAC TOE");

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cursorX == j && cursorY == i) {
                attron(A_REVERSE);
            }
            mvprintw(i * 2 + 3, j * 4 + 5, "[%c]", board[i][j]);
            if (cursorX == j && cursorY == i) {
                attroff(A_REVERSE);
            }
        }
    }
    mvprintw(10, 5, "Player %c's turn", isXTurn ? 'X' : 'O');
    refresh();
}

bool checkWin(char player) {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

void playTicTacToe() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    while (true) {
        drawBoard();
        int ch = getch();

        if (ch == KEY_UP && cursorY > 0) cursorY--;
        else if (ch == KEY_DOWN && cursorY < 2) cursorY++;
        else if (ch == KEY_LEFT && cursorX > 0) cursorX--;
        else if (ch == KEY_RIGHT && cursorX < 2) cursorX++;
        else if (ch == '\n' || ch == ' ') {
            if (board[cursorY][cursorX] == ' ') {
                board[cursorY][cursorX] = isXTurn ? 'X' : 'O';
                if (checkWin(board[cursorY][cursorX])) {
                    drawBoard();
                    mvprintw(12, 5, "Player %c Wins!", board[cursorY][cursorX]);
                    refresh();
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    break;
                }
                isXTurn = !isXTurn;
            }
        }
    }
    endwin();
}
