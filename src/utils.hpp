#ifndef UTILS_HPP
#define UTILS_HPP

#include <ncurses.h>
#include <string>

void initPrintColors();

void printCenter(const std::string &str, int line);

#endif