/*
   Server Program
   Updated: 2011.10.04
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

int main()
{
	int sock, sock2, length;
	struct sockaddr_in server, client;
	short cate;

	if( (sock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{	
		printf(" Socket Open Error \n");
		exit(1); 
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 9190;

	if( bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1 )
	{	
		printf(" Bind Error \n"); 
		exit(1); 
	}

	if( listen(sock, 5) == -1 )
	{	
		printf(" Listen Error \n"); 
		exit(1); 	
	}

	printf(" Connection Ready \n");
	while( 1 )
	{
		if( (sock2 = accept(sock, (struct sockaddr *)&client, &length)) == -1)
		{	
			printf(" Accept Error \n"); 
			exit(1); 
		}

		while(1) {
			if( recv(sock2, &cate, sizeof(cate), 0) == -1 )
			{	
				printf(" Receive Error \n");
				exit(1); 
			}
		
			printf("\n%d�� �о߸� �����ϼ̽��ϴ�. \n", cate);

			if(cate == 4) {
				printf("Ŭ���̾�Ʈ���� ������ ���������� ����Ǿ����ϴ�.\n");
				break;
			}
		}
		close(sock2);
	}
}

