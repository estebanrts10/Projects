//Our code replicates the game codebreaker through the use of numbers
//1 through 7. It has three main loops; set_seed, start_game, and make_guess.
// Set_seed takes in a value from the user and generates a pseudo-random
// solution. It does this through the sscanf function, and any
//incorrect seed values will display an error message. If seed
//is accepted, the code moves on to the start_game loop, which
//sets the solution numbers equal to the pseudo-randomly generated
//numbers. It also sets the guess number, which keeps track of
//turns, to 0. Make_guess gets most of the job done. It checks
//if the guesses are perfectly placed or misplaced. It also
//checks if the guess numbers are invalid. At the end of the
//make_guess function, the number of perfectly placed and
//misplaced numbers are printed. If all numbers are perfect or
//the number of tries exceeds five, the game is over.
//partner: ry6


#include <stdio.h>
#include <stdlib.h>
#include "prog5.h"


static int guess_number;
static int solution1;
static int solution2;
static int solution3;
static int solution4;


/*
 * set_seed -- This function sets the seed value for pseudorandom
 * number generation based on the number the user types.
 * The input entered by the user is already stored in the string seed_str by the code in main.c.
 * This function should use the function sscanf to find the integer seed value from the
 * string seed_str, then initialize the random number generator by calling srand with the integer
 * seed value. To be valid, exactly one integer must be entered by the user, anything else is invalid.
 * INPUTS: seed_str -- a string (array of characters) entered by the user, containing the random seed
 * OUTPUTS: none
 * RETURN VALUE: 0 if the given string is invalid (string contains anything
 *               other than a single integer), or 1 if string is valid (contains one integer)
 * SIDE EFFECTS: initializes pseudo-random number generation using the function srand. Prints "set_seed: invalid seed\n"
 *               if string is invalid. Prints nothing if it is valid.
 */
int set_seed (const char seed_str[])
{
    int seed;
    char post[2];
    if ((sscanf(seed_str, "%d%1s", &seed, post))==1){
      srand(seed);
      return 1;

    }
//this whole function checks that the seed is valid numbers, and inputs them into
//a random number generator.


    else{
      printf("set_seed: invalid seed\n");
      return 0;
    }
}

//    Example of how to use sscanf to read a single integer and check for anything other than the integer
//    "int seed" will contain the number typed by the user (if any) and the string "post" will contain anything after the integer
//    The user should enter only an integer, and nothing else, so we will check that only "seed" is read.
//    We declare
//    int seed;
//    char post[2];
//    The sscanf statement below reads the integer into seed.
//    sscanf (seed_str, "%d%1s", &seed, post)
//    If there is no integer, seed will not be set. If something else is read after the integer, it will go into "post".
//    The return value of sscanf indicates the number of items read succesfully.
//    When the user has typed in only an integer, only 1 item should be read sucessfully.
//    Check that the return value is 1 to ensure the user enters only an integer.
//    Feel free to uncomment these statements, modify them, or delete these comments as necessary.
//    You may need to change the return statement below

/*
 * start_game -- This function is called by main.c after set_seed but before the user makes guesses.
 *               This function creates the four solution numbers using the approach
 *               described in the wiki specification (using rand())
 *               The four solution numbers should be stored in the static variables defined above.
 *               The values at the pointers should also be set to the solution numbers.
 *               The guess_number should be initialized to 1 (to indicate the first guess)
 * INPUTS: none
 * OUTPUTS: *one -- the first solution value (between 1 and 7)
 *          *two -- the second solution value (between 1 and 7)
 *          *three -- the third solution value (between 1 and 7)
 *          *four -- the fourth solution value (between 1 and 7)
 * RETURN VALUE: none
 * SIDE EFFECTS: records the solution in the static solution variables for use by make_guess, set guess_number
 */
void
start_game (int* one, int* two, int* three, int* four)
{
    solution1 = *one = rand()%7+1;  //output one
    solution2 = *two = rand()%7+1;  //output two
    solution3 = *three = rand()%7+1;  //output three
    solution4 = *four = rand()%7+1;   //output four
    guess_number = 0;                 //initializing guess number to be 0


}

/*
 * make_guess -- This function is called by main.c after the user types in a guess.
 *               The guess is stored in the string guess_str.
 *               The function must calculate the number of perfect and misplaced matches
 *               for a guess, given the solution recorded earlier by start_game
 *               The guess must be valid (contain only 4 integers, within the range 1-7). If it is valid
 *               the number of correct and incorrect matches should be printed, using the following format
 *               "With guess %d, you got %d perfect matches and %d misplaced matches.\n"
 *               If valid, the guess_number should be incremented.
 *               If invalid, the error message "make_guess: invalid guess\n" should be printed and 0 returned.
 *               For an invalid guess, the guess_number is not incremented.
 * INPUTS: guess_str -- a string consisting of the guess typed by the user
 * OUTPUTS: the following are only valid if the function returns 1 (A valid guess)
 *          *one -- the first guess value (between 1 and 7)
 *          *two -- the second guess value (between 1 and 7)
 *          *three -- the third guess value (between 1 and 7)
 *          *four -- the fourth color value (between 1 and 7)
 * RETURN VALUE: 1 if the guess string is valid (the guess contains exactly four
 *               numbers between 1 and 7), or 0 if it is invalid
 * SIDE EFFECTS: prints (using printf) the number of matches found and increments guess_number(valid guess)
 *               or an error message (invalid guess)
 *               (NOTE: the output format MUST MATCH EXACTLY, check the wiki writeup)
 */
int
make_guess (const char guess_str[], int* one, int* two,
        int* three, int* four)

{
    char post[2];
    int w,i,j,x,y,z;    //initializing every variable
    int skip_array[]={0,0,0,0}; //initializing skip_array, to be used later as a position checker
    int sol_array[]={solution1,solution2,solution3,solution4};
    int p=0, m=0; //p=perfect, m=misplaced, c=try counter
    sscanf(guess_str,"%d%d%d%d%1s",&w, &x, &y, &z, post); //inputing the str values into wxyz
    int guess_array[]={w,x,y,z};    //making array with the values wxyz
    if (w>7||x>7||y>7||z>7){
        printf("make_guess: invalid guess\n");  //checking that the inputs are valud
        return 0;
    }
    if (w<1||x<1||y<1||z<1){
        printf("make_guess: invalid guess\n");
        return 0;// this isnt complete, must call back to place where game is initialized but still missing entry
    }
    *one = w;   //setting *one to w
    *two = x;   //setting position of one to x
    *three = y; //same for y
    *four = z;  //same for z
    guess_number++; //increment guess counter

    for(i=0;i<=3;i++){
      if (guess_array[i]==sol_array[i]){
        skip_array[i] = 1;
        p++;
//first for loop checks for perfect matches
      }
      else{
        for(j=0;j<=3;j++){
          if(skip_array[j]==1){
            continue;
          }
          if (guess_array[i]==sol_array[j])
          {
            m++;
//second for loop checks for misplaced matches
            break; //breaking out of loop so that the
            // misplaced number doesnt get countedrepeatedly(ex. 1111)
          }}}}

      printf("With guess %d, you got %d perfect matches and %d misplaced matches.\n",guess_number,p,m);



    return 1;
}



