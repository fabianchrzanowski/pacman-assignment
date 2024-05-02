/*
Module: ACS130
Final C Project: Pacman Game
Author: Fabian Chrzanowski
Date: 01/05/2024

Description: A simple Pac-Man game implemented in C. The game is played in the terminal
using the WASD keys to move the Pac-Man character. Game finishes when all dots have been eaten.
Monsters follow the Pac-Man character and can be eaten after eating one of randomly placed foods,
but only for a limited time and monsters respawn after a certain time.
When food is eaten, monsters also go into "scatter" mode for a certain time - running from Pac-Man. 
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // For usleep
#include <termios.h>    // For terminal settings
#include <time.h>       // For random number generation

// Constants
#define NUM_MONSTER 4   // Number of monsters
#define NUM_FOOD 4      // Number of food
#define SCATTER_TIME 30 // Time (ticks) for scatter mode
#define DOT_POINTS 10   // Points for eating dots
#define FOOD_POINTS 50  // Points for eating food
#define MONSTER_POINTS 200  // Points for eating monsters
#define MONSTER_RESPAWN_TIME 10 // Time (ticks) for monster respawn
#define SPAWN_AURA 5    // Aura for monster spawn

//Regular text
#define YELLOW "\033[0;33m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define BLUE "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN "\e[0;36m"

// Struct holding monster information
struct monster
{
    int x;                  // Position of monsters
    int startX;             // Starting position of monsters
    int y;                  // Position of monsters
    int startY;             // Starting position of monsters
    int alive;              // Alive condition for monsters
    int scatter;            // Scatter mode for monsters
    int dead_timer;         // Dead timer for monsters
    int scatter_timer;      // Scatter timer for monsters
    char* look;             // Monster character look
};

// Struct holding game information
struct game
{
    int heightNum;          // Height of the maze
    int widthNum;           // Width of the maze
    int numofchar;          // Number of characters in the maze
    int max_dotpoints;      // Maximum possible points for given board
    int dotpoints;          // Points for eating dots
    int foodpoints;         // Points for eating food
    int monsterpoints;      // Points for eating monsters
    int counter;            // Counter for monster movement
    int score;              // Game score
    int x;                  // Pacman position x
    int y;                  // Pacman position y
    int exit;               // Exit condition
    int end;                // End condition
    int contin;             // Restart condition
    int menu;               // Menu condition
    char* maze_color;       // Maze color
    char look;              // Pacman character look
    char board[150][150];   // Board which tracks points (150x150 for maximum board size)
    char maze[150][150];    // Board which stays empty 
};

// Function prototypes
void pacman_logo(FILE *logo);                                                   // Function to display the pacman logo
void menu(FILE *logo, char *decision);                                          // Function to display the starting menu
void no_color();                                                                // Function to get right of the assigned color
void clear_screen();                                                            // Function to clear the terminal screen
void set_raw_mode();                                                            // Function to set the terminal to raw mode
char get_key();                                                                 // Function to handle keyboard input
void monster_scatter(struct monster *monster);                                  // Function to handle the scatter mode of the monsters
void move_monster(struct game *Pacman, struct monster *monster);                // Function to move the monsters
void initial_values(struct game *Pacman, struct monster *monster);              // Function to initialize the values of the game
void maze_input(struct game *Pacman, FILE *mazefile);                           // Function that inputs the maze from file to the game
void monster_input(struct game *Pacman, struct monster *monster);               // Function to randomly place monsters
void food_input(struct game *Pacman);                                           // Function to randomly place food
void monster_conditions(struct monster *monster, int monster_id);               // Function to handle the conditions when monster is dead
void draw_board(struct game *Pacman, struct monster *monster, FILE *logo);      // Function to draw the game board
void ending_conditions(struct game *Pacman);                                    // Function to handle the ending conditions of the game
void pacman_moves(struct game *Pacman, struct monster *monster);                // Function to handle the movement of pacman
void maze_color_choice(struct game *Pacman, int default_or_custom);             // Function for choosing the maze color

// Main function
int main() {
    
    srand(time(NULL));  // Random seed for random number generation

    set_raw_mode();     // Setting the terminal to raw mode for more user friendly experience of the game

    struct game Pacman;     // Game struct
    struct monster Monster[NUM_MONSTER];    // Monster struct
    int check;
    int menucheck = 0;
    int map = 0;
    int default_color = 1;
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
    while (menucheck == 0)
    {   
        // Menu section
        menu(logo, &cond);
        if (cond == 'Q' || cond == 'q')
        {
            menucheck = 1;
            check = 1;
            printf("\n\tQuitting the game...\n");
            usleep(300000);
            clear_screen();
        }
        else if (cond == 'S' || cond == 's')
        {
            // Settings section
            char settings_cond = '\0';
            clear_screen();
            pacman_logo(logo);
            printf(YELLOW"\n\tSettings\n");
            no_color();
            printf("\n\tMaze color settings (A)\n\n\tMaze map choice (D)\n");
            printf(RED"\n\tBack (B)\n");
            no_color();
            while(settings_cond != 'A' && settings_cond != 'D' && settings_cond != 'B' && settings_cond != 'a' && settings_cond != 'd' && settings_cond != 'b')
            {
                settings_cond = get_key();
            }
            check = 1;
            if (settings_cond == 'A' || settings_cond == 'a')
            {
                check = 1;

                // Choosing the maze color
                maze_color_choice(&Pacman, 0);
                default_color = 0;
                
            }
            else if (settings_cond == 'D' || settings_cond == 'd')
            {
                // Choosing the maze map
                printf("\n\tMaze 1 (A)\n\tMaze 2 (Bigger!) (D)\n");

                char map_choice = '\0';

                while (map_choice != 'A' && map_choice != 'a' && map_choice != 'D' && map_choice != 'd')
                {
                    map_choice = get_key();
                }
                check = 1;
                if (map_choice == 'A' || map_choice == 'a')
                {
                    map = 1;
                    mazefile = fopen("maze1.txt", "r");
                    printf("\n\tMaze 1 chosen!\n\tYou can start the game now!\n");
                }
                else if (map_choice == 'D' || map_choice == 'd')
                {
                    map = 2;
                    mazefile = fopen("maze2.txt", "r");
                    printf("\n\tMaze 2 chosen!\n\tYou can start the game now!\n");
                }
            }

        }
        else if (cond == 'F' || cond == 'f')
        {
            // About the game section
            char about_cond = '\0';
            clear_screen();
            pacman_logo(logo);
            printf(YELLOW"\n\tControls:\n");
            no_color();
            printf("\n\tLeft - 'A'\n\tRight - 'D'\n\tUp - 'W'\n\tDown - 'S'\n\tQuit - 'Q'\n\tRestart - 'C'\n");
            printf(RED"\n\tBack (B)\n");
            while (about_cond != 'B' && about_cond != 'b')
            {
                about_cond = get_key();
            }
            check = 1;
        }
        else
        {   
            // Checking the maze file
            if (mazefile == NULL) 
            {
                mazefile = fopen("maze1.txt", "r");
                if (mazefile == NULL) 
                {
                printf("\n**Failed to open the maze file.**\n\n");
                }   
            }
            check = 0;
            printf("\n\tStarting the game...\n");
            usleep(300000);
            menucheck = 1;
        }
    // Main game loop until user decides to quit
    while (check == 0)
    {   
        // Opening the maze file
        if (map == 2)
        {
            mazefile = fopen("maze2.txt", "r");
        }
        else
        {   // Default maze file if the user did not choose the maze from the menu
            mazefile = fopen("maze1.txt", "r");
        }

        // Default color for the maze if the color was not called from the menu
        if(default_color == 1)
        {
            maze_color_choice(&Pacman, 1);
        }
        exit = 0;
        // Inputting the maze from file to the game
        maze_input(&Pacman, mazefile);

        // Initializing the game values
        initial_values(&Pacman, Monster);

        // Randomly placing food and monsters
        monster_input(&Pacman, Monster);
        food_input(&Pacman);

        // Closing the maze file
        fclose(mazefile);
        
        // Updating game process
        while (exit == 0) 
        {   
            clear_screen();

            // Drawing the game board with pacman and monsters
            draw_board(&Pacman, Monster, logo);

            // Checking the exit conditions
            if (Pacman.exit == 1)
            {
                clear_screen();
                menucheck = 1;
                check = 1;
                exit = 1;
                
            }   // Checking the conditions for restarting the game
            else if (Pacman.contin == 1)
            {
                menucheck = 0;
                check = 0;
                exit = 1;
            }   // Checking the conditions for going back to the menu
            else if (Pacman.menu == 1)
            {
                menucheck = 0;
                check = 1;
                exit = 1;
            }
        }
    }
}
    // Closing the logo file
    fclose(logo);
}

// Function to display the starting menu
void menu(FILE *logo, char *decision)
{
    clear_screen();
    pacman_logo(logo);

    printf("\n");
    printf(YELLOW"\tPress any key to Start\n");
    no_color();
    printf("\n\tSettings (S)\n");
    printf("\n\tAbout (F)\n");
    printf(RED"\n\tQuit (Q)\n");

    no_color();

    //Pushing the decision to the main function
    *decision = get_key();
}


// Function to display the Pacman logo
void pacman_logo(FILE *logo)
{   
    // Resetting the reading point to the start of the file
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

    // Flushing the output buffer
    fflush(stdout);

    // Returning the pressed key
    return key;
}

// Function to handle the scatter mode of the monsters
void monster_scatter(struct monster *monster)
{   
    // For loop for all monsters
    for (int i = 0; i < NUM_MONSTER; i++)
    {
        if (monster[i].scatter == 1)
        {
            // Counting the scatter time
            if (monster[i].scatter_timer < SCATTER_TIME)
            {
                monster[i].scatter_timer++;
            }
            
            // If scatter time ends, monsters go back to normal mode
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
    // Counter for monster movement so monsters move slower than the Pacman, allowing to actually win the game
    if (Pacman->counter % 2 == 0)
    {
        for (int i = 0; i < NUM_MONSTER; i++)
        {
            int x = monster[i].x;           // Monster x position
            int y = monster[i].y;           // Monster y position
            int x_diff = Pacman->x - x;     // Difference in x-plane
            int y_diff = Pacman->y - y;     // Difference in y-plane
            if (monster[i].alive == 1)      // If monster is alive
            {
                if (monster[i].scatter == 0)
                {
                    monster[i].look = RED;          // Monster character - red in normal mode
                    if (abs(x_diff) > abs(y_diff))  // Monster has to go in the x-plane because the distance is bigger in x-plane
                    {
                        if (x_diff > 0)             // Pacman is to the right
                        {    
                            // Checking position next to the monster for empty space so it can actually go in bounds of the maze
                            // Further downa are different conditions for the movement of the monster if space next to it is not empty
                            // We are using pacman maze array to check if the space is empty as it does not have anything printed on it
                            // only the empty maze
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
                            else if (y_diff > 0)    // Pacman is to the left and below
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
                            else if (y_diff < 0)    // Pacman is above and to the left
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
                            else if (x_diff > 0)    // Pacman is below and to the right
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
                            else if (x_diff < 0)    // Pacman is below and to the left
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
                // Every condition is the same as in normal mode, but reversed so monsters are running from pacman
                else if (monster[i].scatter == 1)
                {
                    monster[i].look = GREEN;        // Monster character - green in scatter mode
                    if (abs(x_diff) > abs(y_diff))  // Monster has to go in the x-plane
                    {
                        if (x_diff > 0)     // Pacman is to the right
                        {     
                            if (Pacman->maze[y][x-1] == ' ')
                            {
                                monster[i].x--;
                            }
                            else if (y_diff > 0)    // Pacman is to the right and below
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
                            else if (y_diff < 0)    // Pacman is to the right and above
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
                            else if (y_diff > 0)    // Pacman is to the left and below 
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
                            else if (y_diff < 0)    // Pacman is to the left and above
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
                            else if (x_diff > 0)    // Pacman is below and to the right 
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
                            else if (x_diff < 0)    // Pacman is below and to the left 
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
                            else if (x_diff > 0)    // Pacman is above and to the right 
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
                            else if (x_diff < 0)    // Pacman is above and to the left 
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

// Function to initialize the values of the game
void initial_values(struct game *Pacman, struct monster *monster)
{

    // Initial values for the game
    Pacman->score = 0;
    Pacman->max_dotpoints = 0;
    Pacman->foodpoints = 0;
    Pacman->dotpoints = 0;
    Pacman->monsterpoints = 0;
    Pacman->counter = 0;
    Pacman->look = '<';
    Pacman->contin = 0;
    Pacman->menu = 0;
    
    // Pacman starting position
    Pacman->x = Pacman->widthNum/2;
    Pacman->y = Pacman->heightNum/2;

    // Initial values for the monsters
    for (int i = 0; i < NUM_MONSTER; i++)
    {
        monster[i].scatter = 0;
        monster[i].scatter_timer = 0;
        monster[i].alive = 1;
        monster[i].look = RED;
        monster[i].dead_timer = 0;
    }
}

// Function that inputs the maze from file to the game
void maze_input(struct game *Pacman, FILE *mazefile)
{
    // Initial values for the board
    Pacman->numofchar = 0;
    Pacman->heightNum = 1;
    Pacman->widthNum = 0;

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
            Pacman->numofchar++;        // number of characters in the maze
        }
        
    }
    // Calculating the width of the maze
    Pacman->widthNum = (Pacman->numofchar+Pacman->heightNum)/Pacman->heightNum;

    // Setting the reading point back to the start of the file
    fseek(mazefile, 0, SEEK_SET);

    // Reading the maze from file and storing in board array
    int dots = 0;
    for(int i = 0; i < Pacman->heightNum; i++)
    {
        for (int j = 0; j < Pacman->widthNum; j++)
        {
            char key;
            key = fgetc(mazefile);
            if (key == ' ')
            {
                Pacman->board[i][j] = '.';  // board is maze with points
                dots++;                     // counting the dots
            }
            else 
            {
                Pacman->board[i][j] = key;
            }   
            Pacman->maze[i][j] = key;   // maze is just clear maze from txt file
        }
    }
    
    // Calculating maximum possible points for given board from dots
    Pacman->max_dotpoints = (dots - NUM_FOOD) * 10;
}

// Function to randomly place food
void food_input(struct game *Pacman)
{
    int fCount = 0;
    // Randomly placing food
    while (fCount < NUM_FOOD)
    {
        // Random x and y coordinates of the board for food 
        int x = rand() % Pacman->widthNum;      // Random x coordinate of the board for food
        int y = rand() % Pacman->heightNum;     // Random y coordinate of the board for food
        if (Pacman->board[y][x] == '.')
        {
            Pacman->board[y][x] = 'o';
            fCount++;
        }
    }
}

// Function for choosing the maze color
void maze_color_choice(struct game *Pacman, int default_or_custom)
{
    if (default_or_custom == 1) // Default color - BLUE
    {
        Pacman->maze_color = BLUE;
    }
    else
    {
        // Custom color choice
        printf("\n\t1. Blue\n\t2. Red\n\t3. Green\n\t4. Yellow\n\t5. Magenta\n\t6. Cyan\n");
        char color_choice = '\0';
        while (color_choice != '1' && color_choice != '2' && color_choice != '3' && color_choice != '4' && color_choice != '5' && color_choice != '6')
        {
            color_choice = getc(stdin);
        }
        if (color_choice == '1')
        {
            Pacman->maze_color = BLUE;
        }
        else if (color_choice == '2')
        {
            Pacman->maze_color = "\e[0;31m";
        }
        else if (color_choice == '3')
        {
            Pacman->maze_color = GREEN;
        }
        else if (color_choice == '4')
        {
            Pacman->maze_color = YELLOW;
        }
        else if (color_choice == '5')
        {
            Pacman->maze_color = MAGENTA;
        }
        else if (color_choice == '6')
        {
            Pacman->maze_color = CYAN;
        }
    }
}

// Function to randomly place monsters
void monster_input(struct game *Pacman, struct monster *monster)
{
    // Randomly placing monsters inside the board
    int mCount = 0;
    while (mCount < NUM_MONSTER)
    {
        int x = rand() % Pacman->widthNum;      // Random x coordinate of the board for monster
        int y = rand() % Pacman->heightNum;     // Random y coordinate of the board for monster
        if (Pacman->board[y][x] == '.')
        {
            monster[mCount].startX = x;         // Starting x position of the monster we will use it for respawn
            monster[mCount].startY = y;         // Starting y position of the monster we will use it for respawn
            monster[mCount].x = x;
            monster[mCount].y = y;
            if (abs(Pacman->x - x) > SPAWN_AURA && abs(Pacman->y - y) > SPAWN_AURA)     // Monsters can't spawn too close to the pacman
            {
                mCount++;       // Counting the monsters only if they are not too close to the pacman
            }
        }
    }
}
/// Function to handle the conditions when monster is dead
void monster_conditions(struct monster *monster, int monster_id)
{
    if (monster[monster_id].alive == 1)
    {
        // Printing the monster character
        printf("%sM", monster[monster_id].look);  
        no_color();
    }
    else if (monster[monster_id].alive == 0)
    {
        // Printing the empty space if monster is dead
        if (monster[monster_id].dead_timer < MONSTER_RESPAWN_TIME)
        {
            monster[monster_id].dead_timer++;
            printf(" ");
            
        }
        // Respawn the monster after the respawn time
        else if (monster[monster_id].dead_timer >= MONSTER_RESPAWN_TIME)
        {
            printf(" ");    // Too fix the 'dent' in the board when monster respawns
            monster[monster_id].alive = 1;
            monster[monster_id].dead_timer = 0;
            monster[monster_id].x = monster[monster_id].startX;     // Respawn x position
            monster[monster_id].y = monster[monster_id].startY;     // Respawn y position
            monster[monster_id].scatter = 0;
        }
    }
}

// Function to draw the game board
void draw_board(struct game *Pacman, struct monster *monster , FILE *logo)
{   
    // Game logo
    pacman_logo(logo);

    // Initial conditions for every loop iteration
    Pacman->end = 0;

    // Calculating the score (Because game ends when all dots are eaten)
    Pacman->score = Pacman->dotpoints + Pacman->foodpoints + Pacman->monsterpoints;

    // Printing the game information
    printf("\n\t  Collect all dots!!\n");
    printf(RED"\n\t\tScore: %d\n\n", Pacman->score);    
    no_color();

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
                    Pacman->dotpoints += DOT_POINTS;        // Eating dots
                }
                else if (Pacman->board[i][j] == 'o')
                {
                    Pacman->board[i][j] = ' ';
                    Pacman->foodpoints += FOOD_POINTS;
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
            }   // Printing monsters, I can't find other way to do it wihtout break statement
            else if (i == monster[0].y && j == monster[0].x)
            {
                monster_conditions(monster, 0);
            }
            else if (i == monster[1].y && j == monster[1].x)
            {
                monster_conditions(monster, 1);
            }
            else if (i == monster[2].y && j == monster[2].x)
            {
                monster_conditions(monster, 2);
            }
            else if (i == monster[3].y && j == monster[3].x)
            {
               monster_conditions(monster, 3);
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
                            }
                            else if (monster[k].scatter == 1)
                            {

                                Pacman->monsterpoints += MONSTER_POINTS;   
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
    monster_scatter(monster);
    
    ending_conditions(Pacman);

    pacman_moves(Pacman, monster);
}

// Function to handle the movement of pacman
void pacman_moves(struct game *Pacman, struct monster *monster)
{
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
                // Calling the function to move the monsters
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
            usleep(300000);
            Pacman->exit = 1;
            break;

        case 'c':
            printf("\nStarting a new game...\n");
            usleep(300000);
            Pacman->contin = 1;
            break;
        
        case 'b':
            printf("\nGoing back to the menu...\n");
            usleep(300000);
            Pacman->menu = 1;
            break;
        }
    }
}

// Function to handle the ending conditions of the game
void ending_conditions(struct game *Pacman)
{
    printf("\n\n\tRestart (C)\n");
    printf(RED"\n\tQuit (Q)\n");
    no_color();
    printf(YELLOW"\n\tBack to menu (B)\n");
    no_color();

    if (Pacman->dotpoints == Pacman->max_dotpoints)
    {
        printf(YELLOW"\n\tYou Won!\n");
        no_color();
        char choice = get_key();
        while (choice != 'q' && choice != 'Q' && choice != 'c' && choice != 'C')
        {
            choice = get_key();
        }
        if (choice == 'q' || choice == 'Q')
        {   
            printf("\nQuitting the game...\n");
            usleep(300000);
            Pacman->exit = 1;
        }
        else if (choice == 'c' || choice == 'C')
        {
            printf("\nStarting a new game...\n");
            usleep(300000);
            printf("Press any key to continue...\n");
            Pacman->contin = 1;
        }
        
    }
    else if (Pacman->end == 1)
    {
        //Pacman->maze_color = RED;
        printf(RED"\n\tYou Lost!\n");
        no_color();
        char choice = get_key();
        while (choice != 'q' && choice != 'Q' && choice != 'c' && choice != 'C')
        {
            choice = get_key();
        }
        if (choice == 'q' || choice == 'Q')
        {   
            printf("\nQuitting the game...\n");
            usleep(300000);
            Pacman->exit = 1;
        }
        else if (choice == 'c' || choice == 'C')
        {
            printf("\nStarting a new game...\n");
            usleep(300000);
            printf("Press any key to continue...\n");
            Pacman->contin = 1;
        }
    }
}