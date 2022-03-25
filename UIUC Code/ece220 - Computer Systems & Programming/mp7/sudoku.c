/*Our mp7 solves sudoku given an unsolved instance, with zeroes representing
numbers we need filled. It does this through 4 checker functions; for row, column,
3x3 grid, and if those three conditions are all met. It then has a solve function
that completes the puzzle through recursion, looping until every zero is filled and
every value works in conjunction with each other.
partner: ry6


*/

#include "sudoku.h"


// Function: is_val_in_row
// Return true if "val" already existed in ith row of array sudoku.

int is_val_in_row(const int val, const int i, const int sudoku[9][9]) {

  assert(i>=0 && i<9);
    int z;
    for (z=0;z<9;z++){    // goes through every column in a given row

        if(val==sudoku[i][z]){
            return 1; // return 1 if value is already inside the row
        }
    }


  return 0; //return 0 if value is valid

}

// Function: is_val_in_col
// Return true if "val" already existed in jth column of array sudoku.
int is_val_in_col(const int val, const int j, const int sudoku[9][9]) {

  assert(j>=0 && j<9);
    int z;
    for (z=0;z<9;z++){  //goes through every row in a given column
        if(val==sudoku[z][j]){
            return 1; //return 1 if value is already inside the column
        }
    }


  return 0; //return 0 if value is valid

}

// Function: is_val_in_3x3_zone
// Return true if val already existed in the 3x3 zone corresponding to (i, j)
int is_val_in_3x3_zone(const int val, const int i, const int j, const int sudoku[9][9]) {

  assert(i>=0 && i<9);
  int n,z,x,y;
  n = i % 3;  //find remainder of row/3
  z = j % 3; //find remainder of column/3
  n = i - n; // find lower bound of row
  z = j - z; //find lower bound of column

  for (x=0;x<3;x++){ //goes through 3 row values
    for(y=0;y<3;y++){ //goes through 3 column values
        if(val == sudoku[n+x][z+y]){ //compares val with all 9 numbers in the corresponding 3x3 grid
            return 1; //return 1 if val is inside the grid
        }
    }
  }


  return 0;

}

// Function: is_val_valid
// Return true if the val is can be filled in the given entry.
int is_val_valid(const int val, const int i, const int j, const int sudoku[9][9]) {

  assert(i>=0 && i<9 && j>=0 && j<9);
    if((is_val_in_3x3_zone(val,i,j,sudoku)==1)|| //if any of the above conditions are false, return 0
       (is_val_in_col(val,j,sudoku)==1)||
       (is_val_in_row(val,i,sudoku)==1)){
        return 0;
    }

  return 1;

}

// Procedure: solve_sudoku
// Solve the given sudoku instance.
int solve_sudoku(int sudoku[9][9]) {


    int i,j,x,z,n=0,val;
    for(x=0;x<9;x++){ //goes through all rows
      for(z=0;z<9;z++){ //goes through all columns
        if (sudoku[x][z]==0){ //if val is 0, find the row and col values
          n=1; //counter to determine if we've gone through all zeroes
          i=x;
          j=z;
        }
      }
    }
    if (n==0){ //if there are no more zeroes, return 1
      return 1;
    }

    for(val=1;val<=9;val++){ //given 9 values to fill the 0 space
            if(is_val_valid(val,i,j,sudoku)==1){ //if val is valid, replace 0 with val
            sudoku[i][j] = val;
            if(solve_sudoku(sudoku)){ //if every val works with every other space, return 1
                return 1;
            }
            sudoku[i][j]=0; //sets space back to 0 if not all conditions are met
        }
    }

  return 0;
  // END TODO.
}





// Procedure: print_sudoku
void print_sudoku(int sudoku[9][9])
{
  int i, j;
  for(i=0; i<9; i++) {
    for(j=0; j<9; j++) {
      printf("%2d", sudoku[i][j]);
    }
    printf("\n");
  }
}

// Procedure: parse_sudoku
void parse_sudoku(const char fpath[], int sudoku[9][9]) {
  FILE *reader = fopen(fpath, "r");
  assert(reader != NULL);
  int i, j;
  for(i=0; i<9; i++) {
    for(j=0; j<9; j++) {
      fscanf(reader, "%d", &sudoku[i][j]);
    }
  }
  fclose(reader);
}
