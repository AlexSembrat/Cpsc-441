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

void convert(char value[],char old[], char new[], char final[]);

int main(){
  //Socket setup
  int server_socket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
  //address setup
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(200);
  server_address.sin_addr.s_addr = INADDR_ANY;
  //bind socket to address
  bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  //Setup send/recive variables
  char from_indirection1[256];
  char from_indirection2[256];
  char from_indirection3[256];
  char to_indirection[256];
  //Zero the variables at the start
  memset(from_indirection1,0,sizeof(from_indirection1));
  memset(from_indirection2,0,sizeof(from_indirection2));
  memset(from_indirection3,0,sizeof(from_indirection3));
  memset(to_indirection,0,sizeof(to_indirection));

  while(1){

    //recive data from indirection server
    int size = sizeof(server_address);
    recvfrom(server_socket, from_indirection1, sizeof(from_indirection1), 0, (struct sockaddr*) &server_address, &size);  //amount
    recvfrom(server_socket, from_indirection2, sizeof(from_indirection2), 0, (struct sockaddr*) &server_address, &size);  //current currency
    recvfrom(server_socket, from_indirection3, sizeof(from_indirection3), 0, (struct sockaddr*) &server_address, &size);  //currency to convert to
    printf("Data recived from indirection server:\n%s\n", from_indirection1);
    printf("Data recived from indirection server:\n%s\n", from_indirection2);
    printf("Data recived from indirection server:\n%s\n", from_indirection3);

    //do the service
    convert(from_indirection1,from_indirection2,from_indirection3,to_indirection);

    //send data from indirection server
    sendto(server_socket,to_indirection,strlen(to_indirection),0,(struct sockaddr*) &server_address,size);
    printf("Data sent back to indirection\n");

    //reset the strings used
    memset(from_indirection1,0,sizeof(from_indirection1));
    memset(from_indirection2,0,sizeof(from_indirection2));
    memset(from_indirection3,0,sizeof(from_indirection3));
    memset(to_indirection,0,sizeof(to_indirection));
  }

}

void convert(char money[],char old[], char new[], char final[]){

int value = atoi(money);
float converted = 0;
//find current currency and the currency you want to convert.
  if(strcmp(old,new)==0){
    final = "Your currency is worth the same as you tried to convert it to its original type.";
    final[strlen(final)] = '\0';
    return;
  }
  else if(strcmp(old,"CAD")==0){

    if(strcmp(new,"USD")==0){
      converted = value * 0.81;
    }
    else if(strcmp(new,"EUR")==0){
      converted = value * 0.70;
    }
    else if(strcmp(new,"GBP")==0){
      converted = value * 0.59;
    }
    else if(strcmp(new,"XBT")==0){
      converted = value * 0.000013;
    }

  }
  else if(strcmp(old,"USD")==0){

    if(strcmp(new,"CAD")==0){
      converted = value * 1.24;
    }
    else if(strcmp(new,"EUR")==0){
      converted = value * 0.86;
    }
    else if(strcmp(new,"GBP")==0){
      converted = value * 0.73;
    }
    else if(strcmp(new,"XBT")==0){
      converted = value * 0.0000157;
    }

  }
  else if(strcmp(old,"EUR")==0){

    if(strcmp(new,"USD")==0){
      converted = value * 1.16;
    }
    else if(strcmp(new,"CAD")==0){
      converted = value * 1.44;
    }
    else if(strcmp(new,"GBP")==0){
      converted = value * 0.84;
    }
    else if(strcmp(new,"XBT")==0){
      converted = value * 0.000018;
    }

  }
  else if(strcmp(old,"GBP")==0){

    if(strcmp(new,"USD")==0){
      converted = value * 1.38;
    }
    else if(strcmp(new,"EUR")==0){
      converted = value * 1.18;
    }
    else if(strcmp(new,"CAD")==0){
      converted = value * 1.70;
    }
    else if(strcmp(new,"XBT")==0){
      converted = value * 0.0000216;
    }

  }
  else if(strcmp(old,"XBT")==0){

    if(strcmp(new,"USD")==0){
      converted = value * 63776.32;
    }
    else if(strcmp(new,"EUR")==0){
      converted = value * 54803.75;
    }
    else if(strcmp(new,"GBP")==0){
      converted = value * 46285.67;
    }
    else if(strcmp(new,"CAD")==0){
      converted = value * 78803.09;
    }

  }
//Format return string, 2 decimal palces.
char rounded[256];
sprintf(rounded,"%.2f",converted);

strcpy(final,money);
strcat(final," ");
strcat(final,old);
strcat(final," is equal to ");
strcat(final, rounded);
strcat(final," ");
strcat(final,new);
strcat(final,".");

}
