/*
Final Project: Pacman Game
Authors: Fabian Chrzanowski
Date: 01/05/2024
Description:

*/

/* TO DO:

* Fix the first collision between monsters and pacman as it is 
not being registered
* Make the layout nicer
* Try making monsters respawn for unlimited gameplay
* monsters cant spawn in the same spot as pacman (more than 6 from pacman)


*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <time.h>

// Constants
#define NUM_MONSTER 0   // Number of monsters
#define NUM_FOOD 4      // Number of food
#define SCATTER_TIME 30 // Time (ticks) for scatter mode
#define MAZE_COLOR "\e[0;34m"  // Choose your own maze color;

//Regular text
#define YELLOW "\033[0;33m"
#define BLACK "\e[0;30m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define BLUE "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background 
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define reset "\e[0m"
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"

// Struct holding monster information
struct monster
{
    int x;
    int y;
    int alive;
    char* look;
    int scatter;
    int scatter_timer;
};

// Struct holding game information
struct game
{
    int heightNum;
    int widthNum;
    int numofchar;
    int max_dotpoints;
    int dotpoints;
    int foodpoints;
    int monsterpoints;
    int counter;
    int score;
    int x;
    int y;
    int exit;
    int end;
    int contin;
    char* maze_color;
    char look;
    char board[150][150];  // board which tracks points (150 for maximum board size)
    char maze[150][150];    // board which stays empty 
};

// Function prototypes
void pacman_logo(FILE *logo);
void menu(FILE *logo, char *decision);
void no_color();
void clear_screen();
void set_raw_mode();
char get_key();
void move_monster(struct game *Pacman, struct monster *monster);
void maze_input(struct game *Pacman, struct monster *monster, FILE *mazefile);
void draw_board(struct game *Pacman, struct monster *monster, FILE *logo);

// Main function
int main() {
    
    srand(time(NULL));  // Random seed for random number generation

    set_raw_mode();     // Setting the terminal to raw mode 

    struct game Pacman;     // Game struct
    struct monster Monster[NUM_MONSTER];    // Monster struct
    int check = 0;
    char cond;
    int exit;
    FILE *mazefile;

    // Opening the logo file
    FILE *logo = fopen("pacman-logo.txt", "r");
    if (logo == NULL) 
    {
        printf("\n**Failed to open the logo file.**\n\n");
    }

    // Starting menu
    menu(logo, &cond);
    if (cond == 'Q' || cond == 'q')
    {
        check = 1;
        printf("\n\tQuitting the game...\n");
        sleep(1);
        clear_screen();
    }
    else
    {
        check = 0;
        printf("\n\tStarting the game...\n");
        sleep(1);
    }

    // Main game loop until user decides to quit
    while (check == 0)
    {   
        exit = 0;

        // Opening the maze file
        mazefile = fopen("maze2.txt", "r");
        
        if (mazefile == NULL) {
            printf("\n**Failed to open the maze file.**\n\n");   
        }
        
        // Inputting the maze from file to the game
        maze_input(&Pacman, Monster, mazefile);

        fclose(mazefile);
        
        // Updating game process
        while (exit == 0) 
        {   
            clear_screen();

            // Drawing the game board with pacman and monsters
            draw_board(&Pacman, Monster, logo);

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
    fclose(logo);
}

// Function to display the Pacman logo
void pacman_logo(FILE *logo)
{   
    fseek(logo, 0, SEEK_SET);
    char ch;
    while ((ch = fgetc(logo)) != EOF)
    {
        printf(YELLOW);
        printf("%c", ch);
    }
    printf("\n");
    no_color();
}

// Function to display the starting menu
void menu(FILE *logo, char *decision)
{
    clear_screen();
    pacman_logo(logo);
    printf("\n");
    printf(YELLOW"\tPress any key to Start\n");
    no_color();
    printf(RED"\n\tQuit (Q)\n");
    no_color();
    *decision = get_key();
}

// Function to get right of the assigned color
void no_color()
{
    printf("\033[0m");
}

// Function to clear the terminal screen
void clear_screen() 
{
    printf("\033[H\033[J");
}

// Function to set the terminal to raw mode
void set_raw_mode() 
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to handle keyboard input
char get_key() 
{
    char key = getchar();
    fflush(stdout);
    return key;
}

// Function to handle the scatter mode of the monsters
void monster_scatter(struct game *Pacman, struct monster *monster)
{
    for (int i = 0; i < NUM_MONSTER; i++)
    {
            if (monster[i].scatter == 1)
            {
                if (monster[i].scatter_timer < SCATTER_TIME)
                {
                    monster[i].scatter_timer++;
                }
                
                if (monster[i].scatter_timer == SCATTER_TIME)
                {
                    monster[i].scatter = 0;
                    monster[i].scatter_timer = 0;
                }
            }
        
    }
}

// Function to move the monsters
void move_monster(struct game *Pacman, struct monster *monster)
{

    if (Pacman->counter % 2 == 0)
    {
        for (int i = 0; i < NUM_MONSTER; i++)
        {
            int x = monster[i].x;
            int y = monster[i].y;
            int x_diff = Pacman->x - x;
            int y_diff = Pacman->y - y;
            if (monster[i].alive == 1)
            {
                if (monster[i].scatter == 0)
                {
                    monster[i].look = RED;  // Monster character // red
                    if (abs(x_diff) > abs(y_diff))  // Monster has to go in the x-plane
                    {
                        if (x_diff > 0)     // Pacman is to the right
                        {     
                            if (Pacman->maze[y][x+1] == ' ')
                            {
                                monster[i].x++;
                            }
                            else if (y_diff > 0)
                            {
                                if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                                else if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                            }
                            else if (y_diff < 0)
                            {
                                if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                                else if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                            }
                            else if (Pacman->maze[y][x-1] == ' ')
                            {
                                monster[i].x--;
                            }
                        }
                        else if (x_diff < 0)    // Pacman is to the left
                        {
                            if (Pacman->maze[y][x-1] == ' ')
                            {
                                monster[i].x--;
                            }
                            else if (y_diff > 0)
                            {
                                if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                                else if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                            }
                            else if (y_diff < 0)
                            {
                                if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                                else if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                            }
                            else if (Pacman->maze[y][x+1] == ' ')
                            {
                                monster[i].x++;
                            }
                        }
                    }
                    else if (abs(x_diff) < abs(y_diff))  // Monster has to go in y-plane
                    {
                        if (y_diff > 0)     // Pacman is below
                        {
                            if (Pacman->maze[y+1][x] == ' ')
                            {
                                monster[i].y++;
                            }
                            else if (x_diff > 0)
                            {
                                if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                                else if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                            }
                            else if (x_diff < 0)
                            {
                                if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                                else if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                            }
                            else if (Pacman->maze[y-1][x] == ' ')
                            {
                                monster[i].y--;
                            }
                        }
                        else if (y_diff < 0)    // Pacman is above
                        {
                            if (Pacman->maze[y-1][x] == ' ')
                            {
                                monster[i].y--;
                            }
                            else if (x_diff > 0)    // Pacman is above and to the right
                            {
                                if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                                else if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                            }
                            else if (x_diff < 0)    // Pacman is above and to the left 
                            {
                                if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                                else if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                            }
                            else if (Pacman->maze[y+1][x] == ' ')
                            {
                                monster[i].y++;
                            }
                        }
                    }
                }
                else if (monster[i].scatter == 1)
                {
                    monster[i].look = GREEN;  // Monster character // green
                    if (abs(x_diff) > abs(y_diff))  // Monster has to go in the x-plane
                    {
                        if (x_diff > 0)     // Pacman is to the right
                        {     
                            if (Pacman->maze[y][x-1] == ' ')
                            {
                                monster[i].x--;
                            }
                            else if (y_diff > 0)
                            {
                                if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                                else if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                            }
                            else if (y_diff < 0)
                            {
                                if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                                else if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                            }
                            else if (Pacman->maze[y][x+1] == ' ')
                            {
                                monster[i].x++;
                            }
                        }
                        else if (x_diff < 0)    // Pacman is to the left
                        {
                            if (Pacman->maze[y][x+1] == ' ')
                            {
                                monster[i].x++;
                            }
                            else if (y_diff > 0)
                            {
                                if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                                else if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++; 
                                }
                            }
                            else if (y_diff < 0)
                            {
                                if (Pacman->maze[y+1][x] == ' ')
                                {
                                    monster[i].y++;
                                }
                                else if (Pacman->maze[y-1][x] == ' ')
                                {
                                    monster[i].y--;
                                }
                            }
                            else if (Pacman->maze[y][x-1] == ' ')
                            {
                                monster[i].x--;
                            }
                        }
                    }
                    else if (abs(x_diff) < abs(y_diff))  // Monster has to go in y-plane
                    {
                        if (y_diff > 0)     // Pacman is below
                        {
                            if (Pacman->maze[y-1][x] == ' ')
                            {
                                monster[i].y--;
                            }
                            else if (x_diff > 0)
                            {
                                if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                                else if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                            }
                            else if (x_diff < 0)
                            {
                                if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                                else if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                            }
                            else if (Pacman->maze[y+1][x] == ' ')
                            {
                                monster[i].y++;
                            }
                        }
                        else if (y_diff < 0)    // Pacman is above
                        {
                            if (Pacman->maze[y+1][x] == ' ')
                            {
                                monster[i].y++;
                            }
                            else if (x_diff > 0)
                            {
                                if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                                else if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                            }
                            else if (x_diff < 0)
                            {
                                if (Pacman->maze[y][x+1] == ' ')
                                {
                                    monster[i].x++;
                                }
                                else if (Pacman->maze[y][x-1] == ' ')
                                {
                                    monster[i].x--;
                                }
                            }
                            else if (Pacman->maze[y-1][x] == ' ')
                            {
                                monster[i].y--;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Function that inputs the maze from file to the game
void maze_input(struct game *Pacman, struct monster *monster , FILE *mazefile)
{
    // Initial values
    Pacman->numofchar = 0;
    Pacman->heightNum = 1;
    Pacman->widthNum = 0;
    Pacman->score = 0;
    Pacman->max_dotpoints = 0;
    Pacman->foodpoints = 0;
    Pacman->dotpoints = 0;
    Pacman->monsterpoints = 0;
    Pacman->counter = 0;
    Pacman->look = '<';
    Pacman->contin = 0;
    

    for (int i = 0; i < NUM_MONSTER; i++)
    {
        monster[i].scatter = 0;
        monster[i].scatter_timer = 0;
        monster[i].alive = 1;
        monster[i].look = RED;
    }

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
    
    // Pacman starting position
    Pacman->x = Pacman->widthNum/2;
    Pacman->y = Pacman->heightNum/2;

    // Setting the reading point back to the start of the file
    fseek(mazefile, 0, SEEK_SET);

    // Reading the maze from file and storing in board array

    int dots = 0;
    int fCount = 0;
    int mCount = 0;
    for(int i = 0; i < Pacman->heightNum; i++)
    {
        for (int j = 0; j < Pacman->widthNum; j++)
        {
            char key;
            key = fgetc(mazefile);
            if (key == ' ')
            {
                Pacman->board[i][j] = '.';
                dots++;
            }
            else 
            {
                Pacman->board[i][j] = key;
            }    // board is maze with points    
            

            Pacman->maze[i][j] = key;   // maze is just clear maze from txt file
            
        }
    }

    // Randomly placing monsters
    while (mCount < NUM_MONSTER)
    {
    
            int x = rand() % Pacman->widthNum;
            int y = rand() % Pacman->heightNum;
            if (Pacman->board[y][x] == '.')
            {
                monster[mCount].x = x;
                monster[mCount].y = y;
                mCount++;
            }
        
    }
    
    // Randomly placing food
    while (fCount < NUM_FOOD)
    {
        int x = rand() % Pacman->widthNum;
        int y = rand() % Pacman->heightNum;
        if (Pacman->board[y][x] == '.')
        {
            Pacman->board[y][x] = 'o';
            fCount++;
        }
    }
    
    // Calculating maximum possible points for given board from dots
    Pacman->max_dotpoints = (dots - NUM_FOOD) * 10;

    
}

// Function to draw the game board
void draw_board(struct game *Pacman, struct monster *monster , FILE *logo)
{   
    // Game logo
    pacman_logo(logo);

    // Calculating the score (Because game ends when all dots are eaten)
    Pacman->score = Pacman->dotpoints + Pacman->foodpoints + Pacman->monsterpoints;

    // Printing the game information
    printf("\nMax dot points: %d\n", Pacman->max_dotpoints);
    printf(RED"\n\t\tScore: %d\n\n", Pacman->score);    
    no_color();

    // Initial conditions for every loop iteration
    Pacman->end = 0;
    Pacman->maze_color = MAZE_COLOR;

    // Printing the board
    for (int i = 0; i < Pacman->heightNum; i++) 
    {
        printf("     ");
        for (int j = 0; j < Pacman->widthNum; j++) 
        {
            if (i == Pacman->y && j == Pacman->x) 
            {
                printf(YELLOW"%c", Pacman->look);  // Pacman character // yellow
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
                    for (int k = 0; k < NUM_MONSTER; k++)
                    {
                        if (monster[k].scatter == 0)
                        {
                            monster[k].scatter = 1;
                        }
                        if (monster[k].scatter == 1)
                        {
                            monster[k].scatter_timer = 0;
                        }
                    }
                }
            }
            else if (i == monster[0].y && j == monster[0].x)
            {
                if (monster[0].alive == 1)
                {
                    printf("%sM", monster[0].look);  
                    no_color();
                }
                else if (monster[0].alive == 0)
                {
                    printf(" ");
                }
            }
            else if (i == monster[1].y && j == monster[1].x)
            {
                if (monster[1].alive == 1)
                {
                    printf("%sM", monster[1].look); 
                    no_color();
                }
                else if (monster[1].alive == 0)
                {
                    printf(" ");
                }
            }
            else if (i == monster[2].y && j == monster[2].x)
            {
                if (monster[2].alive == 1)
                {
                    printf("%sM", monster[2].look);  
                    no_color();
                }
                else if (monster[2].alive == 0)
                {
                    printf(" ");
                }
            }
            else if (i == monster[3].y && j == monster[3].x)
            {
                if (monster[3].alive == 1)
                {
                    printf("%sM", monster[3].look);  
                    no_color();
                }
                else if (monster[3].alive == 0)
                {
                    printf(" ");
                }
            }
            else
            {   for (int k = 0; k < NUM_MONSTER; k++)
                {
                    if (monster[k].x == Pacman->x && monster[k].y == Pacman->y)
                    {
                        if (monster[k].alive == 1)
                        {
                            if (monster[k].scatter == 0)
                            {
                                Pacman->end = 1;
                                //Pacman->maze_color

                            }
                            else if (monster[k].scatter == 1)
                            {

                                    Pacman->monsterpoints += 200;
                                    monster[k].alive = 0;
                            }
                        }
                    }
                }        

                if (Pacman->board[i][j] == '.')
                {
                    printf("%c", Pacman->board[i][j]);
                }
                else if (Pacman->board[i][j] == 'o')
                {
                    printf(MAGENTA"%c", Pacman->board[i][j]);
                    no_color();
                }
                else if (Pacman->board[i][j] == ' ')
                {
                    printf("%c", Pacman->board[i][j]);  
                }
                else
                {
                    printf("%s%c", Pacman->maze_color, Pacman->board[i][j]);    
                    no_color();
                }
            }
        }
    }

    // Scatter function for monsters so we can count the time for scatter mode
    monster_scatter(Pacman, monster);

    printf("\n\nQuit (Q)\nRestart (C)\n");
    
    if (Pacman->dotpoints == Pacman->max_dotpoints)
    {
        // clear_screen();
        // pacman_logo(logo);
        //printf("\n\tYou Won!\n\n\tYour Score: %d\n\n\tQuit (Q)\n\tPlay Again (C)\n", Pacman->score);
        printf(YELLOW"\nYou Won!\n");
        no_color();
        char choice = get_key();
        while (choice != 'q' && choice != 'Q' && choice != 'c' && choice != 'C')
        {
            choice = get_key();
        }
        if (choice == 'q' || choice == 'Q')
        {   
            printf("\nQuitting the game...\n");
            sleep(1);
            Pacman->exit = 1;
        }
        else if (choice == 'c' || choice == 'C')
        {
            printf("\nStarting a new game...\n");
            sleep(1);
            printf("Press any key to continue...\n");
            Pacman->contin = 1;
        }
        
    }
    else if (Pacman->end == 1)
    {
        //Pacman->maze_color = RED;
        printf(RED"\nYou Lost!\n");
        no_color();
        char choice = get_key();
        while (choice != 'q' && choice != 'Q' && choice != 'c' && choice != 'C')
        {
            choice = get_key();
        }
        if (choice == 'q' || choice == 'Q')
        {   
            printf("\nQuitting the game...\n");
            sleep(1);
            Pacman->exit = 1;
        }
        else if (choice == 'c' || choice == 'C')
        {
            printf("\nStarting a new game...\n");
            sleep(1);
            printf("Press any key to continue...\n");
            Pacman->contin = 1;
        }
    }

    if (Pacman->end == 0)
    {
        char key = get_key();

        // Move pacman with WASD keys
        switch (key) 
        {
            case 'w':
                if (Pacman->maze[Pacman->y-1][Pacman->x] == ' ')
                {
                    Pacman->y--;
                    Pacman->counter++;
                    move_monster(Pacman, monster);
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
                if (Pacman->maze[Pacman->y][Pacman->x-1] == ' ')
                {
                    Pacman->x--;
                    Pacman->counter++;
                    move_monster(Pacman, monster);
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
                    Pacman->counter++;
                    move_monster(Pacman, monster);
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
                if (Pacman->maze[Pacman->y][Pacman->x+1] == ' ')
                {
                    Pacman->x++;
                    Pacman->counter++;
                    move_monster(Pacman, monster);
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
                printf("\nQuitting the game...\n");
                sleep(1);
                Pacman->exit = 1;
                break;

            case 'c':
                printf("\nStarting a new game...\n");
                sleep(1);
                Pacman->contin = 1;
                break;
        }
    }
}
