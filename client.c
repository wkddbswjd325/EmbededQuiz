#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>

#define IP "192.168.1.153"

void showImage(char *file);
void playSound(char *file);
void categoryGameStart(char *img[]);

// �ʿ��� ����������
int life = 4;
int hint = 4;
int score = 0;

char* category[3] = {"��ȭ", "����", "����"};

int led, fnd, dot;
unsigned short led_buff;
unsigned long fnd_buff;
unsigned char dot_buff;
unsigned char loop = 200;

int main()
{
	int sock;
	struct sockaddr_in server;
	
	int push;
	unsigned short push_buff = 0;

	char result[30];

	int chk_play[3]={0,0,0};
	int i=0;
	short cate;
		
	//ī�װ� �� ���� �� �̹��� �̸�
	char *q1[5] = {"q1_1.jpg", "q1_2.jpg", "q1_3.jpg", "q1_4.jpg", "q1_5.jpg"};
	char *q2[5] = {"q2_1.jpg", "q2_2.jpg", "q2_3.jpg", "q2_4.jpg", "q2_5.jpg"};
	char *q3[5] = {"q3_1.jpg", "q3_2.jpg", "q3_3.jpg", "q3_4.jpg", "q3_5.jpg"};
		
	if( (sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		printf("Socket Open Error\n");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_port = 9190;

	if( connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1 )
	{
		printf("Connect Error\n");
		exit(1);
	}

	printf("�ʼ����� START!\n");
	showImage("title.jpg"); 
	sleep(2);
	
	push = open("/dev/PUSH", O_RDWR);
	led = open("/dev/LEDS", O_WRONLY);
	fnd = open("/dev/FNDS", O_WRONLY);
	dot = open("/dev/DOTS", O_WRONLY);

	if ( (push != -1) && (led != -1) && (fnd != -1) && (dot != -1) ) {
		fnd_buff = (unsigned long)0x0;
		write(fnd, &fnd_buff, 3);	

		/* �о� ���� ��� ���ƿͼ� �ϰ� �ݺ� */
		while(1) {
			cate = 0;

			/* screen = category choice(image)*/
			showImage("choice.jpg");
	
			/* push switch�� ī�װ� ���� ���� */	
			
			printf("�о� ����(push switch) \n");
			while(1) {
				usleep(40000);
				read(push, &push_buff, 2);
				if(push_buff > 0) {
					switch(push_buff) {
					  case 2: cate=1; break;
					  case 4: cate=2; break;
					  case 8: cate=3; break;
					  case 16: cate=4; break;
					  default:
						printf("�߸��� �о߰� �ԷµǾ����ϴ�.\n");
						printf("�о߸� �ٽ� �������ּ���\n");
						break;
					}
					
				} 
				if(cate > 0)  break;
			}

			if(cate == 4) {
				showImage("game_over.jpg");
				send(sock, (void *)&cate, 2, 0);
				break;
			}
		
			if(chk_play[cate-1] == 1) {
				printf("�̹� %s �о��� ������ �Ϸ��ϼ̽��ϴ�.\n", category[cate-1]);
				continue;
			}

			send(sock,(void *)&cate, 2, 0);

			printf("%s �о߸� �����ϼ̽��ϴ�.\n", category[cate-1]);
			/*
			   ���õ� ī�װ��� ���� ���� start!! 
			   ī�װ��� ���� �̹��� �������ְ� 
			   ���� START!! -> ������ categoryGameStart(short cate)�Լ�
			   ī�װ��� �Ű������� �޾Ƽ� ����! 
			*/

			showImage("game_start.jpg");			
			playSound("game_start_sound.mp3");
					
			switch(cate) {
				case 1: categoryGameStart(q1); break;	
				case 2: categoryGameStart(q2); break;
				case 3: categoryGameStart(q3); break;
			}
			// ������ �о��� ������ ��ġ�� �о��� ���¸� true�� �ٲ���
			chk_play[cate-1]=1;

			if(life == 0) { //����� �� �޾��� ��� -> ����ó�� ���� �ؾ���!
				showImage("game_over.jpg");
				break;
			}

			/* ��� �о� �Ϸ����� �� �������� �ϰ� ¥���Ѵ�!! -> clear */
			if(chk_play[0] && chk_play[1] && chk_play[2]) {
				showImage("game_clear.jpg");
				playSound("game_clear_sound.mp3");
				cate = 4;
				send(sock,(void *)&cate, 2, 0);
				break;

			}
	
		}
		close(push);
		close(led);
		close(fnd);
		close(dot);
		
		printf("������ ����Ǿ����ϴ�.\n");	

	} else {
		printf("Device Open ERROR!\n");
		exit(-1);
	} 	
	
	close(sock);
} 

void categoryGameStart(char *img[]) {
	hint=4;
	int g=0;

	for(g=0; g<5; g++) {
		showImage(img[g]);
		sleep(2);		
	}

}

void showImage(char *file) {
	FILE *fp;
	char buff[100];
	
	char name[100] = "image/";
	char result_str[200] = "./jpg_sample_view ";
	
	strcat(name , file);
	strcat(result_str, name);

	fp = popen(result_str, "r");

	if(fp == NULL) {
		perror("error");
		exit(0);
	}

	while(fgets(buff, 100, fp) != NULL) {
		printf("%s", buff);
	}
	pclose(fp);
}

void playSound(char *file) {
	FILE *fp;
	char buff[100];
	
	char name[100] = "sound/";
	char result_str[200] = "./madplay ";
	
	strcat(name , file);
	strcat(result_str, name);

	fp = popen(result_str, "r");

	if(fp == NULL) {
		perror("error");
		exit(0);
	}

	while(fgets(buff, 100, fp) != NULL) {
		printf("%s", buff);
	}
	pclose(fp);
}
