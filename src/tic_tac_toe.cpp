#include "tic_tac_toe.hpp"
#include <ncurses.h>
#include <vector>
#include <iostream>
#include <wchar.h>
#include <chrono>
#include <cstring>

char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
int cursorX = 0, cursorY = 0;
bool isXTurn = true;
bool gameOver = false;
char winner = ' ';
bool isBoardFull = false;

static int termHeight, termWidth;

#define PAIR_TITLE 1
#define PAIR_GRID 2
#define PAIR_X 3
#define PAIR_O 4
#define PAIR_CURSOR 5
#define PAIR_STATUS 6
#define PAIR_WIN_MSG 7
#define PAIR_SEPARATOR 8

// ASCII art title
std::vector<std::string> title {
    "████████╗██╗ ██████╗      ████████╗ █████╗  ██████╗      ████████╗ ██████╗ ███████╗",
    "╚══██╔══╝██║██╔════╝      ╚══██╔══╝██╔══██╗██╔════╝      ╚══██╔══╝██╔═══██╗██╔════╝",
    "   ██║   ██║██║     █████╗   ██║   ███████║██║     █████╗   ██║   ██║   ██║█████╗  ",
    "   ██║   ██║██║     ╚════╝   ██║   ██╔══██║██║     ╚════╝   ██║   ██║   ██║██╔══╝  ",
    "   ██║   ██║╚██████╗         ██║   ██║  ██║╚██████╗         ██║   ╚██████╔╝███████╗",
    "   ╚═╝   ╚═╝ ╚═════╝         ╚═╝   ╚═╝  ╚═╝ ╚═════╝         ╚═╝    ╚═════╝ ╚══════╝",
};

// Initialize ncurses color pairs
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

// Helper function to print text centered horizontally
void printCentered(int y, const char* text, int colorPair, bool bold = false) {
    int x = (termWidth - strlen(text)) / 2;
    if (x < 0) x = 0;

    if (bold) attron(COLOR_PAIR(colorPair) | A_BOLD);
    else attron(COLOR_PAIR(colorPair));

    mvprintw(y, x, "%s", text);

    if (bold) attroff(COLOR_PAIR(colorPair) | A_BOLD);
    else attroff(COLOR_PAIR(colorPair));
}

// Draw the ASCII art title with proper centering
void drawTitle(int startY) {
    attron(COLOR_PAIR(PAIR_TITLE) | A_BOLD);

    for (size_t i = 0; i < title.size(); i++) {
        // Calculate the width of the ASCII art line
        size_t displayWidth = title[i].length();

        // Center horizontally
        int startX = (termWidth - displayWidth) / 2;
        if (startX < 0) startX = 0;

        mvprintw(startY + i, startX, "%s", title[i].c_str());
    }

    attroff(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
    refresh();
}

// Draw the game grid with box-drawing characters
void drawGrid(int startY) {
    attron(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);

    // Calculate the starting point to center the grid
    int gridWidth = 13; // Width of the grid including borders
    int startX = (termWidth - gridWidth) / 2;
    if (startX < 0) startX = 0;

    // Draw the grid with nice box characters
    // Horizontal lines
    mvaddstr(startY + 2, startX, "├───┼───┼───┤");
    mvaddstr(startY + 4, startX, "├───┼───┼───┤");

    // Top and bottom borders
    mvaddstr(startY + 0, startX, "┌───┬───┬───┐");
    mvaddstr(startY + 6, startX, "└───┴───┴───┘");

    // Vertical lines (for rows 1, 3, 5 which are content rows)
    for (int i = 1; i <= 5; i += 2) {
        mvaddch(startY + i, startX, '│');
        mvaddch(startY + i, startX + 4, '│');
        mvaddch(startY + i, startX + 8, '│');
        mvaddch(startY + i, startX + 12, '│');
    }

    attroff(COLOR_PAIR(PAIR_SEPARATOR) | A_BOLD);
}

// Draw a single cell of the game grid
void drawCell(int y, int x, char value, int gridStartY) {
    int gridWidth = 13;
    int startX = (termWidth - gridWidth) / 2;
    if (startX < 0) startX = 0;

    // Calculate exact position for cell content
    int screenY = gridStartY + y * 2 + 1;  // +1 to center in the cell
    int screenX = startX + x * 4 + 2;      // +2 to center in the cell

    // Draw cursor highlight if this is the active cell
    if (cursorX == x && cursorY == y && !gameOver) {
        attron(COLOR_PAIR(PAIR_CURSOR));
        mvaddch(screenY, screenX, ' ');
        attroff(COLOR_PAIR(PAIR_CURSOR));
    }

    // Draw X or O based on the cell value
    if (value == 'X') {
        attron(COLOR_PAIR(PAIR_X) | A_BOLD);
        mvaddch(screenY, screenX, 'X');
        attroff(COLOR_PAIR(PAIR_X) | A_BOLD);
    } else if (value == 'O') {
        attron(COLOR_PAIR(PAIR_O) | A_BOLD);
        mvaddch(screenY, screenX, 'O');
        attroff(COLOR_PAIR(PAIR_O) | A_BOLD);
    }
}

// Draw the status message showing current player or game result
void drawStatus(int statusY) {
    char statusMsg[50];

    if (!gameOver) {
        sprintf(statusMsg, "Player %c's turn", isXTurn ? 'X' : 'O');
        printCentered(statusY, statusMsg, PAIR_STATUS, true);

        // Highlight the player symbol with appropriate color
        int msgLen = strlen(statusMsg);
        int centerX = (termWidth - msgLen) / 2;
        int xPos = centerX + 7;  // Position of X/O in the status message

        if (isXTurn) {
            attron(COLOR_PAIR(PAIR_X) | A_BOLD);
            mvprintw(statusY, xPos, "X");
            attroff(COLOR_PAIR(PAIR_X) | A_BOLD);
        } else {
            attron(COLOR_PAIR(PAIR_O) | A_BOLD);
            mvprintw(statusY, xPos, "O");
            attroff(COLOR_PAIR(PAIR_O) | A_BOLD);
        }
    } else {
        if (winner != ' ') {
            sprintf(statusMsg, "Player %c Wins!", winner);
            printCentered(statusY, statusMsg, PAIR_WIN_MSG, true);

            // Highlight the winner symbol
            int msgLen = strlen(statusMsg);
            int centerX = (termWidth - msgLen) / 2;
            int xPos = centerX + 7;  // Position of X/O in the status message

            int colorPair = (winner == 'X') ? PAIR_X : PAIR_O;
            attron(COLOR_PAIR(colorPair) | A_BOLD);
            mvprintw(statusY, xPos, "%c", winner);
            attroff(COLOR_PAIR(colorPair) | A_BOLD);
        } else {
            printCentered(statusY, "Game ended in a draw!", PAIR_WIN_MSG, true);
        }
    }
}

// Draw the complete game board with all elements
void drawBoard() {
    clear();
    getmaxyx(stdscr, termHeight, termWidth);

    // Calculate the total space needed
    int titleHeight = title.size();
    int gridHeight = 7;  // Height of the grid (3 cells plus borders)
    int controlsHeight = 2;  // Height for controls display
    int spacing = 1;  // Spacing between components

    int totalHeight = titleHeight + spacing + gridHeight + spacing + 1 + spacing + controlsHeight;

    // Calculate starting positions to center everything vertically
    int startY = (termHeight - totalHeight) / 2;
    if (startY < 0) startY = 0;

    int titleY = startY;
    int gridY = titleY + titleHeight + spacing;
    int statusY = gridY + gridHeight + spacing;
    int controlsY = statusY + 1 + spacing;

    // Draw all components
    drawTitle(titleY);
    drawGrid(gridY);

    // Draw cells
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            drawCell(i, j, board[i][j], gridY);
        }
    }

    // Draw status and controls
    drawStatus(statusY);
    printCentered(controlsY, "Controls: Arrow Keys to move, Enter to place", PAIR_GRID);
    printCentered(controlsY + 1, "          Q to quit, R to restart game", PAIR_GRID);

    refresh();
}

// Check if the current player has won
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

// Check if the board is full (draw condition)
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

// Reset the game state
void resetGame() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }

    cursorX = 0;
    cursorY = 0;
    isXTurn = true;
    gameOver = false;
    winner = ' ';
    isBoardFull = false;
}

// Main game function
void playTicTacToe() {
    // Initialize ncurses
    setlocale(LC_ALL, "");  // Set locale for proper wide character handling
    initscr();
    cbreak();
    noecho();
    curs_set(0);  // Hide cursor
    keypad(stdscr, TRUE);  // Enable arrow keys

    // Get terminal dimensions
    getmaxyx(stdscr, termHeight, termWidth);

    // Initialize colors if supported
    if (has_colors()) {
        initColors();
    }

    // Main game loop
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
                    // Place X or O if cell is empty
                    if (board[cursorY][cursorX] == ' ') {
                        board[cursorY][cursorX] = isXTurn ? 'X' : 'O';

                        // Check win conditions
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

    // Cleanup
    endwin();
}