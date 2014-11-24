#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#define BUF_BIG 2048
#define BUF_SMALL 100
#define BUF_VERYSMALL 10

void error_handling(char *message);
void * handle_clnt(void * arg);
void send_data(int sock, char * ctntType, char * filename);
void send_errMsg(int sock);
char * content_type(char * filename);
int main(int argc, char *argv[]){

  int serv_sock, clnt_sock;
  struct sockaddr_in serv_addr, clnt_addr;
  socklen_t addr_size;
  pthread_t t_id;

  if( argc != 2 ) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }
  
  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    error_handling("bind() error");
  if(listen(serv_sock, 5) == -1)
    error_handling("listen() error");

  while(1){
    addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
    printf("connected clinet IP : %s \n", inet_ntoa(clnt_addr.sin_addr));
    pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
    pthread_detach(t_id);
  }
  close(serv_sock);
  return 0;
}

void * handle_clnt(void * arg){
  int clnt_sock = *((int*)arg);
  char buf[BUF_BIG];
  char method[BUF_VERYSMALL];
  char ctntType[BUF_SMALL];
  char filename[BUF_SMALL];
  
  read(clnt_sock, buf, BUF_BIG);
  if(strstr(buf, "HTTP/") == NULL){
    send_errMsg(clnt_sock);
    return (void*)1;
  }
  printf("here\n");
  strcpy(method, strtok(buf, " /"));
  printf("method : %s\n", method);
  //error
  if(strcmp(method, "GET")){
    send_errMsg(clnt_sock);
  }
  
  strcpy(filename, strtok(NULL, " /"));
  printf("filename : %s\n", filename);
  strcpy(ctntType, content_type(filename));
  send_data(clnt_sock, ctntType, filename);
  return 0;  
}

void send_data( int sock, char * ctntType, char * filename){
  char protocol[] = "HTTP/1.0 200 OK\r\n";
  char servname[] = "Simple Server\r\n";
  char ctntlength[] = "Content-length:2048\r\n";
  char ctntTypeForm[BUF_SMALL];
  char buf[BUF_BIG];
  FILE * sendFile;
  printf("ctnt : %s\n", ctntType);
  sprintf(ctntTypeForm, "Content-type:%s\r\n\r\n", ctntType);
  if((sendFile = fopen(filename, "r")) == NULL){
    send_errMsg(sock);
    return;
  }
  
  write(sock, protocol, strlen(protocol));
  write(sock, servname, strlen(servname));
  write(sock, ctntlength, strlen(ctntlength));
  write(sock, ctntTypeForm, strlen(ctntTypeForm));
  
  while(fgets(buf, BUF_BIG, sendFile) != NULL)
    write(sock, buf, strlen(buf));

  close(sock);
}

void send_errMsg( int sock ){
  char protocol[] = "HTTP/1.0 404 Bad Request\r\n";
  char servname[] = "Simple Server\r\n";
  char ctntlength[] = "Content-length:2048\r\n";
  char ctntTypeForm[] = "Content-type:text/html\r\n\r\n";
  char buf[BUF_BIG];
  FILE * sendFile;

  printf("404 !!! \n");
  if((sendFile = fopen("errorpage.html", "r")) == NULL)
     return;
  
  write(sock, protocol, strlen(protocol));
  write(sock, servname, strlen(servname));
  write(sock, ctntlength, strlen(ctntlength));
  write(sock, ctntTypeForm, strlen(ctntTypeForm));

  while(fgets(buf, BUF_BIG, sendFile) != NULL)
    write(sock, buf, strlen(buf));

  close(sock);
}

char * content_type(char * filename){
  char ext[BUF_VERYSMALL];
  char name[BUF_SMALL];
  char *tmp;
  //  printf("filename in ctnt function : %s\n", filename);
  strcpy(name, filename);
  tmp = strtok(NULL, ".");
  if(!tmp) strcpy(ext, tmp);
  else return "text/html";

  if(!strcmp(ext, "html") || !strcmp(ext, "htm"))
    return "text/html";
  else if(!strcmp(ext, "css"))
    return "text/css";
  else if(!strcmp(ext, "js"))
    return "application/javascript";
  else 
    return "text/plain";
}

void error_handling(char * msg){
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}
