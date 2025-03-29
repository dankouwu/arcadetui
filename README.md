# ArcadeTUI

ArcadeTUI is a terminal-based arcade game collection built using C++ and the ncurses library. It aims to provide a nostalgic experience of classic arcade games with a modern text-based user interface. The project is designed to run in a terminal, making it lightweight and perfect for low-resource environments.

## Features

- **TUI-Based Interface**: The project leverages the power of the terminal with the ncurses library to render interactive text-based user interfaces (TUI).
- **Classic Games**: Includes implementations of popular arcade games such as Tic-Tac-Toe and other simple games.
- **Interactive Design**: Arrow keys, enter keys, and other keyboard inputs allow you to control the game and navigate through menus.
- **Colorful and Clean UI**: Utilizes color pairs to make the user interface visually appealing and easy to navigate.

## Requirements

To run this project, you need to have the following installed:

- **C++ Compiler** (e.g., `g++`, `clang++`)
- **CMake** (for building the project)
- **ncurses** library (for handling terminal graphics and input)

On Linux, you can install the `ncurses` library using your package manager:

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev  # For Ubuntu/Debian
sudo yum install ncurses-devel  # For Fedora/CentOS

