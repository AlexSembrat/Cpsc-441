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

void translate(char from[], char to[]);

int main(){
  //socket setup
  int server_socket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
  //address setup
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(100);
  server_address.sin_addr.s_addr = INADDR_ANY;
  //bind socket to address
  bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  //Setup send/recive variables
  char from_indirection[256];
  char to_indirection[256];
  //zero variables
  memset(from_indirection,0,sizeof(from_indirection));
  memset(to_indirection,0,sizeof(to_indirection));

  while(1){

    //recive data from indirection server
    int size = sizeof(server_address);
    recvfrom(server_socket, from_indirection, sizeof(from_indirection), 0, (struct sockaddr*) &server_address, &size);
    printf("Data recived from indirection server:\n%s\n", from_indirection);

    //do the service
    translate(from_indirection,to_indirection);

    //send data from indirection server
    sendto(server_socket,to_indirection,strlen(to_indirection),0,(struct sockaddr*) &server_address,size);
    printf("Data sent back to indirection\n");

    //reset the strings used
    memset(from_indirection,0,sizeof(from_indirection));
    memset(to_indirection,0,sizeof(to_indirection));
  }

}

void translate(char from[], char to[]){
//Find the enlgish word and copy the translation into the return string
  if(strcmp(from,"Hello")==0){
    strcpy(to,"Bonjour");
    to[7] = '\0';
  }
  else if(strcmp(from,"Coffee")==0){
    strcpy(to,"Cafe");
    to[4] = '\0';
  }
  else if(strcmp(from,"Computer")==0){
    strcpy(to,"Ordinateur");
    to[10] = '\0';
  }
  else if(strcmp(from,"Food")==0){
    strcpy(to,"Nourriture");
    to[10] = '\0';
  }
  else if(strcmp(from,"Network")==0){
    strcpy(to,"Reseau");
    to[6] = '\0';
  }

}
