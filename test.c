#include <stdio.h>

int main(){

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

    while ((ch = fscanf(mazefile, "%c")) != EOF) 
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
    
    widthNum = numofchar/heightNum;     // width of the maze
    char board[widthNum][heightNum];

    for (int i = 0; i < heightNum; i++) {
            for (int j = 0; j < widthNum; j++) {
                char ch = fscanf(mazefile, "%c");
                if (ch != '\n') {
                    board[j][i] = ch;
                }
            }
        }
        fclose(mazefile);

    // Print contents of board array
    for (int i = 0; i < heightNum; i++) {
        for (int j = 0; j < widthNum; j++) {
            printf("%c", board[j][i]);
        }
        printf("\n");
    }
}

