#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <filesystem>
/* You will to add includes here */

int clientid;

int gsready(std::string &ip, int port);

bool file_exists(const std::string& filename);

void send_file(int csocketfd, const std::string& filename);


void send_response(int csocketfd, const std::string& status_code, const std::string& content_type, const std::string& content);

bool file_exists(const std::string& filename);

void handle_client_1(int csocketfd);

    struct sockaddr_in server_addr;
    struct timeval timeout;

void *callback(void* temp);

void *callback_1(void* temp);

std::vector<std::string> split(std::string sString, std::string delimiter);

int main(int argc, char *argv[]){

clientid =0;
struct sockaddr_in clientaddress;
socklen_t addrlen = sizeof(clientaddress);
  
  /* Do more magic */
 std::string delimiter = ":";

 std::vector<std::string> outputString = split(argv[1],":"); 

 std::string ipString = "";

 int port;



 if(outputString.size() > 2){
  port = atoi(outputString[outputString.size()-1].c_str());
  for(int i=0; i < 8 ; i++){
   ipString = ipString + outputString[i];}}
else{
   port = atoi(outputString[1].c_str());
   ipString = outputString[0];}





 int mastersocketfd = gsready(ipString ,port);



        int yes = 1;
        if (setsockopt(mastersocketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt error");
            close(mastersocketfd);
        }



std::vector<char> recvbuffvec(1024);

while (1)
{
     
  int *csocketfd = new int;
  *csocketfd = accept(mastersocketfd,(struct sockaddr*)&clientaddress, &addrlen);
  clientid++;
  if(*csocketfd < 0){perror("error with accept function"); exit(1);}
   timeout.tv_sec = 1;
   timeout.tv_usec = 0;
    if (setsockopt(*csocketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


        int opt = 1;
    if (setsockopt(*csocketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: Unable to set socket option\n";
        close(*csocketfd);
        exit(1);
    }
    
   pthread_t calhandel;
   if(pthread_create(&calhandel,NULL,callback_1,(void*)csocketfd)!= 0) {
            perror("pthread_create");
            close(*csocketfd);
            continue;
        }

 pthread_detach(calhandel);
} //end master socker while loop



printf("done.\n");
return(0);
}

void handle_client_1(int csocketfd) {

    const int BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    std::string http_response;

      bytes_received = recv(csocketfd, buffer, BUFFER_SIZE, 0);
      if (bytes_received <= 0) {

       if (errno == EAGAIN || errno == EWOULDBLOCK) {
         //no data to read
        }
        else if (bytes_received == 0) {
          // client left
        }
        else{
              perror("recv 1");
        }
        return;
}
    std::string received_data(buffer, bytes_received);
    std::vector<std::string> lines = split(received_data, "\n");
if (!lines.empty()) {
        std::vector<std::string> first_line = split(lines[0], " ");
        if (first_line.size() >= 2) {
            const std::string& method = first_line[0];
            const std::string& path = first_line[1];
            if (method == "GET") {
                if (path == "/") {
                    send_file(csocketfd, "html/get_index.html");
                } else if (file_exists(path.substr(1))) {
                    send_file(csocketfd, path.substr(1));
                } else {
                    send_response(csocketfd, "404 Not Found", "text/html", "");
                }
            } else if (method == "HEAD") {
                if (path == "/") {
                    send_response(csocketfd, "200 OK", "text/html", "");
                } else if (file_exists(path.substr(1))) {
                    send_response(csocketfd, "200 OK", "text/html", "");
                } else {
                    send_response(csocketfd, "404 Not Found", "text/html", "");
                }
            }
        }
    }
    
}



void send_file(int csocketfd, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(file_content.size()) + "\r\n\r\n";
        send(csocketfd, response_header.c_str(), response_header.size(), 0);
        send(csocketfd, file_content.c_str(), file_content.size(), 0);
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
        send_response(csocketfd, "404 Not Found", "text/html", "");
    }
}

void send_response(int csocketfd, const std::string& status_code, const std::string& content_type, const std::string& content) {
    std::string response_header = "HTTP/1.1 " + status_code + "\r\nContent-Type: " + content_type + "\r\nContent-Length: " + std::to_string(content.size()) + "\r\n\r\n";
    send(csocketfd, response_header.c_str(), response_header.size(), 0);
    send(csocketfd, content.c_str(), content.size(), 0);
}
void *callback_1(void* temp){
  int *csocketfd = (int*)temp;
  handle_client_1(*csocketfd);
  close(*csocketfd);
  delete csocketfd;
  pthread_exit(0);
}

void *callback(void* temp){
      int *csocketfd = (int*)temp;
      std::vector<char> recvbuffvec(2048);
      int sentrecvbytes = recv(*csocketfd, &recvbuffvec[0], recvbuffvec.size(), 0);
      if(sentrecvbytes == 0){close(*csocketfd);clientid--;}
      else if(sentrecvbytes < 0){perror("error with recv");clientid--;}
      char* recvbuff = new char[recvbuffvec.size()];
      std::memcpy(recvbuff, recvbuffvec.data(), recvbuffvec.size());
      std::vector<std::string> sVectorString = split(std::string(recvbuff), "\n");
      delete recvbuff;
      std::vector<std::string> sVectorStringfirstline = split(sVectorString[0], " ");
      if(sVectorStringfirstline[0] == "GET"){

      if(sVectorStringfirstline[1] == "/"){
      std::ifstream htmlFile("html/get_index.html");
     if (!htmlFile.is_open()) {std::cerr << "Failed to open index.html\n";exit(1);}
     std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
     std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size()) + "\r\n\r\n" + htmlContent;
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;} 
    else if (bytesSent == 0){close(*csocketfd);}

      }
      else if (file_exists(sVectorStringfirstline[1].substr(1))){
    std::ifstream htmlFile(sVectorStringfirstline[1].substr(1));
    if (!htmlFile.is_open()) {std::cerr << "Failed to open afile.html\n";exit(1);}
    std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size()) + "\r\n\r\n" + htmlContent;
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;} 
    else if (bytesSent == 0){close(*csocketfd);clientid--;}
    
      }
      else{


    std::ifstream htmlFile("html/error.html");
    if (!htmlFile.is_open()) {std::cerr << "Failed to open error.html\n";exit(1);}
    std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size()) + "\r\n\r\n" + htmlContent;
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;}
     else if (bytesSent == 0){close(*csocketfd);} 



      }
      


      }
      else if(sVectorStringfirstline[0] == "HEAD")
      {
    if(sVectorStringfirstline[1] == "/"){
      std::ifstream htmlFile("html/get_index.html");
     if (!htmlFile.is_open()) {std::cerr << "Failed to open index.html\n";exit(1);}
     std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
     std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size());
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;} 
    else if (bytesSent == 0){close(*csocketfd);clientid--;}

      }

  else if (file_exists(sVectorStringfirstline[1].substr(1))){
    std::ifstream htmlFile(sVectorStringfirstline[1].substr(1));
     if (!htmlFile.is_open()) {std::cerr << "Failed to open index.html\n";exit(1);}
    std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size());
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;exit(1);} 
      else if (bytesSent == 0){close(*csocketfd);clientid--;} 

      }
      else{

    std::ifstream htmlFile("html/error.html");
    if (!htmlFile.is_open()) {std::cerr << "Failed to open error.html\n";exit(1);}
    std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(htmlContent.size());
    // Send HTML content to the client
    int bytesSent = send(*csocketfd, httpResponse.c_str(), httpResponse.size(), 0);
    if (bytesSent < 0) {std::cerr << "Error sending HTML content\n";close(*csocketfd);clientid--;exit(1);} 
      else if (bytesSent == 0){close(*csocketfd);clientid--;} 



      }
      
      
      }

close(*csocketfd);
delete csocketfd;
pthread_exit(0);
}

int gsready(std::string &ip, int port) {
    int socketfd; 
    std::string portStr = std::to_string(port);
    struct addrinfo hint, *output, *temp;
    int check = 0;
    
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    
    int status = getaddrinfo(ip.c_str(), portStr.c_str(), &hint, &output);

    if (status != 0) {
        perror("getaddrinfo error");
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }

    for (temp = output; temp != NULL; temp = temp->ai_next) {
        socketfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

        if (socketfd == -1) {
            perror("socket error");
            continue;
        }

        if (bind(socketfd, temp->ai_addr, temp->ai_addrlen) == -1) {
            perror("bind error");
            close(socketfd); // Close the socket if bind fails
            continue;
        } else {
            std::cout << "[x]Listening on " << ip << ":" << port << std::endl;
            break;
        }
    }

    if (temp == NULL) {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(1);
    }

    if (listen(socketfd, 5) == -1) {
        perror("error with listen function");
        exit(1);
    }
  
    freeaddrinfo(output);
    return socketfd;
}

std::vector<std::string> split(std::string sString, std::string delimiter) {
    std::vector<std::string> nString;
    std::string temp;
    size_t pos = 0;

    // Find the first occurrence of the delimiter
    pos = sString.find(delimiter);

    while (pos != std::string::npos) {
        // Split the string at the delimiter
        nString.push_back(sString.substr(0, pos));
        
        // Erase the split part from the original string
        sString.erase(0, pos + delimiter.length());

        // Find the next occurrence of the delimiter
        pos = sString.find(delimiter);
    }

    // Push the remaining part of the string
    nString.push_back(sString);

    return nString;
}




bool file_exists(const std::string& filename) {
    // Check if file exists and we can access it
    std::filesystem::path filepath(filename);
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
}

