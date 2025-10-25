#include "../inc/chat42.h"

void    *udp_thread_func(void* user) {

    char               user_id[BUF_SIZE];
	strncpy(user_id, (char *)user, BUF_SIZE - 1);
	user_id[BUF_SIZE - 1] = '\0';
	size_t user_id_len = strlen(user_id);


    int                sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    char               buffer[BUF_SIZE];
    socklen_t          len = sizeof(cliaddr);
    

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return (NULL);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);


    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        return (NULL);

    while (1) {

        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE-1, 0, (struct sockaddr*)&cliaddr, &len);
        buffer[n] = '\0';
        printf("%s (online)\n", buffer);
        sendto(sockfd, user_id, user_id_len, 0, (struct sockaddr*)&cliaddr, len);
        sleep(5);
    }

    close(sockfd);
}