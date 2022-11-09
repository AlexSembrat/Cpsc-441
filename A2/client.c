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

int main(){

  //request indirection server IP and Port
  char IP[30];
  char porty[30];
  printf("Enter the IP of the indirection server\n");
  scanf("%s",IP);
  printf("Enter the Port of the indirection server\n");
  scanf("%s",porty);
  int port = atoi(porty);

  //socket setup
  int Socket;
	Socket = socket(AF_INET, SOCK_STREAM, 0);

	if(Socket==-1){
		printf("Socket Call Failed\n");
	}

  //server address setup
	struct sockaddr_in indirection_address;
	indirection_address.sin_family = AF_INET;
	indirection_address.sin_port = htons(port);
	indirection_address.sin_addr.s_addr = inet_addr(IP); //INADDR_ANY;

  //connect to server
	int status = connect(Socket, (struct sockaddr *) &indirection_address, sizeof(indirection_address));

  if(status==-1){
		printf("Connection Failed\n");
	}
  else{
    printf("Connection Initialized\n");
  }

  //Setup message variables
  char my_message[256];
	char server_message[256];
  char arg1[256];
  char arg2[256];
  char arg3[256];
  //ensure the space allocated for variables is empty
  memset(my_message,0,sizeof(my_message));
  memset(server_message,0,sizeof(server_message));
  memset(arg1,0,sizeof(arg1));
  memset(arg2,0,sizeof(arg2));
  memset(arg3,0,sizeof(arg3));

  while(1){
    //Request input
    printf("Please enter your command. You may choose from Translator, Currency_Converter, Voting, or quit.\n");
    scanf("%s", my_message);
    my_message[strlen(my_message)] = '\0';

    //Disconnect and end program if quit
    if(strcmp(my_message,"quit")==0){
      printf("Thanks for using our micro services, the client will now close.\n");
      send(Socket, my_message, strlen(my_message), 0);
      close(Socket);
      return 0;
    }

    //Find the command user requested
    if(strcmp(my_message,"Translator")==0){
      //Send translator request to indirection server
      send(Socket, my_message, strlen(my_message), 0);
      printf("Enter the word you wish to translate. We support Hello, Coffee, Computer, Food, and Network.\n");
      scanf("%s", arg1);
      //Send word translation request to indirection server
      send(Socket, arg1, strlen(arg1), 0);
      //recive the translated word or a faliure message
      recv(Socket, &server_message, sizeof(server_message),0);
      if(strcmp(server_message,"UDP is unreliable please try again")==0){
        printf("%s\n",server_message );
        goto memsets;
      }
      //Display the translated word.
      printf("The French translation is: %s\n", server_message);
    }
    else if(strcmp(my_message,"Currency_Converter")==0){
      //Send currency converter request to indirection server
      send(Socket, my_message, strlen(my_message), 0);
      printf("Enter the value of currency you wish to convert in an integer format. (i.e: 1,2,3)\n");
      scanf("%s", arg1);
      //send value of currency
      send(Socket, arg1, strlen(arg1), 0);
      printf("Enter the type of currency you wish to convert from. We support CAD,USD,EUR,GBP,XBT(bitcoin))\n");
      scanf("%s", arg2);
      //send current currency
      send(Socket, arg2, strlen(arg2), 0);
      printf("Enter the type of currency you wish to convert to. We support CAD,USD,EUR,GBP,XBT(bitcoin))\n");
      scanf("%s", arg3);
      //send currency to conver to
      send(Socket, arg3, strlen(arg3), 0);
      //Recive the converted currency or error
      recv(Socket, &server_message, sizeof(server_message),0);
      if(strcmp(server_message,"UDP is unreliable please try again")==0){
        printf("%s\n",server_message );
        goto memsets;
      }
      //display the conversion
      printf("Currency Data Recived:\n%s\n", server_message);
    }
    else if(strcmp(my_message,"Voting")==0){
      //Send voting reqest to indirection server
      send(Socket, my_message, strlen(my_message), 0);
      //Get input for the type of voting request
      printf("Enter voting command you wish to perform. We support Show_Candidates, Secure_Voting, and Voting_Summary\n");
      scanf("%s", arg1);
      //Find voting request
      if(strcmp(arg1,"Secure_Voting")==0){
        /*
        send Secure_Voting
        recv key
        scanf(uservote)
        encrypt = vote*key
        send encrypt
        recv vote_success
        printf(vote_success);
        */
        //Send secure voting request
        send(Socket, arg1, strlen(arg1), 0);
        //Recive encyption key, error, or polls have closed message.
        recv(Socket, &server_message, sizeof(server_message),0);
        if(strcmp(server_message,"UDP is unreliable please try again")==0){
          printf("%s\n",server_message );
          goto memsets;
        }
        else if(strcmp(server_message,"The Polls have closed you can no longer vote. If you wish you may view the results.")==0){
          printf("%s\n", server_message);
          goto memsets;
        }
        //Convert key to int
        int key = atoi(server_message);
        //Ask for user vote
        printf("Enter the integer ID of who you wish to vote for.\n");
        scanf("%s", arg2);
        //encrypt vote and format in string
        int vote = atoi(arg2);
        vote = vote*key;
        sprintf(arg3,"%d",vote);
        //Send vote back to indirection server
        send(Socket, arg3, strlen(arg3), 0);
        //clear server message variable
        memset(server_message,0,sizeof(server_message));
        //recive vote confirmation or error and display
        recv(Socket, &server_message, sizeof(server_message),0);
        if(strcmp(server_message,"UDP is unreliable please try again")==0){
          printf("%s\n",server_message );
          goto memsets;
        }
        printf("%s\n",server_message );
      }
      else{
        //send request to server (candidates or summary)
        send(Socket, arg1, strlen(arg1), 0);
        //Recive list of candidates or summary. (Possible error)
        recv(Socket, &server_message, sizeof(server_message),0);
        if(strcmp(server_message,"UDP is unreliable please try again")==0){
          printf("%s\n",server_message );
          goto memsets;
        }
        //display 
        printf("%s\n", server_message);
      }
    }

memsets:
    memset(my_message,0,sizeof(my_message));
    memset(server_message,0,sizeof(server_message));
    memset(arg1,0,sizeof(arg1));
    memset(arg2,0,sizeof(arg2));
    memset(arg3,0,sizeof(arg3));
  }


}
