#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define BUF_SIZE 1024
#define TRUE 1
#define FALSE 0

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * message);

char message[BUF_SIZE];

int main(int argc, char* argv[])
{
  int sock;
  int str_len, recv_len, recv_cnt;
  pthread_t send_thread, recv_thread;
  struct sockaddr_in serv_addr;
  void * thread_return;

  if( argc != 3 ){
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock == -1)
    error_handling("socket() error");
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    error_handling("connect() error!");
  else
    puts("Connected........");

  pthread_create(&send_thread, NULL, send_msg, (void*)&sock);
  pthread_create(&recv_thread, NULL, recv_msg, (void*)&sock);
  pthread_join(send_thread, &thread_return);
  pthread_join(recv_thread, &thread_return);
  close(sock);
  return 0;
}

void * send_msg(void * arg){
  int sock = *((int*)arg);
  int str_len = 0;
  while(TRUE){
    fputs("Input message ( Q to quit ): ", stdout);
    fgets(message, BUF_SIZE, stdin);
    if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")){
      close(sock);
      exit(0);
    }
    
    str_len = write(sock, message, strlen(message));
    printf("strlen : %d\n", str_len); 
  }

}
void * recv_msg(void * arg){
  int sock = *((int*)arg);
  int str_len;
  str_len = 0;
  while(TRUE){
    str_len = read(sock, message, BUF_SIZE-1);
    if(str_len == -1){
      error_handling("read() error!");
      return (void*)-1;
    }
    message[str_len] = '\0';
    printf("\nmessage from server : %s", message);
  }
}

void error_handling(char * message){
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
