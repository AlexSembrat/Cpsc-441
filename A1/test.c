/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
 /*  char blocked_list[10][1000];
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
	blocked_list[2][0] = NULL;
	
	printf("%s\n", blocked_list[0]);
	printf("%s\n", blocked_list[1]);
	
	memset(blocked_list[1],0,sizeof(blocked_list[1]));
	
	printf("%s\n", blocked_list[0]);
	printf("%s\n", blocked_list[1]);
	



	struct hostent *address;
	address = gethostbyname("pages.cpsc.ucalgary.ca");
	char *ip = address->h_addr;
	//printf("%s\n", address->*h_addr);
    return 0;

}
*/


#include <unistd.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
    char string[10];
while(1){
    if( poll(&mypoll, 1, 2000) )
    {
        scanf("%9s", string);
        printf("Read string - %s\n", string);
    }
    else
    {
        puts("Read nothing");
    }
}
    return 0;
}