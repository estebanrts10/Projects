/*MP10 is supposed to create a sparse matrix through the use of several functions. Ours does not output any matrix, but we believe a good portion of the code is here. In our load, we allocate sp_tuples’ memory as well as read the text file, setting variables to their corresponding values. We call set_tuples from load, which follows the algorithm given; checking if value is zero or not, and either replacing the old value or freeing and recreating a new node entirely. We again allocated memory for the 2nd struct, sp_tuples_node. Gv_tuples gives the value at a specified row and column, and is used by add and mult_tuples. Save_tuples is supposed to print out the matrix, but we somehow overwrote tuples_head and now it’s always null. Destroy_tuples frees the nodes and matrix, and our helper function just frees the node.
Partner: ry6
*/

#include "sparsemat.h"
#include <stdio.h>
#include <stdlib.h>


void delete_node(sp_tuples_node * node); //helper function to free nodes


sp_tuples * load_tuples(char* input_file)
{
  int r,c; //initialize row, col, value variables
  double val;
  sp_tuples * tuple = malloc(sizeof(sp_tuples)); //allocate memory for funct sp_tuples
    FILE * f = fopen(input_file,"r"); //read file
  fscanf(f,"%d %d\n", &tuple->m,&tuple->n); //get m and n from file

  tuple->nz=0; //start off nz at 0
tuple->tuples_head=NULL;
  while(feof(f) != 0){ //if not end of file, initialize row, col, and value
    fscanf(f,"%d %d %lf", &r, &c, &val);
    set_tuples(tuple,r,c,val);


  }


  fclose(f); //close file
  return tuple;
}



double gv_tuples(sp_tuples * mat_t,int row,int col)

{
  int val;
  sp_tuples_node * head=mat_t->tuples_head; //make pointer to tuples_head
  val=head->value; //set val equal to value of head



  while(head!=NULL){ //while value of head is not zero
    if((head->row==row)&&(head->col==col)){ //if on the correct row and column, return value
      return val;
    }
    head=head->next; //increment head to next head of list
  }
  return 0;
}




void set_tuples(sp_tuples * mat_t, int row, int col, double value)

{

    sp_tuples_node * loc = mat_t->tuples_head; //create pointer

    int c=0; //location tracker
    if(mat_t!=NULL){ //make sure matrix isnt zero
  while(loc->next!=NULL){ //while the next location isn't zero
    if(loc->row==row && loc->col==col){ //if you're at the correct row and col
        if(value==0){ //if the value is zero
                    free(loc); //free node from memory
                    c++; //increment location tracker
                    sp_tuples_node * node = malloc(sizeof(sp_tuples_node)); //allocate memory for sp_tuples_node
                  node->row=row; //set node row/col/value to row/col/value in struct
                  node->col=col;
                  node->value=value;
                    node->next = loc->next; //increment pointer
                    mat_t->nz++; //increment nz tracker
                }
        if(c==0){ //if entry already exists
      loc->value=value; //replace old value
          }



    }
  }
}

    //find the correct location for the node within
    //the linked list, it it already exists replace it
    //if it does not exist create a new node

}


void save_tuples(char * file_name, sp_tuples * mat_t)

{
  if(mat_t!=NULL){ //if matrix isnt zero



    FILE * f = fopen(file_name, "w"); //write to file
    fprintf(f, "%d %d", mat_t->m, mat_t->n); //sends values to file
      sp_tuples_node * head = mat_t->tuples_head; //creates pointer


    if(head==NULL){
      printf("could not open");

}
  while(head!=NULL){
        fprintf(f, "%d %d %lf\n", head->row, head->col, head->value);
        head = head->next; //sets pointer to next
    }
    fclose(f);
}
}



sp_tuples * add_tuples(sp_tuples * matA, sp_tuples * matB){
    //Add matrix A and Matrix B with same sizes into matrix C with same
    //size and nz=0
    //for every non-zero entry in A
    if((matA->n==matB->n)&&(matA->m==matB->m)){//checks boundaries
    sp_tuples * matC = malloc(sizeof(sp_tuples_node));//allocate memory
    matC->m = matA->m;//sets matC the same bounds as other 2
    matC->n = matA->n;
    matC->nz = 0;

    sp_tuples_node * node = matA->tuples_head;

    while(node != NULL){ //if node isn't zero
      set_tuples(matC,node->row,node->col,node->value); //call set_tuples
      //call set tuples
      node = node->next;
    }
    node = matB->tuples_head;

    while(node!=NULL){
      //call set tuples
      set_tuples(matC,node->row,node->col,gv_tuples(matB, node->row,node->col));//call set_tuples with matC and matA value
      node = node->next;
    }


    return matC;
  }
  else{
    return NULL;
  }
}



sp_tuples * mult_tuples(sp_tuples * matA, sp_tuples * matB){
  int i,j,k,l;
  if((matA->n==matB->n)&&(matA->m==matB->m)){//checks bounds
  sp_tuples * matC = malloc(sizeof(sp_tuples_node));
  matC->m = matA->m;
  matC->n = matA->n;
  matC->nz = 0;
  for(i=0;i<matA->m;i++){//goes through matrixA
    for(j=0;j<matA->n;j++){
      if(gv_tuples(matA,i,j)==0){//if value is zero, continue
        continue;
      }
      else{
      for(k=0;k<matB->m;k++){//goes through matrixB
        for(l=0;l<matB->n;l++){
          if(gv_tuples(matB,k,l)==0){//if value is zero, continue
            continue;
          }
          if(j==l){//if same bounds
            double value = gv_tuples(matA,i,j)*gv_tuples(matB,k,l);//multiply the 2 matrices
            set_tuples(matC,i,j,value);
          }
        }
      }
    }
    }
  }

return matC;
}
return 0;
}
void delete_node(sp_tuples_node * node){
  free(node); //frees node
}


void destroy_tuples(sp_tuples * mat_t){//frees nodes and matrix itself
  if(mat_t!=NULL){
  sp_tuples_node * head= mat_t->tuples_head;
  sp_tuples_node * rand;
  while(head != NULL){
    rand=head;
    head=head->next;
    delete_node(rand);
  }
  free(mat_t);
}
}
