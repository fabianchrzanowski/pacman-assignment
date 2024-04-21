#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define WIDTH 20
#define HEIGHT 10

int x, y; // Pacman's position

// Function to clear the terminal screen
void clear_screen() {
    printf("\033[H\033[J");
}

// Function to set the terminal to raw mode
void set_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to handle keyboard input
char get_key() {
    char key = getchar();
    fflush(stdout);
    return key;
}

// Function to draw the game board
void draw_board() {
    clear_screen();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == y && j == x) {
                printf("C"); // Pacman character
            } else {
                printf("."); // Empty space
            }
        }
        printf("\n");
    }
}

int main() {
    set_raw_mode();

    x = WIDTH / 2;
    y = HEIGHT / 2;

    while (1) {
        draw_board();

        char key = get_key();

        // Move Pacman based on arrow keys
        switch (key) {
            case 'w':
                y--;
                break;
            case 'a':
                x--;
                break;
            case 's':
                y++;
                break;
            case 'd':
                x++;
                break;
            case 'q':
                exit(0);
        }
    }

    return 0;
}