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

// Function to get right of the assigned color
void no_color()
{
    printf("\033[0m");
}

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

    // Counting the size of the board to know what size of array to make
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
    char maze[heightNum][widthNum];

    // Setting the reading point back to the start of the file
    fseek(mazefile, 0, SEEK_SET);
    
    // Reading the maze from file and storing in board array
    int i = 0, j = 0;
    int dots, points;
    for(i = 0; i < heightNum; i++)
    {
        for (j = 0; j < widthNum; j++)
        {
            
            char key;
            key = fgetc(mazefile);
            if (key == ' ')
            {
                board[i][j] = '.';
                dots++;
            }
            else 
            {
                board[i][j] = key;    // board is maze with points
                    
            }
            maze[i][j] = key;   // maze is just clear maze from txt file
            
        }
    }
    fclose(mazefile);
    points = dots*10;
    int x = widthNum/2, y = heightNum/2;    //starting position of pacman (middle)
    int score = 0;
    // Main game process
    while (exit == 0) {

        // allows for screen refreshing without printing another screen        
        clear_screen();

        for (int i = 0; i < heightNum; i++) {
            for (int j = 0; j < widthNum; j++) {
                if (i == y && j == x) {
                    printf("\033[0;33mC");  // Pacman character // yellow
                    no_color();
                    if (board[i][j] == '.')
                    {
                        board[i][j] = ' ';
                        score += 10;
                    }
                    

                } else {
                    if (board[i][j] == '.')
                    {
                        printf("%c", board[i][j]);
                    }
                    else
                    {
                        printf("\033[0;34m%c", board[i][j]);     //blue //printf("■\n");
                        no_color();
                    }
            


                    // printf("\033[0;34m%c", board[i][j]);      //printf("■\n");
                    // printf("\033[0m");
                }
            }
            //printf("\n");

        }
        printf("\nCurrent Score: %d\n", score);

        printf("height: %d, Width: %d\n", heightNum, widthNum);

        if (score == points)
        {
            clear_screen();
            printf("\nYou Won!\nYour Score: %d", score);
        }
        printf("\nPress 'q' to leave.\n");

        char key = get_key();
        
        // Move pacman with WASD keys
        switch (key) {
            case 'w':
                if (maze[y-1][x] == ' ')
                {
                    y--;
                }
                break;

            case 'a':
                if (maze[y][x-1] == ' ')
                {
                    x--;
                }
                break;

            case 's':
                if (maze[y+1][x] == ' ')
                {
                    y++;
                }
                break;

            case 'd':
                if (maze[y][x+1] == ' ')
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