#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netdb.h>
#include<string.h>

int main(){
          
   int sock_fd =  socket(AF_INET, SOCK_STREAM, 0);//just defining the protocol here
   struct addrinfo hint, *res;
   

   int yes=1;
   // fixing the "Address already in use" error message
   setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    
   memset(&hint, 0, sizeof(hint)); 
   if(sock_fd < 0)  
   {
    perror("Socket creation failed");
    exit(-1);
   }
   
   hint.ai_family = AF_INET;// i want to support IPV4 only
   hint.ai_socktype = SOCK_STREAM;
   hint.ai_flags = AI_PASSIVE;

   getaddrinfo(NULL, "8080", &hint, &res);

   int bind_result = bind(sock_fd, res->ai_addr, res->ai_addrlen);

   if(bind_result < 0)
   {
   perror("Bind error ...\n");
   exit(-1);
   }
   //listening on this port -> 
   int listen_result = listen(sock_fd,20);   
   
   if(listen_result < 0)
   {
   perror("error while trying to listen");
   exit(-1); 
   }
  
    //accept
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    int new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);


    int max_len = 1000;
    char recieved_req[1000];
    memset(recieved_req, 0, sizeof(recieved_req));

    int num_bytes = recv(new_fd, recieved_req, max_len, 0);
        
    if(strncmp(recieved_req,"GET",3) == 0)
    {

      printf("Recieved a GET request\n"); 
      //we need to respond to the GET request now
      
      char *status_line = "HTTP/1.1 200 OK\r\n";
      //what appears on the top of HTTP response is just a plain text
      send(new_fd, status_line, strlen(status_line), 0);
      

      char *headers = "Content-Type: text/html\r\n\r\n";
      int sent_bytes = send(new_fd, headers, strlen(headers), 0);
      printf("sent_bytes = %d\n", sent_bytes);     
 

      char *blank_line = "\r\n";
      sent_bytes = send(new_fd, blank_line, strlen(blank_line), 0);
      printf("sent_bytes = %d\n", sent_bytes); 
      

      FILE *index_file = fopen("index.html", "r");
      if (index_file == NULL) {
        perror("Failed to open file");
        // Send 404 error response...
      }

     char buffer[4096];
     size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), index_file)) > 0) {
       send(new_fd, buffer, bytes_read, 0);
    }
    fclose(index_file);


  close(sock_fd);
  close(new_fd);
  }
}
