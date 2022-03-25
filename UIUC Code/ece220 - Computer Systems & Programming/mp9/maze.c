/* Our code solves a maze with the use of 4 functions. The first creates
the maze by allocating memory,  reading a 2D array from a text file through
file io, and putting the correct symbol in each cell. The next function
printmaze, prints out the maze. SolvemazeDFS uses recursion to get through
the maze, backtracking when encountering a wall or a previously visited
spot on the path. Lastly, destroymaze frees all allocated memory and destroys
the maze.
Partner: ry6
*/

#include <stdio.h>
#include <stdlib.h>
#include "maze.h"


/*
 * createMaze -- Creates and fills a maze structure from the given file
 * INPUTS:       fileName - character array containing the name of the maze file
 * OUTPUTS:      None
 * RETURN:       A filled maze structure that represents the contents of the input file
 * SIDE EFFECTS: None
 */
maze_t * createMaze(char * fileName)
{
  int i,j,width,height;
//  char *cells;
    FILE * f = fopen(fileName, "r"); //need to open test files
    maze_t * maze = malloc(sizeof(maze_t)); //have to set aside memory for the maze
    fscanf(f, "%d %d",&width,&height); //scans the height and width of the 2D array
    fgetc(f); //get the newline that comes right after the width and height


    maze->width = width;    //the scanned width is input into the width in the struct
    maze->height = height;  //same thing for the height

    maze->cells = (char **) malloc(sizeof(char *)*height);  //sets aside memory for the cells

    for (i=0;i<height;i++){
        maze->cells[i] = (char *) malloc(sizeof(char)*(width+1));
    }

    //maze_t.cells = cells;
    for(i=0;i<height;i++){
      for(j=0;j<width+1;j++){
        maze->cells[i][j]=fgetc(f);    //goes through the maze and inputs the values of the maze into its respective spots

        if(maze->cells[i][j]== START){  //sets start column and startrow to the start location
          maze->startColumn = j;
          maze->startRow = i;
          }
        if(maze->cells[i][j]== END){  //sets endcolumn and endrow to the end location
          maze->endColumn = j;
          maze->endRow = i;
          }
      }

    }
    //maze->cells[row][col] width height<51
    // cells contains the layout for the maze_t(2d array)(first index = row,2nd=col)
    //create a pointer using malloc that will then use the struct to find the width and height
    //first two numbers = width and height, followed by %
    // space represents an empty cell S start E end


    //In C, the library function malloc is used to
    //allocate a block of memory on the heap
    // Your code here. Make sure to replace following line with your own code.
    fclose(f);    //closes the file
    return maze;
}

/*
 * destroyMaze -- Frees all memory associated with the maze structure, including the structure itself
 * INPUTS:        maze





  -- pointer to maze structure that contains all necessary information
 * OUTPUTS:       None
 * RETURN:        None
 * SIDE EFFECTS:  All memory that has been allocated for the maze is freed
 */
void destroyMaze(maze_t * maze)
{

  free(maze->cells);    //removes all the values of the cells
  free(maze);     //removes the values in the struct maze_t
    // Your code here.
}

/*
 * printMaze --  Prints out the maze in a human readable format (should look like examples)
 * INPUTS:       maze -- pointer to maze structure that contains all necessary information
 *               width -- width of the maze
 *               height -- height of the maze
 * OUTPUTS:      None
 * RETURN:       None
 * SIDE EFFECTS: Prints the maze to the console


 */
void printMaze(maze_t * maze)
{
  int i,j;
  for(i=0;i<maze->height;i++){
    for(j=0;j<((maze->width)+1);j++){
      printf("%c",maze->cells[i][j]);   //prints the board cell by cell
    }
  }
    // Your code here.
}

/*
 * solveMazeManhattanDFS -- recursively solves the maze using depth first search,
 * INPUTS:               maze -- pointer to maze structure with all necessary maze information
 *                       col -- the column of the cell currently beinging visited within the maze
 *                       row -- the row of the cell currently being visited within the maze
 * OUTPUTS:              None
 * RETURNS:              0 if the maze is unsolvable, 1 if it is solved
 * SIDE EFFECTS:         Marks maze cells as visited or part of the solution path
 */
int solveMazeDFS(maze_t * maze, int col, int row)
{
  int startColumn = maze->startColumn; //making a local variables equal to start location
  int startRow = maze->startRow;
  if((maze->endColumn == col)&&(maze->endRow == row) ){
    maze->cells[startRow][startColumn]=START; //the start was overwritten and must be restored once the game is over
    return 1;

  }
  if((col>maze->width+1)||(col<0)||(row<0)||(row>maze->height+1)){
    return 0; //if the location is outside of the bounds then return 0
  }

  if(maze->cells[row][col]!=START){
    if((maze->cells[row][col]!=EMPTY)){
      return 0;     //if the location is not an S or an empty space then return 0
    }
  }
  if(maze->cells[row][col]!=WALL&&maze->cells[row][col]!=VISITED){
    maze->cells[row][col]=PATH;   //wherever we move we make a path
}
  if(solveMazeDFS(maze,col-1,row)==1){
    return 1;
  }
  if(solveMazeDFS(maze,col+1,row)==1){
    return 1;                 //if any next move is valid return 1 (comment for every if loop)
  }
  if(solveMazeDFS(maze,col,row-1)==1){
    return 1;
  }
  if(solveMazeDFS(maze,col,row+1)==1){
    return 1;
  }



  maze->cells[row][col]=VISITED;    //make cell we passed through a visited sign



    // Your code here. Make sure to replace following line with your own code.
    return 0;
}
