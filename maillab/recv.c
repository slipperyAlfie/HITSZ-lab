#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SIZE 65535

char buf[MAX_SIZE+1];

void recv_mail()
{
    const char* host_name = "pop.qq.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 110; // POP3 server port
    const char* user = "xxxxxxx"; // TODO: 你的邮箱地址
    const char* pass = "xxxxxxx"; // TODO: 你的邮箱授权码
    char dest_ip[16];
    int s_fd; // socket file descriptor
    struct hostent *host;
    struct in_addr **addr_list;
    int i = 0;
    int r_size;
    char * strbuf = malloc(MAX_SIZE);
    bzero(strbuf,MAX_SIZE);

    // Get IP from domain name
    if ((host = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    addr_list = (struct in_addr **) host->h_addr_list;
    while (addr_list[i] != NULL)
        ++i;
    strcpy(dest_ip, inet_ntoa(*addr_list[i-1]));

    // TODO: Create a socket,return the file descriptor to s_fd, and establish a TCP connection to the POP3 server
    
    s_fd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    struct in_addr temp;
    temp.s_addr = inet_addr(dest_ip);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = (port << 8) | (port >> 8);
    bzero(servaddr.sin_zero,8);
    servaddr.sin_addr.s_addr = inet_addr(dest_ip);

    connect(s_fd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    // Print welcome message
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);

    // TODO: Send user and password and print server response
    char* USER = "user ";
    strcpy(strbuf,USER);
    strcat(strbuf,user);
    strcat(strbuf,"\r\n");
    send(s_fd, strbuf, strlen(strbuf), 0);
    // TODO: Print server response to EHLO command
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);

    char* PASS = "pass ";
    strcpy(strbuf,PASS);
    strcat(strbuf,pass);
    strcat(strbuf,"\r\n");
    send(s_fd, strbuf, strlen(strbuf), 0);

    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    // TODO: Send STAT command and print server response
    const char* STAT = "stat\r\n";
    send(s_fd, STAT, strlen(STAT), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    // TODO: Send LIST command and print server response
    // no para first
    const char* LIST = "list\r\n"; // TODO: Enter EHLO command here
    send(s_fd, LIST, strlen(LIST), 0);
    // TODO: Print server response to EHLO command
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    // TODO: Retrieve the first mail and print its content
    const char* RETR = "retr 1\r\n";
    send(s_fd, RETR, strlen(RETR), 0);
    // TODO: Print server response to EHLO command
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    // TODO: Send QUIT command and print server response
    const char* QUIT = "quit\r\n";
    send(s_fd, QUIT, strlen(QUIT), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    close(s_fd);
}

int main(int argc, char* argv[])
{
    recv_mail();
    exit(0);
}
