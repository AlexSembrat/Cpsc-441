#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define POLLS_CLOSE 11

int main(){
  //setup voting variables
  char candidates[] = "Registered Candidates:\nRichard Smith ID:1\nKevin Nolan ID:2\nJessica Price ID:3\nAmy Kerr ID:4";
  int richard = 1000;
  int kevin = 503;
  int jessica = 982;
  int amy = 1245;
  char summary[256];
  strcpy(summary,"Voting Summary:\nRichard has 1000 votes\nKevin has 503 votes\nJessica has 982 votes\nAmy has 1245 votes");
  char confirm[] = "Your vote has been counted";
  char mustvote[] = "You cannot view results until the polls have closed";//"To view results you must vote first, please try again.";

  //socket setup
  int server_socket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
  //address setup
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(300);
  server_address.sin_addr.s_addr = INADDR_ANY;
  //bind socket to address
  bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  //Allocate Sending/Recived variables
  char from_indirection1[256];
  char from_indirection2[256];
  char from_indirection3[256];
  char to_indirection[256];
  //zero variables
  memset(from_indirection1,0,sizeof(from_indirection1));
  memset(from_indirection2,0,sizeof(from_indirection2));
  memset(from_indirection3,0,sizeof(from_indirection3));
  memset(to_indirection,0,sizeof(to_indirection));

  while(1){

    //recive data from indirection server
    int size = sizeof(server_address);
    recvfrom(server_socket, from_indirection1, sizeof(from_indirection1), 0, (struct sockaddr*) &server_address, &size);
    printf("Recived: %s\n",from_indirection1 );
    //setup time variables
    time_t current_time;
    time(&current_time);
    struct tm *my_time = localtime(&current_time);

    //Depending on request decide on action
    if(strcmp(from_indirection1,"Show_Candidates")==0){
      //Send list of candidates back to indirection server
      sendto(server_socket,candidates,strlen(candidates),0,(struct sockaddr*) &server_address,size);
      printf("Candidate Data sent back to indirection\n");
    }
    else if(strcmp(from_indirection1,"Voting_Summary")==0){
      //check if the polls have closed
      if(my_time->tm_hour >= POLLS_CLOSE){//if(richard!=1000||kevin!=503||jessica!=982||amy!=1245){
        sendto(server_socket,summary,strlen(summary),0,(struct sockaddr*) &server_address,size);
        printf("Summary Data sent back to indirection\n");
      }
      else{
        //the polls have not closed send back that information
        sendto(server_socket,mustvote,strlen(mustvote),0,(struct sockaddr*) &server_address,size);
      }
    }
    else if(strcmp(from_indirection1,"Secure_Voting")==0){
      if(my_time->tm_hour >= POLLS_CLOSE ){
        //the polls have closed send back that info and skip the rest of this else if statement
        sendto(server_socket,"The Polls have closed you can no longer vote. If you wish you may view the results.",strlen("The Polls have closed you can no longer vote. If you wish you may view the results."),0,(struct sockaddr*) &server_address,size);
        goto memsets;
      }
      /*
      generate key
      send key
      recv encrypt
      send confirmation
      decrypt
      tally votes
      */
      int key = rand() % 9; //random number between 0 and 8
      key++;                //now a random number between 1 and 9
      printf("Key is: %d\n",key);
      sprintf(to_indirection,"%d",key);
      //Send key back to indirection server
      sendto(server_socket,to_indirection,strlen(to_indirection),0,(struct sockaddr*) &server_address,size);
      //Recive encrypted vote from indirection server
      recvfrom(server_socket, from_indirection2, sizeof(from_indirection2), 0, (struct sockaddr*) &server_address, &size);
      printf("Vote recivedis %s\n", from_indirection2);
      //Send confirmation of arrival
      sendto(server_socket,confirm,strlen(confirm),0,(struct sockaddr*) &server_address,size);

      //decrypt the key and tally the vote
      int id = atoi(from_indirection2) / key;
      printf("ID %d voted for\n", id );

      if(id==1){
        richard++;
      }
      else if(id==2){
        kevin++;
      }
      else if(id==3){
        jessica++;
      }
      else if(id==4){
        amy++;
      }

      //Update the summary string
      char rvotes[256];
      sprintf(rvotes,"%d",richard);
      char kvotes[256];
      sprintf(kvotes,"%d",kevin);
      char jvotes[256];
      sprintf(jvotes,"%d",jessica);
      char avotes[256];
      sprintf(avotes,"%d",amy);

      memset(summary,0,sizeof(summary));

      strcat(summary,"Voting Summary:\nRichard has ");
      strcat(summary,rvotes);
      strcat(summary," votes\nKevin has ");
      strcat(summary,kvotes);
      strcat(summary," votes\nJessica has ");
      strcat(summary,jvotes);
      strcat(summary," votes\nAmy has ");
      strcat(summary,avotes);
      strcat(summary," votes");

      memset(rvotes,0,sizeof(rvotes));
      memset(kvotes,0,sizeof(kvotes));
      memset(jvotes,0,sizeof(jvotes));
      memset(avotes,0,sizeof(avotes));

    }

memsets:    //reset the strings used
    memset(from_indirection1,0,sizeof(from_indirection1));
    memset(from_indirection2,0,sizeof(from_indirection2));
    memset(from_indirection3,0,sizeof(from_indirection3));
    memset(to_indirection,0,sizeof(to_indirection));
  }

}
