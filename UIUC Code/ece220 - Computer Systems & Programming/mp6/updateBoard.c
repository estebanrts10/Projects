/*
In our code, we programmed the game of life. The game follows a set of rules to determine
if a cell, denoted by 0 or 1, is dead or alive respectively. There are 3 main
functions that we wrote, countliveneighbor, updateboard, and alivestable. By utilizing
a 1D array to mimic the 2D board and a pointer to find every position in the array, we
found how many alive neighbors are next to each position. Next, we checked to see if any
updating was required in alivestable. Finally, we updated the board based on the rules
specified, making sure to update from the original board and not the updated value for
each step.
partner: ry6
*/







int countLiveNeighbor(int* board, int boardRowSize, int boardColSize, int row, int col){

int i,j,live_count=0;				//initiating variables and lives counter
for(i=row-1;i<=row+1;i++){		//for loop that goes through the three rows that are around the spot we are at
	for(j=col-1;j<=col+1;j++){ 	//this does the same thing but for columns
		if (i<0||j<0||i>=boardRowSize||j>=boardColSize){	//if we are checking the edges
			continue;									//we make sure we are not checking the wrong spaces
		}
		if(board[i*boardColSize+j]==1){		//checks the spot to see if it contains a 1 or 0
			live_count++;			//if it has a 1, increment live_count
		}

	}
}
if(board[row*boardColSize+col]==1){		//if the original cell was already alive,
	live_count--;											//this means we added it to live counter, so decrement it now
}
return live_count;

}


void updateBoard(int* board, int boardRowSize, int boardColSize) {
int row, col, live_count;						//initialize variables
int tboard[boardRowSize*boardColSize];		//create a new board that will
int i;																	//be updated instead of the original

for(i=0;i<boardRowSize*boardColSize;i++) {	//goes through the cells in the original board
	tboard[i] = board[i];								//and sets the new board equal to the original
}

for(row=0;row<boardRowSize;row++){		//goes through every cell in the board and then goes to
	for(col=0;col<boardColSize;col++){	//countLiveNeighbor and checks for the number of live neighbors
		live_count = countLiveNeighbor(board,boardRowSize,boardColSize,row,col);
		if(board[row*boardColSize+col]==1){		//if the cell we are looking at is alive
				if((live_count<2)||(live_count>3)){	//if its less than 2 or greater than 3
					tboard[row*boardColSize+col]=0;		//it dies
				}																		//otherwise it stays alive
		}
		else{
			if (live_count==3){									//if the live count is 3,
			tboard[row*boardColSize+col]=1;		//that cell becomes a 1
		}
	}
}
}
for(i=0;i<boardRowSize*boardColSize;i++){		//once done changing all the cells in the new board
	board[i] = tboard[i];							//set board equal to tboard in order to finalize changes
}
}

/*
aliveStable does the same thing as update board but instead of changing the values
of the cells, just checks if there are any changes to be done,
if there are changes to be done, return 0,  otherwise return 1
if this function returns 1, the game is over.
*/

int aliveStable(int* board, int boardRowSize, int boardColSize){
	int row, col, live_count;														//initialize variables
	for(row=0;row<boardRowSize;row++){							//goes through the board
		for(col=0;col<boardColSize;col++){
			live_count = countLiveNeighbor(board,boardRowSize,boardColSize,row,col);
			if(board[row*boardColSize+col]==1){				//checks if there are less than 2
					if((live_count<2)||(live_count>3)){		//or greater than 3 neighbors
						return 0;													//if there are, it dies and return 0
					}
			}
			else{
				if (live_count==3){									//if a cell is born(livecount=3) return 0
				return 0;
			}

		}
		}
	}
return 1;														//otherwise return 1
}
