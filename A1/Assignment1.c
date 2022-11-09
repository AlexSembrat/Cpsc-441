#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){

	//open server listening socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8001);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	
	listen(server_socket, 5);
	
	int client_socket;
	
	printf("Listening Socket Open\n");
	
	//prepare new web server client socket
	int web_socket;
	struct sockaddr_in web_address;
	
	//declare variables to send and receive data
	char client_message[10000];
	char web_request[10000];
	char web_response[1500000];
	char *temp;
	char url[1000];
	char host[1000];		
	char path[1000];
	char error_path[] = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error.html ";
	char error_path1[] = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error1.html ";
	char blocked_list[10][1000];
	int total_blocked = 0;			

	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
	char blocker[100];
	while(1){
		
		
		if( poll(&mypoll, 1, 10) ){
		scanf("%s", blocker);
		}
		else{
			printf("No new blocks\n");
		}
		
		if(strcmp(blocker,"block")==0){
			//printf("Enter the word you would like to block.\n");
			scanf("%s", blocked_list[total_blocked]);
		//	for(int i = 0; blockee!=NULL; i++){
		//		blocked_list[total_blocked][i] = *blockee;
		//		blockee++;
		//	}
			printf("%s is now blocked.\n", blocked_list[total_blocked]);
			total_blocked++;
		}
		
		if(!strcmp(blocker,"unblock") && total_blocked>0){
			total_blocked--;
			printf("%s is the most recent in blocked\n", blocked_list[total_blocked]);
			for(int i = 0; blocked_list[total_blocked][i] != '\0'; i++){
				blocked_list[total_blocked][i] = '\0';
			}
			printf("The most recent block has been UNBLOCKED.\n");
		}
		else if(!strcmp(blocker,"unblock") && total_blocked==0){
			printf("Nothing to UNBLOCK\n");
		}
		
		//accept client socket
		client_socket = accept(server_socket, NULL, NULL); 
		printf("\nAccepted Client Socket\n");
		recv(client_socket, &client_message, sizeof(client_message), 0); //receive the data that must be parsed
		printf("\nReceived Data From Client Socket:\n");
		
			//sometimes the socket will connect but because the data was already cached it wont send anything
			//if thats the case we just skip to the end of the loop and reset to wait for another connection
		if(client_message[0] == '\0'){
			//printf("NOTHING IN CMESSAGE\n");
			goto cleanUp;
		}
		printf("This is client message:\n%s\n", client_message);
		
		//parse data
		//copy the client message into web request
		for(int i = 0; client_message[i]!='\0'; i++){
			web_request[i] = client_message[i];					//THIS WAS HERE
		}
		
		//get the url
		temp = strstr(web_request, "http:");
		if(temp!=NULL){
			for(int i = 0; *temp!=' '; i++){
				url[i] = *temp;
				temp++;
			}
			printf("URL is: %s\n", url);
		}
		
		//get the host
		for(int i = 0; url[i+7]!='/'; i++){
			host[i] = url[i+7];
		}
		printf("Host is: %s\n", host);

	
		//get the Path
		temp = strstr(url, "//");
		if(temp!=NULL){
			temp = temp + 2;
			while(*temp!='/'){
				temp++;
			}
			for(int i = 0; *temp!=' '; i++){
				path[i] = *temp;
				temp++;
			}
			printf("Path is: %s\n", path);
		}
		
		//redirect to an error if a blocked word is found.
		for(int i = 0; i<total_blocked; i++){
			if(blocked_list[i]!=NULL){
				if(strstr(path,blocked_list[i])!=NULL){
					
					memset(web_request,0,sizeof(web_request));
					strcpy(web_request, error_path);
					temp = strstr(client_message, "HTTP/");
					strcat(web_request,temp);
					
					printf("\nA blocked word was detected the web request was changed to the following\n");
					printf("%s\n", web_request);
					
				}
			}
		}
	
		//give web server address
		printf("Server Addressing\n");
		web_socket = socket(AF_INET, SOCK_STREAM, 0);
		web_address.sin_family = AF_INET;
		web_address.sin_port = htons(80);
		inet_aton("136.159.2.17", &web_address.sin_addr.s_addr); //I hard coded this because copying the method from the slides gave me some random IP address and I couldn't figure it out.
		
		//connect to web server
		connect(web_socket, (struct sockaddr*) &web_address, sizeof(web_address));
		printf("Connected To Web\n");
		
		//send request
		send(web_socket, web_request, sizeof(web_request),0);
		printf("Sent Data\n\n");
		
		//receive response and close web socket
		//recv(web_socket, web_response, sizeof(web_response),0);
		
		int count = 0;
		int total = 0;
		while((count = recv(web_socket, &web_response[total], sizeof(web_response)-total,0))>0){
			total = total + count;
		}
		
		close(web_socket);
		printf("Web Response Received:\n");
		printf("%s\n", web_response);
		
/*		//check the response for any blocked words
			//content type is the last parameter in the http response header so we find that and 
			//move the pointer until the end of the line to get to the body
		temp = NULL;	
		temp = strstr(web_response, "Content-Type:");
		if(temp!=NULL){
			while(*temp != '\n'){
				temp++;
			}
			temp++;
			printf("This is what is in temp\n%s\n", temp);
			
			if(strstr(temp,"Floppy")!=NULL){
				printf("FLOPPY IN BODY\n");
			}
			for(int i = 0; i<total_blocked; i++){
				if(blocked_list[i]!=NULL){
					if(strstr(temp,blocked_list[i])!=NULL){
						printf("INSIDE THE BODY BLOCKER\n");
						memset(web_request,0,sizeof(web_request));
						strcpy(web_request, error_path1);
						temp = strstr(client_message, "HTTP/");
						strcat(web_request,temp);
						memset(web_request,0,sizeof(web_request));
						
						printf("\nA blocked word was detected in the BODY of the web response and was changed to the following\n");
						printf("%s\n", web_request);
						
						//give web server address
						web_socket = socket(AF_INET, SOCK_STREAM, 0);
						web_address.sin_family = AF_INET;
						web_address.sin_port = htons(80);
						inet_aton("136.159.2.17", &web_address.sin_addr.s_addr);
						
						//connect to web server
						connect(web_socket, (struct sockaddr*) &web_address, sizeof(web_address));
						printf("Connected To Web\n");
						
						//send request
						send(web_socket, web_request, sizeof(web_request),0);
						printf("Sent Data\n\n");
						
						//receive response and close web socket
						recv(web_socket, web_response, sizeof(web_response),0);
						close(web_socket);
						printf("Web Response Received:\n");
						printf("%s\n", web_response);
						
						break;
					}
				}
			}
			
		}
*/		
		//send back to original client socket and close the client socket
		send(client_socket, web_response, sizeof(web_response),0);
cleanUp:		
		close(client_socket);
		printf("Web Response sent back to browser\n");
		
		
		//clear the strings we used
		memset(client_message,0,sizeof(client_message));
		memset(web_request,0,sizeof(web_request));
		memset(web_response,0,sizeof(web_response));
		temp = NULL;
		memset(url,0,sizeof(url));
		memset(path,0,sizeof(path));
		memset(host,0,sizeof(host));
		memset(blocker,0,sizeof(blocker));
		printf("End of While Reached\n");
	}
	
	return 0;

}
