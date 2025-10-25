#include "../inc/chat42.h"


void* tcp_thread_func(void* arg) {

    int                 sockfd;
    int                 newsockfd;
    struct sockaddr_in  servaddr;
    struct sockaddr_in  cliaddr;
    socklen_t           clilen = sizeof(cliaddr);
    char                buffer[BUF_SIZE];

    (void)arg;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(TCP_PORT);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        int n = read(newsockfd, buffer, BUF_SIZE-1);
        if (n < 0) { perror("read"); continue; }
        buffer[n] = '\0';
        printf("Received TCP msg:\n%s\n", buffer);

        write(newsockfd, "OK", 2);

        close(newsockfd);
    }

    close(sockfd);
}