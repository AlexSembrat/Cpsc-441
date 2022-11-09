#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INF 1000000  //1 million will serve as our version of infinity.
#define NODES 21    //since we only need the algorithm to work for our map of canada we know there is NODES nodes, this makes things simpler.

void dijkstra(int cost_graph[NODES][NODES],char node[NODES][4]);

int main(){
  //used to read in and parse the .txt file (I have the advantage of knowing how long the .txt file and its contents will be so we can set the memory like this)
  char lines[39][15];
  char file[117][5];

  //file input
  char *filename = "canadamap.txt";
  FILE *fp = fopen(filename, "r");

  //read in each line into buffer, the copy into lines array
  int index = 0;
  char buffer[15];
  while(fgets(buffer,15,fp)){
    strcpy(lines[index],buffer);
    //strcat(lines[index],"\0");
    index++;
  }
  //close the file
  fclose(fp);

  //parse each token of data into its own array index (I know each node has a 3 letter name and a distance of 2-4 characters so I did it this way to be lazy and not do a better way.)
  index = 0;
  for(int i = 0; i<118; i = i+3){

    file[i][0] = lines[index][0];
    file[i][1] = lines[index][1];
    file[i][2] = lines[index][2];
    file[i][3] = '\0';

    file[i+1][0] = lines[index][4];
    file[i+1][1] = lines[index][5];
    file[i+1][2] = lines[index][6];
    file[i+1][3] = '\0';

    file[i+2][0] = lines[index][8];
    file[i+2][1] = lines[index][9];
    file[i+2][2] = lines[index][10];

    if(lines[index][10]=='\0' || lines[index][10]=='\n'){
      file[i+2][2] = '\0';
    }
    else{
      file[i+2][2] = lines[index][10];
      file[i+2][3] = '\0';
    }

    if(lines[index][11]=='\0' || lines[index][11]=='\n'){
      file[i+2][3] = '\0';
    }
    else{
      file[i+2][3] = lines[index][11];
      file[i+2][4] = '\0';
    }

    index++;
  }

//Place all unique nodes into an array
char node[NODES][4];
int newNode = 0;
int count = 0;
for(int i = 0; i<117; i = i+3){
  for(int j = 0; j<NODES; j++){
    if(strcmp(file[i],node[j])==0){
      newNode = 0;
      break;
    }
    else{
      newNode = 1;
    }
  }
  if(newNode==1){
    strcpy(node[count],file[i]);
    count++;
  }
  for(int j = 0; j<NODES; j++){
    if(strcmp(file[i+1],node[j])==0){
      newNode = 0;
      break;
    }
    else{
      newNode = 1;
    }
  }
  if(newNode==1){
    strcpy(node[count],file[i+1]);
    count++;
  }
}

  //make a adj matrix with the correct number of nodes and set all to 0
  int graph[count][count];
  for(int i = 0; i<count; i++){
    for(int j = 0; j<count; j++){
      graph[i][j] = 0;
    }
  }

  //populate the graph with the edges
  for(int i = 0; i<117; i = i+3){
    int node1 = -1;
    int node2 = -1;
    for(int j = 0; j<NODES; j++){
      if(strcmp(file[i],node[j])==0){
        node1 = j;
      }
      if(strcmp(file[i+1],node[j])==0){
        node2 = j;
      }
    }
    int cost = atoi(file[i+2]);

    if(node1==-1||node2==-1){
      printf("Node not detected something is wrong program terminated\n");
      exit(0);
    }
    graph[node1][node2] = cost;
    graph[node2][node1] = cost;
  }

  //we now have a graph with the edges so replace all 0's with inf.
  for(int i = 0; i<count; i++){
    for(int j = 0; j<count; j++){
      if(graph[i][j] == 0){
        graph[i][j] = INF;
      }
    }
  }
  printf("Computing The Shortest Path With Dijkstra's algorithm:\n");
  dijkstra(graph,node);
}

//This implemention will assume that the start node is always 0 becuase YYC will be our 0 node, the node array is simply used to print out the airport codes
void dijkstra(int cost_graph[NODES][NODES],char node[NODES][4]){
  int distance[NODES];  //distance to each node from YYC
  int previous[NODES]; // previous nodes
  int visited[NODES];  //boolean array of if we have visited a node or not
  int minimum_distance = INF; //minimum known distance to a node, default to infinity
  int next_node; //the next node

  //initalize distance, previous, and visited
  for(int i = 0; i<NODES; i++){
    distance[i] = cost_graph[0][i]; //This gets the distance from YYC to each other node
    previous[i] = 0; //Zeros the previous nodes
    visited[i] = 0; //Sets all visited nodes to 0 (false), this is used as a boolean value to keep track of if we have visted said nodes.
  }
  distance[0] = 0; //Distance from YYC to YYC is 0
  visited[0] = 1; //We start at YYC so we have visited it.

  for(int number_visited = 1; number_visited<NODES-1; number_visited++){ //go through each of the 20 remaining nodes, once each is finished increment the number visited
    minimum_distance = INF; //default distance is infinity
    for(int i = 0; i<NODES; i++){//go through each node
      if(distance[i]<minimum_distance && visited[i]==0){ //determine the shortest distance through 1 node
        minimum_distance = distance[i];
        next_node = i;
      }
    }
    visited[next_node] = 1; //next node has been visited so mark it as so
    for(int i = 0; i<NODES; i++){ //go through each node again
      if(visited[i]==0){ //Has the node been visited?
        if(minimum_distance+cost_graph[next_node][i]<distance[i]){ //shorter path through the next node?, if so adjust distance and mark next_node as the previous node for node i
          distance[i] = minimum_distance + cost_graph[next_node][i];
          previous[i] = next_node;
        }
      }
    }
  }

    //print the table
    int hops = 0;
    int path[NODES];
    printf("Destination  Cost  Hops  Shortest Path\n");
    for(int i = 1; i<NODES;i++){
      for(int j = 1; j<NODES; j++){
        path[j] = -1;
      }
      path[0] = i;
      //prints the Destination and Cost
      printf("    %s      %d", node[i],distance[i]);

      //counts  the number of hops and fills the path matrix
      hops = 0;
      int current_node = i;
      while(current_node!=0){
        current_node = previous[current_node];
        hops++;
        path[hops] = current_node;
      }
      //prints the number of hops
      if(distance[i]>1000){
        printf("   %d    ", hops);
      }
      else{
        printf("    %d    ", hops);
      }

      //prints the shortest path
      int length = 0;
      for(int j = 0; path[j]!=-1; j++){
        length++;
      }
      for(int j = length-1; j>0; j--){
        printf("%s -> ",node[path[j]]);
      }
      printf("%s",node[path[0]]);

      printf("\n");
    }


}
