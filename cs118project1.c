#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

int main(int argc, char** argv) {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    if (argc != 2 ){
        fprintf(stderr, "Wrong argument number");
        exit(0);
    }
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
        
        ssize_t n = read(newsockfd, request, 2000);
        if (n < 0) {
            fprintf(stderr, "Error on reading.");
            close(newsockfd);
            continue;
        }
        write(0, request, strlen(request));
        
        
        char file_name[2000];
        memset(file_name, 0, 2000);
        
        
        // Get the file name
        int i=5,j=0;
        while(request[i] != ' ' && i < 2000 && i+1 < 2000 && i+2 < 2000){

            if (request[i] == '%' && request[i+1] == '2' && request[i+2] == '0'){
                file_name[j++]=' ';
                i=i+3;
            }
            else{
                file_name[j++]=request[i++];
               
            }
        }
	
        
        FILE *myfile;
        myfile=fopen(file_name,"r");
        if (myfile == NULL){
            
            char *reponse_header="HTTP/1.1 404 Not Found\r\nConnection: close\r\nServer: Apache\r\nContent-Type: text/html\r\n\r\n <title>Page not found</title> <h1>404</h1>";
            write(newsockfd, reponse_header, strlen(reponse_header));
            
        }
        else {
            
            char *reponse_type=NULL;
            char *extention = strrchr (file_name, '.');
          
            
            if (extention == NULL){
                         reponse_type="Content-Type: application/octet-stream\r\n\r\n";
            }
            else if (strcasecmp(extention,".html") == 0  ||
                     strcasecmp(extention,".htm")  == 0  ||
                     strcasecmp(extention,".txt")  == 0  ){
                         reponse_type="Content-Type: text/html\r\n\r\n";
            }
            else if (strcasecmp(extention,".jpg")  == 0  ||
                     strcasecmp(extention,".jpeg") == 0  ||
                     strcasecmp(extention,".png")  == 0  ){
                         reponse_type="Content-Type: image/jpeg\r\n\r\n";
            }
            else if (strcasecmp(extention,".gif") == 0   ){
                         reponse_type="Content-Type: image/gif\r\n\r\n";
            }
            else {
                
                        char *reponse_header_h1="HTTP/1.1 404 Not Found\r\nContent-Length: 75\r\nConnection: close\r\nServer: Apache\r\nContent-Type: text/html\r\n\r\n <title>File not found</title> <h1>The file enxtension is not avaiable</h1>";
                        write(newsockfd, reponse_header_h1, strlen(reponse_header_h1));
                        close(newsockfd);
                        continue;
                
            }
            
            //Get the file size
            struct stat st;
            stat(file_name, &st);
            char *reponse_header=malloc(3000);
            sprintf(reponse_header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\nServer: Apache\r\nDate: Wed, 27 Mar 2019 08:38:00 GMT\r\nLast-Modified: Wed, 27 Mar 2019 08:38:00 GMT\r\n%s", st.st_size,reponse_type);
            
            write(newsockfd, reponse_header, strlen(reponse_header));
            
            //Write file to the scoket
            char reponsefile[9000];
            while(fread(reponsefile,1,sizeof(reponsefile),myfile) > 0) {
                if (write(newsockfd, reponsefile,sizeof(reponsefile)) < 0 ){
                    fprintf(stderr, "Error on reading.");
                    close(newsockfd);
                    continue;
                }
                memset(reponsefile, 0, 9000);
                
            }
	    fclose(myfile);
        }
	 
        close(newsockfd);
        
        
    }
    close(sockfd);

}



