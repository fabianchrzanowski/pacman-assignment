#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define WIDTH 20
#define HEIGHT 10

int x, y; // pacman position

struct game
{
    char pixels[100][100];      // width == how many before \n and height how many \n
};

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
void draw_board(int heightNum, int widthNum) {      //nie wiem czy dam rade to zrobic w funkcji
    
    // clear_screen();

    
    // for (int i = 0; i < heightNum; i++) {
    //     for (int j = 0; j < widthNum; j++) {
    //         if (i == y && j == x) {
    //             printf("\033[0;33mC");  // Pacman character
    //             printf("\033[0m");
    //         } else {
    //             printf("\033[0;34m.");      //printf("■\n");
    //             printf("\033[0m");
    //         }
    //     }
    //     printf("\n");

    // }
    // printf("\nPress 'q' to leave.\n");
}

int main() {

    set_raw_mode();     // setting the terminal

    int exit = 0;
    char ch;

    FILE *mazefile;
    mazefile = fopen("maze.txt", "r");

    if (mazefile == NULL) {
        printf("Failed to open the file.\n");   
    }

    int numofchar = 0;
    int heightNum = 1;
    int widthNum = 0;

    // Counting the size of the board
    while ((ch = fgetc(mazefile)) != EOF) 
    {
        
        if (ch == '\n')
        {   
            heightNum++;        // height of the maze
        }
        else
        {
            numofchar++;
        }
        
    }
    
    widthNum = (numofchar+heightNum)/heightNum;     // width of the maze

    char board[heightNum][widthNum];

    // Setting the reading point back to the start of the file
    fseek(mazefile, 0, SEEK_SET);
    
    // Reading the maze from file and storing in board array
    int i = 0, j = 0;
    
    for(i = 0; i < heightNum; i++)
    {
        for (j = 0; j < widthNum; j++)
        {
            
            
            
            board[i][j] = fgetc(mazefile);
            
            //fscanf(mazefile, "%c", &board[i][j]);
            //board[i][j] = fgetc(mazefile);
            // ch = fgetc(mazefile);
            // while (fgetc(mazefile) == '\n') {  // Skip over newline characters
            //     ch = fgetc(mazefile);
            // }
            // board[i][j] = ch;
        }
    }
    fclose(mazefile);

    int x = widthNum/2, y = heightNum/2;    //starting position of pacman (middle)

    // Main game process
    while (exit == 0) {

        clear_screen();

        for (int i = 0; i < heightNum; i++) {
            for (int j = 0; j < widthNum; j++) {
                if (i == y && j == x) {
                    printf("\033[0;33mC");  // Pacman character
                    printf("\033[0m");  // Command to remove the color
                } else {
                    printf("\033[0;34m%c", board[i][j]);      //printf("■\n");
                    printf("\033[0m");
                }
            }
            //printf("\n");

        }
        printf("\nPress 'q' to leave.\n");

        printf("height: %d, Width: %d\n", heightNum, widthNum);

        char key = get_key();
        
        // Move pacman with WASD keys
        switch (key) {
            case 'w':
                if (board[y-1][x] == ' ')
                {
                    y--;
                }
                break;

            case 'a':
                if (board[y][x-1] == ' ')
                {
                    x--;
                }
                break;

            case 's':
                if (board[y+1][x] == ' ')
                {
                    y++;
                }
                break;

            case 'd':
                if (board[y][x+1] == ' ')
                {
                    x++;
                }
                break;

            case 'q':
                clear_screen();     
                exit = 1;
                break;
        
        }

        // Keeping pacman coordinates inside the given board
        if (x >= widthNum) 
        {
            x = widthNum - 1;
        }
        else if (x <= 0) 
        {
            x = 0;
        }
        

        if (y >= heightNum) 
        { 
            y = heightNum - 1; 
        }
        else if (y <= 0)
        {
            y = 0;
        }
    }

    return 0;
}