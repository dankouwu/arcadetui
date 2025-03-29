#include "tic_tac_toe.hpp"
#include <ncurses.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

// Game state
char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
int cursorX = 0, cursorY = 0;
bool isXTurn = true;
bool gameOver = false;
char winner = ' ';
bool isBoardFull = false;

// Color pairs
#define PAIR_TITLE 1
#define PAIR_GRID 2
#define PAIR_X 3
#define PAIR_O 4
#define PAIR_CURSOR 5
#define PAIR_STATUS 6
#define PAIR_WIN_MSG 7
#define PAIR_SEPARATOR 8

void initColors() {
    start_color();
    init_pair(PAIR_TITLE, COLOR_CYAN, COLOR_BLACK);
    init_pair(PAIR_GRID, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_X, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_O, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_CURSOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(PAIR_STATUS, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_WIN_MSG, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(PAIR_SEPARATOR, COLOR_BLUE, COLOR_BLACK);
}

void drawTitle() {
    attron(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
    mvprintw(1, 10, "+--------------------+");
    mvprintw(2, 10, "|     TIC TAC TOE    |");
    mvprintw(3, 10, "+--------------------+");
    attroff(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
}

void drawSeparators() {
    attron(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);

    // Vertical separators
    for (int i = 0; i < 5; i++) {
        mvprintw(6 + i, 18, "|");
        mvprintw(6 + i, 25, "|");
    }

    // Horizontal separators
    mvprintw(8, 12, "-----------");
    mvprintw(10, 12, "-----------");

    attroff(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);
}

void drawCell(int y, int x, char value) {
    int screenY = y * 2 + 6;
    int screenX = x * 7 + 13;

    // Draw cell
    attron(COLOR_PAIR(PAIR_GRID));
    mvprintw(screenY, screenX, "[   ]");
    attroff(COLOR_PAIR(PAIR_GRID));

    // Highlight the cursor position
    if (cursorX == x && cursorY == y && !gameOver) {
        attron(COLOR_PAIR(PAIR_CURSOR));
        mvprintw(screenY, screenX, "[   ]");
        attroff(COLOR_PAIR(PAIR_CURSOR));
    }

    // Draw X or O
    if (value == 'X') {
        attron(COLOR_PAIR(PAIR_X) | A_BOLD);
        mvprintw(screenY, screenX + 2, "X");
        attroff(COLOR_PAIR(PAIR_X) | A_BOLD);
    } else if (value == 'O') {
        attron(COLOR_PAIR(PAIR_O) | A_BOLD);
        mvprintw(screenY, screenX + 2, "O");
        attroff(COLOR_PAIR(PAIR_O) | A_BOLD);
    }
}

void drawBoard() {
    clear();

    // Draw title
    drawTitle();

    // Draw separators first
    drawSeparators();

    // Draw the board with cells
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            drawCell(i, j, board[i][j]);
        }
    }

    // Draw status message
    attron(COLOR_PAIR(PAIR_STATUS) | A_BOLD);
    if (!gameOver) {
        mvprintw(13, 10, "Player %c's turn", isXTurn ? 'X' : 'O');
        if (isXTurn) {
            attron(COLOR_PAIR(PAIR_X));
            mvprintw(13, 17, "X");
            attroff(COLOR_PAIR(PAIR_X));
        } else {
            attron(COLOR_PAIR(PAIR_O));
            mvprintw(13, 17, "O");
            attroff(COLOR_PAIR(PAIR_O));
        }
    } else {
        if (winner != ' ') {
            attron(COLOR_PAIR(PAIR_WIN_MSG) | A_BOLD);
            mvprintw(13, 10, "Player %c Wins!", winner);
            attroff(COLOR_PAIR(PAIR_WIN_MSG) | A_BOLD);
        } else {
            attron(COLOR_PAIR(PAIR_WIN_MSG) | A_BOLD);
            mvprintw(13, 10, "Game ended in a draw!");
            attroff(COLOR_PAIR(PAIR_WIN_MSG) | A_BOLD);
        }
    }
    attroff(COLOR_PAIR(PAIR_STATUS) | A_BOLD);

    // Controls help
    mvprintw(15, 8, "Controls: Arrow Keys to move, Enter to place");
    mvprintw(16, 8, "          Q to quit, R to restart game");

    refresh();
}

bool checkWin(char player) {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }

    // Check diagonals
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

bool isBoardFullCheck() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

void resetGame() {
    // Reset board
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }

    // Reset game state
    cursorX = 0;
    cursorY = 0;
    isXTurn = true;
    gameOver = false;
    winner = ' ';
    isBoardFull = false;
}

void playTicTacToe() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Setup colors if terminal supports them
    if (has_colors()) {
        initColors();
    }

    // Game loop
    bool running = true;
    while (running) {
        drawBoard();
        int ch = getch();

        if (ch == 'q' || ch == 'Q') {
            running = false;
        } else if (ch == 'r' || ch == 'R') {
            resetGame();
        } else if (!gameOver) {
            switch (ch) {
                case KEY_UP:
                    if (cursorY > 0) cursorY--;
                    break;
                case KEY_DOWN:
                    if (cursorY < 2) cursorY++;
                    break;
                case KEY_LEFT:
                    if (cursorX > 0) cursorX--;
                    break;
                case KEY_RIGHT:
                    if (cursorX < 2) cursorX++;
                    break;
                case '\n':
                case ' ':
                    if (board[cursorY][cursorX] == ' ') {
                        board[cursorY][cursorX] = isXTurn ? 'X' : 'O';

                        if (checkWin(board[cursorY][cursorX])) {
                            gameOver = true;
                            winner = board[cursorY][cursorX];
                        } else if (isBoardFullCheck()) {
                            gameOver = true;
                            isBoardFull = true;
                        } else {
                            isXTurn = !isXTurn;
                        }
                    }
                    break;
            }
        }
    }

    // Clean up and exit
    endwin();
}