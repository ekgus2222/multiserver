/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "mymysql.h"

#include <time.h>

//#include "ocr_DB.h"

void error_handling(char *message);

int cnt = 0;
int main(int argc, char *argv[])
{
	//TIME
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	int start = ts.tv_sec; ////time

	//MYSQL
	MYSQL *con = mysql_init(NULL);
	if(con == NULL){
		fprintf(stderr, "mysql_init() failed\n");
		exit(1);
	}

	if(mysql_real_connect(con, "localhost", "dahyeon", "k7572930", "projectDB", 0, NULL, 0) == NULL)
	{
		finish_with_error(con);
	}

	printf("server start!\n");
	

	//SOCKET
	int serv_sock;
	int clnt_sock;
	char buf[256];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[] = "Hello World!";

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
	if (clnt_sock == -1)
		error_handling("accept() error");

	write(clnt_sock, message, sizeof(message));
	printf("test message send\n");

////////////////////////////////////////////////////////
	for(int i=0; i<1000; i++){
		if(i%10 == 1||i%10 == 2||i%10 == 3) continue;
		
		char filename[30] = "baram(";
		char fileend[10] = ").png";
		char number[7];
		sprintf(number, "%d", i);
		strcat(filename, number);
		strcat(filename, fileend);
		int size;
		recv(clnt_sock, &size, sizeof(int), 0);  ///r1
		recv(clnt_sock, filename, sizeof(char)*30, 0);	///r2
		printf("filename : %s / file size : %d byte\n",filename, size);
//set size
		char* f=malloc(size);
		char* ptr = f;
//get file
		int remain = size;

		while(0<remain){
			int recvsize = 0;
			if(remain<1460){
				recvsize = recv(clnt_sock, ptr, remain, 0);
			}else{
				recvsize=recv(clnt_sock, ptr, 1460, 0);
			}

			ptr+=recvsize;
			remain-=recvsize;
		}

//make file dir
		int fd;
		while(1){
			fd=open(filename,O_CREAT|O_EXCL|O_WRONLY,0666);
			if(fd == -1){
				sprintf(filename+strlen(filename), "_1");
			}else break;
		}

//save file
		printf("fd : %d / size = %d\n",fd,size);
		write(fd,f,size);
		close(fd);

		struct stat downloadfile;
		stat(filename, &downloadfile);
		int download_size = downloadfile.st_size;
		if(download_size == size){
			printf("download_size : %dbyte \nfile size : %dbyte \n--perfect--\n",download_size,size);
		}else{
			printf("download_size : %dbyte \nfile size : %dbyte \n--%dbyte is not downloaded--\n",download_size,size,size-download_size);
		}
	
		run(filename, con);

	}
//////////////////////////////////////////////////////////

	close(clnt_sock);
	close(serv_sock);

	//ocr_run("aurora0.png");

	mysql_close(con);

	clock_gettime(CLOCK_MONOTONIC, &ts);
	printf("Time : %ld\n", ts.tv_sec-start);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
