
//Our code takes in an input from the user for the row number
//and outputs the coefficients for that row in Pascal’s Triangle.
//It uses a for loop with an if and else loop inside, separating
//the conditions for the first coefficient, which is always 1,
//and the rest of the coefficients. It prints out the coefficients
//with a space in between.
//Partner:enr2


#include <stdio.h>
#include <stdlib.h>

int main() {
  int row; //define row variable
  int i; //define i variable
  unsigned long j=1; //define output variable, using long for bigger coefficients
  printf("Enter a row index: "); // display message on screen
  scanf("%d",&row); //take in row number from user

for (i=0; i<=row; i++) // i starts at 0, if less than row number, increase by 1
 {
  if (i==0){
    printf("1"); //first number is always 1
  }
  else{
    j=j*(row+1-i)/i; //follow algorithm for coefficient, multiply new j with previous

    printf(" %lu",j); //print outputs with space before

  }

}
  printf("\n"); //new line after printing code
  return 0;
}
