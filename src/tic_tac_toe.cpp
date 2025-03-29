#include "tic_tac_toe.hpp"
#include <ncurses.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <utils.hpp>

// Game state
char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
int cursorX = 0, cursorY = 0;
bool isXTurn = true;
bool gameOver = false;
char winner = ' ';
bool isBoardFull = false;

// Screen dimensions
int screenWidth, screenHeight;

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

// Utility function to center text
void printCentered(int y, const char* text, int colorPair, bool bold = false) {
    int x = (screenWidth - strlen(text)) / 2;
    if (bold) attron(COLOR_PAIR(colorPair) | A_BOLD);
    else attron(COLOR_PAIR(colorPair));

    mvprintw(y, x, "%s", text);

    if (bold) attroff(COLOR_PAIR(colorPair) | A_BOLD);
    else attroff(COLOR_PAIR(colorPair));
}

void drawTitle() {
    const char* line1 = "+--------------------+";
    const char* line2 = "|     TIC TAC TOE    |";
    const char* line3 = "+--------------------+";

    printCentered(1, line1, PAIR_TITLE, true);
    printCentered(2, line2, PAIR_TITLE, true);
    printCentered(3, line3, PAIR_TITLE, true);
}

void drawSeparators() {
    attron(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);

    // Calculate center position for the grid
    int gridWidth = 11; // Width of the grid area
    int startX = (screenWidth - gridWidth) / 2;

    // Vertical separators
    for (int i = 0; i < 5; i++) {
        mvprintw(6 + i, startX + 6, "|");
        mvprintw(6 + i, startX + 13, "|");
    }

    // Horizontal separators
    mvprintw(8, startX, "-----------");
    mvprintw(10, startX, "-----------");

    attroff(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);
}

void drawCell(int y, int x, char value) {
    int gridWidth = 21; // Total width of the grid area including separators
    int startX = (screenWidth - gridWidth) / 2;

    int screenY = y * 2 + 6;
    int screenX = startX + x * 7 + 1;

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

    // Get current screen dimensions
    getmaxyx(stdscr, screenHeight, screenWidth);

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
    char statusMsg[50];
    if (!gameOver) {
        sprintf(statusMsg, "Player %c's turn", isXTurn ? 'X' : 'O');
        printCentered(13, statusMsg, PAIR_STATUS, true);

        // Modify the position of the X/O indicator to be centered
        int msgLen = strlen(statusMsg);
        int centerX = (screenWidth - msgLen) / 2;
        int xPos = centerX + 8; // Position after "Player " in the message

        if (isXTurn) {
            attron(COLOR_PAIR(PAIR_X) | A_BOLD);
            mvprintw(13, xPos, "X");
            attroff(COLOR_PAIR(PAIR_X) | A_BOLD);
        } else {
            attron(COLOR_PAIR(PAIR_O) | A_BOLD);
            mvprintw(13, xPos, "O");
            attroff(COLOR_PAIR(PAIR_O) | A_BOLD);
        }
    } else {
        if (winner != ' ') {
            sprintf(statusMsg, "Player %c Wins!", winner);
            printCentered(13, statusMsg, PAIR_WIN_MSG, true);
        } else {
            printCentered(13, "Game ended in a draw!", PAIR_WIN_MSG, true);
        }
    }

    // Controls help
    printCentered(15, "Controls: Arrow Keys to move, Enter to place", PAIR_GRID);
    printCentered(16, "          Q to quit, R to restart game", PAIR_GRID);

    printCenter("/red/skibidi", screenHeight-1);

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

    // Get screen dimensions
    getmaxyx(stdscr, screenHeight, screenWidth);

    // Setup colors if terminal supports them
    if (has_colors()) {
        initColors();
        initPrintColors();
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