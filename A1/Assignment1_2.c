#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	char web_response[10000];
	char *temp;
	char url[1000];
	char host[1000];		
	char path[1000];
	char error_path[] = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error.html ";
	char blocked_list[10][1000];
	int total_blocked = 2;			//should start as 0 once we are dynamic
	blocked_list[0][0] = 'F'; 
	blocked_list[0][1] = 'l';
	blocked_list[0][2] = 'o';
	blocked_list[0][3] = 'p';
	blocked_list[0][4] = 'p';
	blocked_list[0][5] = 'y';
	blocked_list[1][0] = 'c';
	blocked_list[1][1] = 'u';
	blocked_list[1][2] = 'r';
	blocked_list[1][3] = 'l';
	blocked_list[1][4] = 'i';
	blocked_list[1][5] = 'n';
	blocked_list[1][6] = 'g';
	while(1){
		//accept client socket
		client_socket = accept(server_socket, NULL, NULL); 
		printf("\nAccepted Client Socket\n");
		recv(client_socket, &client_message, sizeof(client_message), 0); //receive the data that must be parsed
		printf("\nReceived Data From Client Socket:\n");
		printf("%s", client_message);
		printf("\n");
		
		//block/unblock features
		
		/* unblock
			memset(blocked_list[total_blocked-1],0,blocked_list[total_blocked-1]);
			total_blocked--;
		*/
		
		/* block
			read in the word to block and copy it into blocked_list[total_blocked] (probably using a loop)
			total_blocked++;
		*/
		
		
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
		
		
		//send back to original client socket and close the client socket
		send(client_socket, web_response, sizeof(web_response),0);
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
		printf("End of While Reached\n");
	}
	
	return 0;

}
