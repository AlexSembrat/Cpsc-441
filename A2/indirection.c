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
#include <sys/select.h>

int main(){
  //set up a TCP server socket
  int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
  //Address Setup
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(441);
	server_address.sin_addr.s_addr = INADDR_ANY;
  //Bind socket to address
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
  //listen for connections
	listen(server_socket, 5);
  printf("Listening\n");

  //prepare a client Socket
  int client_socket;
  wait:
  client_socket = accept(server_socket,NULL,NULL);
  printf("Client Soccet Accepted\n");

  //prepare the UDP socket
  struct sockaddr_in UDP_address = {0};
  int UDP_socket;

  //message variables
  char client_message[256];
  char to_UDP1[256];
  char to_UDP2[256];
  char to_UDP3[256];
  char from_UDP[256];
  int port = 100;
  int length = 0;

  //ensure the space allocated for variables is empty
  memset(client_message,0,sizeof(client_message));
  memset(to_UDP1,0,sizeof(to_UDP1));
  memset(to_UDP2,0,sizeof(to_UDP2));
  memset(to_UDP3,0,sizeof(to_UDP3));
  memset(from_UDP,0,sizeof(from_UDP));

  while(1){
    //Recive from the client
    length = recv(client_socket,&client_message, sizeof(client_message),0);
    client_message[length] = '\0';
    printf("Message Recived:\n%s\n", client_message);

    //determine which service is being used and connect to that port
    if(strcmp(client_message,"Translator")==0){
      port = 100;
      printf("Port set to Translator\n");
    }
    else if(strcmp(client_message,"Currency_Converter")==0){
      port = 200;
      printf("Port set to Currency_Converter\n");
    }
    else if(strcmp(client_message,"Voting")==0){
      port = 300;
      printf("Port set to Voting\n");
    }
    else if(strcmp(client_message,"quit")==0){
      printf("Cilent Disconnected\n");
      goto wait;
    }

    //set up the UDP client Socket interface
    UDP_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    UDP_address.sin_family = AF_INET;
    UDP_address.sin_port = htons(port);
    UDP_address.sin_addr.s_addr = INADDR_ANY;


    //timeout setup
    struct pollfd fd;
    fd.fd = UDP_socket;
    fd.events = POLLIN;

    //Based on port send data to the requested micro service
    if(port==100){
      length = recv(client_socket,&to_UDP1, sizeof(to_UDP1),0);
      to_UDP1[length] = '\0';
      sendto(UDP_socket, to_UDP1, strlen(to_UDP1),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
      printf("Data sent to UDP translation server\n");
      //recive from micro services
      int size = sizeof(UDP_address);
      if( poll(&fd, 1, 500) ){
      recvfrom(UDP_socket, from_UDP, sizeof(from_UDP), 0,(struct sockaddr*) &UDP_address, &size);
      }
      else{
        send(client_socket,"UDP is unreliable please try again", sizeof("UDP is unreliable please try again"),0);
        printf("Error sent back to client\n");
        close(UDP_socket);
        goto memsets;
      }
      printf("Data recived from UDP server:\n%s\n",from_UDP);

      //close the UDP client
      close(UDP_socket);

      //send back to client
      send(client_socket,from_UDP, sizeof(from_UDP),0);
      printf("Data sent back to client\n");
    }
    else if(port==200){
      //Recive value, and both types of currency
      length = recv(client_socket,&to_UDP1, sizeof(to_UDP1),0);
      to_UDP1[length] = '\0';
      printf("Currently in to_UDP1 %s\n",to_UDP1 );
      length = recv(client_socket,&to_UDP2, sizeof(to_UDP2),0);
      to_UDP2[length] = '\0';
      length = recv(client_socket,&to_UDP3, sizeof(to_UDP3),0);
      to_UDP3[length] = '\0';
      //Send the recived data onto the micro service
      sendto(UDP_socket, to_UDP1, strlen(to_UDP1),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
      sendto(UDP_socket, to_UDP2, strlen(to_UDP2),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
      sendto(UDP_socket, to_UDP3, strlen(to_UDP3),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
      printf("Data sent to UDP Currency Converter server\n");
      //recive from micro services
      int size = sizeof(UDP_address);
      if( poll(&fd, 1, 500) ){
        recvfrom(UDP_socket, from_UDP, sizeof(from_UDP), 0,(struct sockaddr*) &UDP_address, &size);
      }
      else{
        send(client_socket,"UDP is unreliable please try again", sizeof("UDP is unreliable please try again"),0);
        printf("Error sent back to client\n");
        close(UDP_socket);
        goto memsets;
      }
      printf("Data recived from UDP server:\n%s\n",from_UDP);

      //close the UDP client
      close(UDP_socket);

      //send back to client
      send(client_socket,from_UDP, sizeof(from_UDP),0);
      printf("Data sent back to client\n");
    }
    else if(port==300){
      //Recive service request
      length = recv(client_socket,&to_UDP1, sizeof(to_UDP1),0);
      to_UDP1[length] = '\0';

      //Determnine the request
      if(strcmp(to_UDP1,"Secure_Voting")==0){
        /*
        send Secure_Voting to UDP
        recv key
        send key to client
        recv vote from client
        send vote to UDP
        recv confirmation
        close udp socket
        send confirmation
        */
        //Send request to micro service
        int size = sizeof(UDP_address);
        sendto(UDP_socket, to_UDP1, strlen(to_UDP1),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
        //Recive Key OR Polls closed message
        if( poll(&fd, 1, 500) ){
          recvfrom(UDP_socket, from_UDP, sizeof(from_UDP), 0,(struct sockaddr*) &UDP_address, &size);
        }
        else{
          send(client_socket,"UDP is unreliable please try again", sizeof("UDP is unreliable please try again"),0);
          printf("Error sent back to client1\n");
          close(UDP_socket);
          goto memsets;
        }
        //Check if polls are closed, if so goto the end of the loop and send that back to client, close socket
        if(strcmp(from_UDP,"The Polls have closed you can no longer vote. If you wish you may view the results.")==0){
          close(UDP_socket);
          send(client_socket,from_UDP, sizeof(from_UDP),0);
          goto memsets;
        }
        //Send key back to client
        send(client_socket,from_UDP, sizeof(from_UDP),0);
        //Recive encrypted vote
        length = recv(client_socket,&to_UDP2, sizeof(to_UDP2),0);
        to_UDP2[length] = '\0';
        //send vote to micro service
        sendto(UDP_socket, to_UDP2, strlen(to_UDP2),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
        //Clear the recive string
        memset(from_UDP,0,sizeof(from_UDP));
        //Recive vote confirmation
        if( poll(&fd, 1, 500) ){
          recvfrom(UDP_socket, from_UDP, sizeof(from_UDP), 0,(struct sockaddr*) &UDP_address, &size);
        }
        else{
          send(client_socket,"UDP is unreliable please try again", sizeof("UDP is unreliable please try again"),0);
          printf("Error sent back to client\n");
          close(UDP_socket);
          goto memsets;
        }
        //Close socket and send confirmation back to client
        close(UDP_socket);
        send(client_socket,from_UDP, sizeof(from_UDP),0);
      }
      else{
        //Send Summary or Candidates request to voting server
        sendto(UDP_socket, to_UDP1, strlen(to_UDP1),0,(struct sockaddr*) &UDP_address, sizeof(UDP_address));
        printf("Data sent to UDP translation server\n");
        //recive from micro services
        int size = sizeof(UDP_address);
        if( poll(&fd, 1, 500) ){
          recvfrom(UDP_socket, from_UDP, sizeof(from_UDP), 0,(struct sockaddr*) &UDP_address, &size);
        }
        else{
          send(client_socket,"UDP is unreliable please try again", sizeof("UDP is unreliable please try again"),0);
          printf("Error sent back to client\n");
          close(UDP_socket);
          goto memsets;
        }
        printf("Data recived from UDP server:\n%s\n",from_UDP);

        //close the UDP client
        close(UDP_socket);

        //send back to client
        send(client_socket,from_UDP, sizeof(from_UDP),0);
        printf("Data sent back to client\n");
      }
    }

memsets:    //clear strings at the end of each loop
    memset(client_message,0,sizeof(client_message));
    memset(to_UDP1,0,sizeof(to_UDP1));
    memset(to_UDP2,0,sizeof(to_UDP2));
    memset(to_UDP3,0,sizeof(to_UDP3));
    memset(from_UDP,0,sizeof(from_UDP));
  }


}
