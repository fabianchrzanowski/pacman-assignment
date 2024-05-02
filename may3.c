#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
//#include <ncurses.h>
#include <time.h>

// Struct holding game information
struct game
{
    int heightNum;
    int widthNum;
    int numofchar;
    int max_dotpoints;
    int dotpoints;
    int foodpoints;
    int score;
    int highscore;
    int x;
    int y;
    int exit;
    int contin;
    char look;
    char board[150][150];  // board which tracks points (150 for maximum board size)
    char maze[150][150];    // board which stays empty 
};

// Function prototypes
void pacman_logo(FILE *logo);
void no_color();
void clear_screen();
void set_raw_mode();
char get_key();
void maze_input(struct game *Pacman, FILE *mazefile);
void draw_board(struct game *Pacman, FILE *logo);

// Main function
int main() {
    
    srand(time(NULL));
    set_raw_mode();     // setting the terminal to raw mode 

    struct game Pacman;
    int check = 0;
    int exit;
    FILE *mazefile;
    FILE *logo = fopen("pacman-logo.txt", "r");
    // FILE *highscore;

    // highscore = fopen("highscore.txt", "w");
    // if (highscore == NULL) {
    //     printf("Failed to open the highscore file.\n");
    // }
    // int hscore = 0;

    while (check == 0)
    {   
        exit = 0;
        if (logo == NULL) {
            printf("Failed to open the logo file.\n");
        }
        
        mazefile = fopen("maze2.txt", "r");
        
        if (mazefile == NULL) {
            printf("Failed to open the maze file.\n");   
        }
        
        maze_input(&Pacman, mazefile);

        fclose(mazefile);
        
        // Main game process
        while (exit == 0) 
        {   
            clear_screen();
            draw_board(&Pacman, logo);

            if (Pacman.exit == 1)
            {
                clear_screen();
                check = 1;
                exit = 1;
                
            }
            else if (Pacman.contin == 1)
            {
                check = 0;
                exit = 1;
            }
        }
        
    }
    //fclose(highscore);
    fclose(logo);
}

// Function to display the Pacman logo
void pacman_logo(FILE *logo)
{   
    fseek(logo, 0, SEEK_SET);
    char ch;
    while ((ch = fgetc(logo)) != EOF)
    {
        printf("\033[0;31m");
        printf("%c", ch);
    }
    printf("\n");
    no_color();
}

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

// Function that inputs the maze from file to the game
void maze_input(struct game *Pacman, FILE *mazefile)
{
    // Initial values
    Pacman->numofchar = 0;
    Pacman->heightNum = 1;
    Pacman->widthNum = 0;
    Pacman->score = 0;
    Pacman->max_dotpoints = 0;
    Pacman->foodpoints = 0;
    Pacman->dotpoints = 0;
    Pacman->look = '<';
    Pacman->contin = 0;

    char ch;
    // Counting the size of the board to know what size of array to make
    while ((ch = fgetc(mazefile)) != EOF) 
    {
        if (ch == '\n')
        {   
            Pacman->heightNum++;        // height of the maze
        }
        else
        {
            Pacman->numofchar++;
        }
        
    }

    Pacman->widthNum = (Pacman->numofchar+Pacman->heightNum)/Pacman->heightNum;     // width of the maze

    // Setting the reading point back to the start of the file
    fseek(mazefile, 0, SEEK_SET);

    // Reading the maze from file and storing in board array
    int i = 0, j = 0;
    int dots = 0;
    int fCount = 0;
    int mCount = 0;
    for(i = 0; i < Pacman->heightNum; i++)
    {
        for (j = 0; j < Pacman->widthNum; j++)
        {
            int fRand = rand() % 50;
            int mRand = rand() % 50;
            char key;
            key = fgetc(mazefile);
            if (key == ' ')
            {
                if (fRand > 48 && fCount < 3)
                {
                    Pacman->board[i][j] = 'o';
                    fCount++;
                }
                else if (mRand > 48 && mCount < 4)
                {
                    Pacman->board[i][j] = 'M';
                    mCount++;
                }
                else
                {
                    Pacman->board[i][j] = '.';
                    dots++;
                }
            }
            else 
            {
                Pacman->board[i][j] = key;    // board is maze with points
                    
            }
            Pacman->maze[i][j] = key;   // maze is just clear maze from txt file
            
        }
    }

    // fscanf(highscore, "%d", hscore);
    
    // Pacman->highscore = *hscore;
    
    // Calculating maximum possible points for given board from dots
    Pacman->max_dotpoints = dots*10;

    // Pacman starting position
    Pacman->x = Pacman->widthNum/2;
    Pacman->y = Pacman->heightNum/2;
}

// Function to draw the game board
void draw_board(struct game *Pacman, FILE *logo)
{   
    pacman_logo(logo);

    for (int i = 0; i < Pacman->heightNum; i++) {
        for (int j = 0; j < Pacman->widthNum; j++) {
            if (i == Pacman->y && j == Pacman->x) {
                printf("\033[0;33m%c", Pacman->look);  // Pacman character // yellow
                no_color();
                if (Pacman->board[i][j] == '.')
                {
                    Pacman->board[i][j] = ' ';
                    Pacman->dotpoints += 10;        // Eating dots
                }
                else if (Pacman->board[i][j] == 'o')
                {
                    Pacman->board[i][j] = ' ';
                    Pacman->foodpoints += 100;
                }
                
            } else {
                if (Pacman->board[i][j] == '.')
                {
                    printf("%c", Pacman->board[i][j]);
                }
                else if (Pacman->board[i][j] == 'o')
                {
                    printf("\033[0;31m%c", Pacman->board[i][j]);
                    no_color();
                }
                else if (Pacman->board[i][j] == 'M') 
                {
                    printf("\033[0;35m%c", Pacman->board[i][j]); // Magenta
                    no_color(); 
                }
                
                else
                {
                    printf("\033[0;34m%c", Pacman->board[i][j]);     //blue //printf("■\n");
                    no_color();
                }
            }
    
        }

    }
    Pacman->score = Pacman->dotpoints + Pacman->foodpoints;
    // if (Pacman->score > Pacman->highscore)
    // {
    //     Pacman->highscore = Pacman->score;
        
    // }
    printf("\nCurrent Score: %d\tMax dot points: %d\n", Pacman->score, Pacman->max_dotpoints);
    printf("\nPress 'q' to leave.\n");

    if (Pacman->dotpoints == Pacman->max_dotpoints)
    {
        clear_screen();
        pacman_logo(logo);
        // fprintf(highscore, "%d", Pacman->highscore);
        printf("\nYou Won!\nYour Score: %d\nPress 'q' to leave.\nPress 'c' to play again.\n", Pacman->score);
        
    }

    char key = get_key();
    // Move pacman with WASD keys
    switch (key) {
        case 'w':
            if (Pacman->maze[Pacman->y-1][Pacman->x] == ' ')
            {
                Pacman->y--;
            }
            else if (Pacman->y >= Pacman->heightNum) 
            { 
                Pacman->y = Pacman->heightNum - 1; 
            }
            else if (Pacman->y <= 0)
            {
                Pacman->y = 0;
            }
            Pacman->look = 'v';
            break;

        case 'a':
            if (Pacman->maze[Pacman->y][Pacman->x-1] == ' ' || Pacman->maze[Pacman->y+1][Pacman->x] == '1')
            {
                Pacman->x--;
            }
            else if (Pacman->x >= Pacman->widthNum) 
            {
                Pacman->x = Pacman->widthNum - 1;
            }
            else if (Pacman->x <= 0) 
            {
                Pacman->x = 0;
            }
            Pacman->look = '>';
            break;

        case 's':
            if (Pacman->maze[Pacman->y+1][Pacman->x] == ' ')
            {
                Pacman->y++;
            }
            else if (Pacman->y >= Pacman->heightNum) 
            { 
                Pacman->y = Pacman->heightNum - 1; 
            }
            else if (Pacman->y <= 0)
            {
                Pacman->y = 0;
            }
            Pacman->look = '^';
            break;

        case 'd':
            if (Pacman->maze[Pacman->y][Pacman->x+1] == ' ' || Pacman->maze[Pacman->y+1][Pacman->x] == '2')
            {
                Pacman->x++;
            }
            else if (Pacman->x >= Pacman->widthNum) 
            {
                Pacman->x = Pacman->widthNum - 1;
            }
            else if (Pacman->x <= 0) 
            {
                Pacman->x = 0;
            }
            Pacman->look = '<';
            break;

        case 'q':  
            Pacman->exit = 1;
            break;

        case 'c':
            Pacman->contin = 1;
            break;
    }
}
