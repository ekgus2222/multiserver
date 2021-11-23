/* client.c */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <sys/stat.h>

#include <pthread.h>

#include <time.h>

void *send_file(void *send_sniffling);
void error_handling(char *message);

typedef struct
{
	char *port;
	char *addr;
	int sniffling;
} socket_data;

int main(int argc, char *argv[])
{
	struct timespec ts;
	int start = ts.tv_sec;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <PORT> \n", argv[0]);
		exit(1);
	}

	pthread_t p_thread[2];
	for (int i = 0; i < 2; i++)
	{
		socket_data *sd = (socket_data *)malloc(sizeof(socket_data));
		sd->port = argv[2];
		sd->addr = argv[1];
		sd->sniffling = i;
		pthread_create(&p_thread[i], NULL, send_file, (void *)sd);
	}
	for (int i = 0; i < 2; i++)
	{
		pthread_join(p_thread[i], NULL);
	}

	printf("finish : %ld sec\n", ts.tv_sec - start);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *send_file(void *socketdata)
{
	socket_data *sd = (socket_data *)socketdata;
	int sniffling = sd->sniffling;

	int serv_sock, fd;
	int str_len, len;
	struct sockaddr_in serv_addr;
	char message[30], buf[BUFSIZ];
	FILE *file = NULL;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(sd->addr);
	serv_addr.sin_port = htons(atoi(sd->port));

	if (connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");

	// test
	str_len = read(serv_sock, message, sizeof(message) - 1);

	if (str_len == -1)
		error_handling("read() error!");
	printf("Message from server: %s \n", message);

	////////////////////////////////////////////////////////
	if (sniffling == 0)
	{ //odd
		for (int i = 1; i < 1000; i+=2)
		{
			if (i % 10 == 1 || i % 10 == 2 || i % 10 == 3)
				continue;
			struct stat file;

			//set file name
			char end[20] = "th copy).png";
			char filename[50] = "baram ("; //char* filename = "0.png";
			char number[5];
			if (i >= 1000)
			{
				char number2[5];
				sprintf(number, "%d", i / 1000);
				strcat(filename, number);
				strcat(filename, ",");
				sprintf(number2, "%d", i % 1000);
				strcat(filename, number2);
			}
			else
			{
				sprintf(number, "%d", i);
				strcat(filename, number);
			}
			strcat(filename, end);

			//printf("%s \n", filename);

			stat(filename, &file);
			int fd = open(filename, O_RDONLY);
			int size = file.st_size;
			//printf("fd:%d / size:%d /filename:%s\n", fd, size, filename);
			if (fd == -1)
				return 0;

			send(serv_sock, &size, sizeof(int), 0); ///s1
			sleep(1);
			send(serv_sock, filename, sizeof(char) * 30, 0); ///s2

			if (size)
			{
				printf("sending %s file\n", filename);
				//printf("fd : %d, size = %d\n", fd, size);
				//clck_gettime(CLOCK_REALTIME, &ts);
				sendfile(serv_sock, fd, NULL, size); ///s3
			}
		}
	}
	else
	{
		for (int i = 0; i < 1000; i+=2)
		{
			if (i % 10 == 1 || i % 10 == 2 || i % 10 == 3)
				continue;
			struct stat file;

			//set file name
			char end[20] = "th copy).png";
			char filename[50] = "baram ("; //char* filename = "0.png";
			char number[5];
			if (i >= 1000)
			{
				char number2[5];
				sprintf(number, "%d", i / 1000);
				strcat(filename, number);
				strcat(filename, ",");
				sprintf(number2, "%d", i % 1000);
				strcat(filename, number2);
			}
			else
			{
				sprintf(number, "%d", i);
				strcat(filename, number);
			}
			strcat(filename, end);

			//printf("%s \n", filename);

			stat(filename, &file);
			int fd = open(filename, O_RDONLY);
			int size = file.st_size;
			//printf("fd:%d / size:%d /filename:%s\n", fd, size, filename);
			if (fd == -1)
				return 0;

			send(serv_sock, &size, sizeof(int), 0); ///s1
			sleep(1);
			send(serv_sock, filename, sizeof(char) * 30, 0); ///s2

			if (size)
			{
				printf("sending %s file\n", filename);
				//printf("fd : %d, size = %d\n", fd, size);
				//clck_gettime(CLOCK_REALTIME, &ts);
				sendfile(serv_sock, fd, NULL, size); ///s3
			}
		}
	}

	/////////////////////////////////////////////////////////

	close(serv_sock);
}
