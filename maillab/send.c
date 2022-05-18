#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include "base64_utils.h"

#define MAX_SIZE 4095
#define swap16(x) ((((x)&0xFF) << 8) | (((x) >> 8) & 0xFF))

char buf[MAX_SIZE+1];

// receiver: mail address of the recipient
// subject: mail subject
// msg: content of mail body or path to the file containing mail body
// att_path: path to the attachment
void send_mail(const char* receiver, const char* subject, const char* msg, const char* att_path)
{
    
    const char* end_msg = "\r\n.\r\n";
    const char* end_one = "\r\n";
    const char* host_name = "smtp.qq.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 25; // SMTP server port
    const char* pass = "xxxxxxx"; // TODO: 你的邮箱授权码
    const char* from = "xxxxxxx"; // TODO: 你的邮箱地址
    char dest_ip[16]; // Mail server IP address
    int s_fd; // socket file descriptor
    struct hostent *host;
    struct in_addr **addr_list;
    int i = 0;
    int r_size;
    char *strbuf = malloc(MAX_SIZE + 1);
    //printf("start:\n");

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

    // TODO: Create a socket, return the file descriptor to s_fd, and establish a TCP connection to the mail server
    
    s_fd = socket(AF_INET,SOCK_STREAM,0);
    //printf("create socket complete\n");
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    uint16_t swap_port = swap16(port);
    servaddr.sin_port = swap_port;
    in_addr_t to_ip = inet_addr(dest_ip);
    servaddr.sin_addr.s_addr = to_ip;
    bzero(servaddr.sin_zero,8);
    
    //printf("%x\n",to_ip);
    
    connect(s_fd,(struct sockaddr*)&servaddr,sizeof(servaddr));

    //printf("connect socket complete\n");
    // Print welcome message
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    
    //printf("rsize = %d\n",r_size);
    buf[r_size] = '\0'; // Do not forget the null terminator
    // printf("%s", buf);
    // Replace buf
    // bzero(buf,MAX_SIZE);
    // Send EHLO command and print server response

    const char* EHLO = "EHLO qq.com\r\n"; // TODO: Enter EHLO command here
    r_size = send(s_fd, EHLO, strlen(EHLO), 0);
    // TODO: Print server response to EHLO command
    // printf("rsize = %d\n",r_size);

    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);
    // printf("EHLO complete\n");
    // TODO: Authentication. Server response should be printed out.
    const char* AUTH = "AUTH login\r\n";
    send(s_fd, AUTH, strlen(AUTH), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);

    char* base64_from = encode_str(from);
    
    strcpy(strbuf,base64_from);
    strcat(strbuf,"\r\n");
    // printf("base64_from = %s\n",strbuf);
    
    send(s_fd,strbuf,strlen(strbuf),0);
    free(base64_from);

    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);

    char* base64_pass = encode_str(pass);
    
    strcpy(strbuf,base64_pass);
    strcat(strbuf,"\r\n");
    // printf("base64_pass = %s\n",strbuf);
    
    r_size = send(s_fd,strbuf,strlen(strbuf),0);
    // printf("r_size = %d\n",r_size);
    free(base64_pass);
    // bzero(strbuf,MAX_SIZE);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    // bzero(buf,MAX_SIZE);

    // TODO: Send MAIL FROM command and print server response
    char* MAIL_FROM = "MAIL FROM:";
    strcpy(strbuf,MAIL_FROM);
    strcat(strbuf,"<");
    strcat(strbuf,from);
    strcat(strbuf,">");
    strcat(strbuf,"\r\n");
    
    send(s_fd, strbuf, strlen(strbuf), 0);
    //memset(strbuf,0,MAX_SIZE);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);
    // printf("Mail from complete\n");
    // TODO: Send RCPT TO command and print server response
    char* PCRT_TO = "RCPT TO:";
    strcpy(strbuf,PCRT_TO);
    strcat(strbuf,"<");
    strcat(strbuf,receiver);
    strcat(strbuf,">");
    strcat(strbuf,"\r\n");
    send(s_fd, strbuf, strlen(strbuf), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);
    // printf("PCRT complete\n");
    // TODO: Send DATA command and print server response
    const char* DATA = "data\r\n";
    send(s_fd, DATA, strlen(DATA), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // bzero(buf,MAX_SIZE);
    // printf("data complete\n");

    // TODO: Send message data
    FILE * fp;
    fp = fopen(msg,"r");
    char* msg_get = malloc(1000);
    int index = 0;
    while(fscanf(fp,"%c",&msg_get[index])!=EOF){
        index++;
    }
    // printf("%s\n",msg_get);
    fclose(fp);
    
    char* message = "From:";
    
    strcpy(strbuf,message);
    strcat(strbuf,from);
    strcat(strbuf,"\r\n");
    strcat(strbuf,"To:");
    strcat(strbuf,receiver);
    strcat(strbuf,"\r\n");
    strcat(strbuf,"MIME-Version: 1.0\r\n");
    strcat(strbuf,"Content-Type: multipart/mixed; boundary=qwertyui\r\n");
    strcat(strbuf,"Subject:");
    strcat(strbuf,subject);
    strcat(strbuf,"\r\n");
    strcat(strbuf,"preamble\r\n");
    strcat(strbuf,"--qwertyui\r\n");
    strcat(strbuf,"Content-Type:text/plain\r\n\r\n");
    strcat(strbuf,msg_get);
    strcat(strbuf,"\r\n\r\n");
    strcat(strbuf,"--qwertyui\r\n");
    char* name = malloc(30);
    strcpy(name,att_path);
    strcat(strbuf,"Content-Type: application/octet-stream;name=\"");
    strcat(strbuf,name);
    strcat(strbuf,"\"\r\n");
    strcat(strbuf,"Content-Disposition: attachment;filename=\"");
    strcat(strbuf,name);
    strcat(strbuf,"\"\r\n");
    strcat(strbuf,"Content-Transfer-Encoding:base64\r\n\r\n");
    FILE * fd = fopen(att_path,"r");
    FILE * sd = fopen("outputFile.txt","w");
    encode_file(fd,sd);
    fclose(fd);
    fclose(sd);
    FILE * sd2;
    sd2 = fopen("outputFile.txt","r");
    char* zip_get = malloc(1000);
    index = 0;
    while(fscanf(sd2,"%c",&zip_get[index])!=EOF){
        index++;
    }
    // printf("%s\n",zip_get);
    fclose(sd2);
    strcat(strbuf,zip_get);
    strcat(strbuf,"\r\n\r\n");
    strcat(strbuf,"--qwertyui");
    strcat(strbuf,end_msg);
    
    send(s_fd,strbuf,strlen(strbuf),0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);
    // printf("message complete\n");
    // TODO: Send QUIT command and print server response
    
    
    const char* QUIT = "quit\r\n";
    send(s_fd, QUIT, strlen(QUIT), 0);
    r_size = recv(s_fd, buf, MAX_SIZE, 0);
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    bzero(buf,MAX_SIZE);

    //printf("sending complete\n");
    
    close(s_fd);
    
}

int main(int argc, char* argv[])
{
    int opt;
    char* s_arg = NULL;
    char* m_arg = NULL;
    char* a_arg = NULL;
    char* recipient = NULL;
    const char* optstring = ":s:m:a:";
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 's':
            s_arg = optarg;
            break;
        case 'm':
            m_arg = optarg;
            break;
        case 'a':
            a_arg = optarg;
            break;
        case ':':
            fprintf(stderr, "Option %c needs an argument.\n", optopt);
            exit(EXIT_FAILURE);
        case '?':
            fprintf(stderr, "Unknown option: %c.\n", optopt);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Unknown error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
    {
        fprintf(stderr, "Recipient not specified.\n");
        exit(EXIT_FAILURE);
    }
    else if (optind < argc - 1)
    {
        fprintf(stderr, "Too many arguments.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        recipient = argv[optind];
        send_mail(recipient, s_arg, m_arg, a_arg);
        exit(0);
    }
}