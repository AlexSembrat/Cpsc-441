#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define K 10 //set what K is equal to
#define M 1024 //set what M is equal to

int contains(int want, int cart[]);
//int level(int node);
//int counter(int start, int end, int tree[]);

int main(){

  //File input
  char *filename = "customer10.txt";
  FILE *file = fopen(filename,"r");

  //File error checking
  if(file==NULL){
    printf("Failiure file cant be opened\n");
    return 0;
  }

  //Variables and buffer to read the file into
  char item[8];
  char cart[M][8];
  char cart1[M][8];
  int cart_int[M];
  memset(cart,0,sizeof(cart));
  memset(cart1,0,sizeof(cart1));

  //File input, read each line into item, them copy into cart
  int i = 0;
  int count = 0;
  while(fgets(item,8,file)){
    strcpy(cart[i],item);
    i++;
    count++;
  }

  //close the file
  fclose(file);

  //Check if the basket is empty
  //printf("The count is %d\n", count);
  if(count==0){
    printf("The basket is empty.\n");
    return 0;
  }

/*  //Print
  for(int i = 0; i<M; i++){
    printf("%s",cart[i]);
  }
*/

  //Copy cart into cart1, removing any \n's
  for(int i = 0; i<M; i++){
    for(int j = 0; j<6; j++){
      if(cart[i][j] != '\n'){
        cart1[i][j] = cart[i][j];
      }
    }
  }

  //copy cart1 into cart_int using atoi
  for(int i = 0; i<M; i++){
    cart_int[i] = -1;
    cart_int[i] = atoi(cart1[i]);
  }

  //change any extra 0's to -1
  for(int i = 1; i<M; i++){
    if(cart_int[i]==0){
      cart_int[i] = -1;
    }
  }

/*  //Print
  printf("Int Version:\n");
  for(int i = 0; i<M; i++){
    printf("%d\n",cart_int[i]);
  }
*/

  //Create Array Binary Tree
  int tree[M+M-1];
  for(int i = 0; i<M+M-1; i++){
    tree[i] = 0;
  }

  //the 0 of the leaf of the tree will be at tree[M-1];
  for(int i = M-1; i<M+M-1; i++){
    if(contains(i-M+1,cart_int)==1){ //if cart_int contains i-M-1 then tree[i] = i-M-1
      tree[i] = 1;
    }
  }

/*  //print
  printf("The Tree\n");
  for(int i = 0; i<M+M-1; i++){
    printf("%d\n",tree[i]);
  }
*/

  //THE MAC PROTOCOL

  //to count probes and collisions
  int probes = 0;
  int collisions = 0;

  for(int level = K; level>0; level--){   //go through each level starting at the bottom
    for(int i = pow(2,level)-1; i<2*pow(2,level)-1; i+=2){      //calculate the bounds of each level and go through 2 at a time
      if(tree[i]>0 && tree[i+1]>0){
        //set the parent node to 1
        //set both children to 1
        //collisions++;
        tree[(i-1)/2] = 1;
        collisions++;
        tree[i] = 1;
        tree[i+1] = 1;
      }
      else if(tree[i]==0 && tree[i+1]==0){
        //set the parent node to 0;
        //zero both children
        tree[(i-1)/2] = 0;
      }
      else{
        //set the parent node to 1
        //zero both children
        tree[(i-1)/2] = 1;
        tree[i] = 0;
        tree[i+1] = 0;
      }
    }
  }

  //sum each node in the tree to get the number of probes
  for(int i = 0; i<M+M-1; i++){
    probes = probes + tree[i];
  }

  //calculate efficiency
  float efficiency = (1 - (float)collisions/(float)probes)*100;

  //print the results
  printf("This customer had %d items.\nIt took %d probes with %d collisions to scan for an efficiency of %.2f%%\n", count,probes,collisions,efficiency);

  return 0;
}

int contains(int want, int cart[]){
  for(int i = 0; i<M; i++){
    if(cart[i]==want){
      return 1;
    }
  }
  return 0;
}

/*int level(int node){
  return floor(log2(node+1));
}

int counter(int start, int end, int tree[]){
  int counter = 0;
  for(int i = start; i<=end; i++){
    counter++;
  }
  return counter;
}
*/
