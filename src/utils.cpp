#include "utils.hpp"
#include <regex>
#include <iostream>

// Initialize all colors in ncurses
void initPrintColors() {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);    // Red
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Green
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Yellow
    init_pair(4, COLOR_BLUE, COLOR_BLACK);   // Blue
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Magenta
    init_pair(6, COLOR_CYAN, COLOR_BLACK);   // Cyan
    init_pair(7, COLOR_WHITE, COLOR_BLACK);  // White
    init_pair(8, COLOR_BLACK, COLOR_WHITE);  // Black on white
}

// Function to center and print colored text with line argument
void printCenter(const std::string &str, int line) {
    // Get the terminal size
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // Find the length of the string and calculate the position to center it
    int len = str.length();
    int x = (maxX - len) / 2;

    // Print the string with color formatting
    std::string formattedStr = str;
    std::regex colorRegex("/([a-zA-Z]+)/");  // Regex to match color codes like /green/
    std::smatch match;

    // Start the color pair by default (set to normal color)
    attron(COLOR_PAIR(0));

    int currentPos = 0;
    while (std::regex_search(formattedStr, match, colorRegex)) {
        // Print text before the color code
        std::string beforeColor = formattedStr.substr(currentPos, match.position());
        mvprintw(line, x, beforeColor.c_str());
        currentPos = match.position() + match.length();

        // Determine color based on the matched code
        std::string color = match.str(1);  // The color name without slashes

        if (color == "red") {
            attron(COLOR_PAIR(1));  // Red
        } else if (color == "green") {
            attron(COLOR_PAIR(2));  // Green
        } else if (color == "yellow") {
            attron(COLOR_PAIR(3));  // Yellow
        } else if (color == "blue") {
            attron(COLOR_PAIR(4));  // Blue
        } else if (color == "magenta") {
            attron(COLOR_PAIR(5));  // Magenta
        } else if (color == "cyan") {
            attron(COLOR_PAIR(6));  // Cyan
        } else if (color == "white") {
            attron(COLOR_PAIR(7));  // White
        } else if (color == "black") {
            attron(COLOR_PAIR(8));  // Black on white
        }

        // Print the colored part of the string
        std::string coloredText = formattedStr.substr(currentPos, match.position(0) - currentPos);
        mvprintw(line, x, coloredText.c_str());
        currentPos = match.position(0) + match.length();

        // Turn off color
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(3));
        attroff(COLOR_PAIR(4));
        attroff(COLOR_PAIR(5));
        attroff(COLOR_PAIR(6));
        attroff(COLOR_PAIR(7));
        attroff(COLOR_PAIR(8));
    }

    // Print the remaining part of the string after the last color code
    std::string remainingText = formattedStr.substr(currentPos);
    mvprintw(line, x, remainingText.c_str());

    refresh();
}
