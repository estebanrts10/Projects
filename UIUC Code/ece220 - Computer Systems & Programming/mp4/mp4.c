//Our code prints out every number between
//two number specified by the user that is a
//semiprime. It does this through a main loop
//that calls the semiprime function, which in turn
 //uses the prime function to determine which numbers
 //are semiprime. It does this with conditions
 //set in the for loop, and returns 1 if successful,
 //0 if not.
//partner ry6
int is_prime(int k);
int print_semiprimes(int a, int b);


int main(){
   int a, b;                //initializing a and b as ints (range of numbers)
   printf("Input two numbers: ");
   scanf("%d %d", &a, &b);        //a and b are input
   if( a <= 0 || b <= 0 ){            // if either is a 0 or negative then don’t run
     printf("Inputs should be positive integers\n");
     return 1;                //end, print top line, and return 1
   }

   if( a > b ){                //the range has to be increasing
     printf("The first number should be smaller than or equal to the second number\n");
     return 1;
   }
    return print_semiprimes(a, b);    //after the input is done and the range
//is checked to be valid, go to print_semiprimes

 }



   int is_prime(int n)            //function that checks if number is prime
   {
       int m;                //m is an int
    for(m=2; m<n; m++){        //for loop that divides the input number by every number
        if ((n%m)==0){            //that is smaller than that and checks if it has a remainder
            return 0;            //if it doesn’t have a remainder it is not prime, return 0
        }

        }
        return 1;                //otherwise return 1
}



    int print_semiprimes(int a, int b){
        int i,k, l=0;            //initialize i k and l as ints, l as 0




        for (i=a; i<=b; i++)        //first for loop goes through every number between a,b
        {
            for (k=2; k<=i-1; k++)        //count up from 2 to i-1 for division check later

            {

                if ((is_prime(k)==1)&&(is_prime(i/k)==1)&&(is_prime(i)==0)&&((i%k)==0))
                 {                //check if k is prime, check if i/k is prime, check if
                    //make sure i isnt prime, and i/k must be whole #
                     printf("%d ", i);        //if all the above statements are correct then print
                     l++;
                     break;            //break is needed so it does not print the num twice

                 }

            }
        }
        if (l!=0)                //l counts up every time that we print a number
        {                    //this checks to see if we have printed anything
            return 1;            //if we have, return 1 otherwise return 0
        }
        return 0;



}
