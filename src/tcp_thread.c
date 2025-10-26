#include "../inc/chat42.h"

void send_tcp_message(t_client *client, const char *msg)
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) return ;

	if (connect(sockfd, (struct sockaddr*)&client->addr, sizeof(client->addr)) < 0) {
		close(sockfd);
		return ;
	}

	write(sockfd, msg, strlen(msg));
	close(sockfd);
}

void* tcp_thread_func(void* arg) {

    int                 sockfd, newsockfd, opt = 1;
    struct sockaddr_in  servaddr, cliaddr;
    socklen_t           clilen;
    char                buffer[BUF_SIZE];

    (void)arg;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return (NULL);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(TCP_PORT + 1);

	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		close(sockfd);
		return (NULL);
	}

	if (listen(sockfd, 5) < 0) {
		perror("listen");
		close(sockfd);
		return (NULL);
	}
    clilen = sizeof(cliaddr);
    while (1) {

        newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if (newsockfd < 0) continue;

        int n = read(newsockfd, buffer, BUF_SIZE-1);
        if (n <= 0) { close(newsockfd); continue; }

        buffer[n] = '\0';
        printf("\n[MSG] %s\n> ", buffer);
        fflush(stdout);

        write(newsockfd, "OK", 2);
        close(newsockfd);
    }

    close(sockfd);
}