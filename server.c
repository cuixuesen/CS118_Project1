#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    fprintf(stderr, "Unable to open socket.");

  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(atoi(argv[1]));  

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    fprintf(stderr, "Unable to bind socket");

  listen(sockfd, 1);

  while(1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      fprintf(stderr, "Unable to accept connection.");

    char request[2000];
    memset(request, 0, 2000);

    int n = read(newsockfd, request, 2000);
    if (n < 0) {
      fprintf(stderr, "Error on reading.");
      close(newsockfd);
      continue;
    }
    write(0, request, strlen(request));
  }
  close(sockfd);
}
