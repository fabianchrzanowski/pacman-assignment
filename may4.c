#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <time.h>

// Constants
#define NUM_MONSTER 4   // Number of monsters
#define NUM_FOOD 3      // Number of food
#define SCATTER_TIME 20 // Time for scatter mode

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
    int score;
    int x;
    int y;
    int exit;
    int end;
    int contin;
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
    
    srand(time(NULL));
    set_raw_mode();     // setting the terminal to raw mode 

    struct game Pacman;
    struct monster Monster[NUM_MONSTER];
    int check = 0;
    char cond;
    int exit;
    FILE *mazefile;
    FILE *logo = fopen("pacman-logo.txt", "r");
    if (logo == NULL) 
    {
        printf("\n**Failed to open the logo file.**\n\n");
    }

    menu(logo, &cond);
    if (cond == 'X' || cond == 'x')
    {
        check = 0;
        printf("\nStarting the game...\n");
        sleep(1);
    }
    else
    {
        check = 1;
        printf("\nQuitting the game...\n");
        sleep(1);
        clear_screen();
    }

    while (check == 0)
    {   
        exit = 0;

        mazefile = fopen("maze2.txt", "r");
        
        if (mazefile == NULL) {
            printf("\n**Failed to open the maze file.**\n\n");   
        }
        
        maze_input(&Pacman, Monster, mazefile);

        fclose(mazefile);
        
        // Main game process
        while (exit == 0) 
        {   
            clear_screen();
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
    printf("Play (X)");
    printf(RED"\tPress any key to quit\n");
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
                move_monster(Pacman, monster);
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

    for (int i = 0; i < NUM_MONSTER; i++)
    {
        int x = monster[i].x;
        int y = monster[i].y;
        int x_diff = Pacman->x - x;
        int y_diff = Pacman->y - y;
        if (monster[i].scatter == 0)
        {
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
                    else if (Pacman->maze[y+1][x] == ' ')
                    {
                        monster[i].y++;
                    }
                }
            }
        }
        else if (monster[i].scatter == 1)
        {
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
    Pacman->look = '<';
    Pacman->contin = 0;

    for (int i = 0; i < NUM_MONSTER; i++)
    {
        monster[i].scatter = 0;
        monster[i].scatter_timer = 0;
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
                Pacman->board[i][j] = key;    // board is maze with points    
            }

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

    // Pacman starting position
    Pacman->x = Pacman->widthNum/2;
    Pacman->y = Pacman->heightNum/2;
}

// Function to draw the game board
void draw_board(struct game *Pacman, struct monster *monster , FILE *logo)
{   
    pacman_logo(logo);

    Pacman->score = Pacman->dotpoints + Pacman->foodpoints;

    printf("\nMax dot points: %d\n", Pacman->max_dotpoints);
    printf(CYAN"\nCurrent Score: %d\n", Pacman->score);    
    no_color();
    Pacman->end = 0;

    for (int i = 0; i < Pacman->heightNum; i++) 
    {
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
                    }
                }
                else
                {
                    for (int k = 0; k < NUM_MONSTER; k++)
                    {
                        if (monster[k].scatter == 0)
                        {
                            if (monster[k].x == Pacman->x && monster[k].y == Pacman->y)
                            {
                                Pacman->end = 1;
                            }
                        }
                    }
                }
            } 
            else if (i == monster[0].y && j == monster[0].x)
            {
                printf(MAGENTA"%c", 'M');  // Monster character // red
                no_color();
            }
            else if (i == monster[1].y && j == monster[1].x)
            {
                printf(MAGENTA"%c", 'M');  // Monster character // red
                no_color();
            }
            else if (i == monster[2].y && j == monster[2].x)
            {
                printf(MAGENTA"%c", 'M');  // Monster character // red
                no_color();
            }
            else if (i == monster[3].y && j == monster[3].x)
            {
                printf(MAGENTA"%c", 'M');  // Monster character // red
                no_color();
            }
            else
            {
                if (Pacman->board[i][j] == '.')
                {
                    printf("%c", Pacman->board[i][j]);
                }
                else if (Pacman->board[i][j] == 'o')
                {
                    printf("\033[0;31m%c", Pacman->board[i][j]);
                    no_color();
                }
                else if (Pacman->board[i][j] == ' ')
                {
                    printf("%c", Pacman->board[i][j]);     //blue //printf("■\n");
                }
                else
                {
                    printf(BBLU"%c", Pacman->board[i][j]);     //blue //printf("■\n");
                    no_color();
                }
            }
        }
    }

    monster_scatter(Pacman, monster);

    printf("\n\nQuit (Q)\n");
    
    if (Pacman->dotpoints == Pacman->max_dotpoints)
    {
        clear_screen();
        pacman_logo(logo);
        printf("\nYou Won!\n\nYour Score: %d\n\nQuit (Q)\nPlay Again (C)\n", Pacman->score);
        sleep(1);
        
    }
    else if (Pacman->end == 1)
    {
        clear_screen();
        pacman_logo(logo);
        printf(RED"\nYou Lost!\n\nQuit (Q)\nPlay Again (C)\n");
        no_color();
        Pacman->exit = 1;
        sleep(1);
    }

    //move_monster(Pacman, monster);

    char key = get_key();
    // Move pacman with WASD keys
    switch (key) 
    {
        case 'w':
            if (Pacman->maze[Pacman->y-1][Pacman->x] == ' ')
            {
                Pacman->y--;
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
            if (Pacman->maze[Pacman->y][Pacman->x-1] == ' ' || Pacman->maze[Pacman->y+1][Pacman->x] == '1')
            {
                Pacman->x--;
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
            if (Pacman->maze[Pacman->y][Pacman->x+1] == ' ' || Pacman->maze[Pacman->y+1][Pacman->x] == '2')
            {
                Pacman->x++;
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
