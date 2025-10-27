#include "../inc/chat42.h"


void* tcp_thread_func(void* tcp_struct) {

    t_tcp               *tcp = (t_tcp *)tcp_struct;
    int                 newsockfd;
    struct sockaddr_in  cliaddr;
    socklen_t           clilen = sizeof(cliaddr);
    char                buffer[BUF_SIZE];

    while (1) {

        newsockfd = accept(tcp->sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if (newsockfd < 0) continue;

        int n = read(newsockfd, buffer, BUF_SIZE-1);
        if (n <= 0) { close(newsockfd); continue; }

        buffer[n] = '\0';
        printf("\n[MSG] %s\n> ", buffer);
        fflush(stdout);

        write(newsockfd, "OK", 2);
        close(newsockfd);
    }
}


int tcp_struct_init(t_tcp  *tcp_struct) {

    tcp_struct->USER_ID = get_user_info(0);
    tcp_struct->PLACE_USER_ID = get_user_info(1);
    tcp_struct->USER_ID_LEN = strlen(tcp_struct->USER_ID);
    tcp_struct->sockfd = -1;
    tcp_struct->users_table = users_table;
    tcp_struct->opt = -1;

    tcp_struct->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_struct->sockfd < 0) {

        return (1);
    }

    setsockopt(tcp_struct->sockfd, SOL_SOCKET, SO_REUSEADDR, &tcp_struct->opt, sizeof(tcp_struct->opt));
    memset(&tcp_struct->servaddr, 0, sizeof(tcp_struct->servaddr));
    tcp_struct->servaddr.sin_family = AF_INET;
    tcp_struct->servaddr.sin_addr.s_addr = inet_addr("192.168.122.255");
    tcp_struct->servaddr.sin_port = htons(TCP_PORT);

    if (bind(tcp_struct->sockfd, (struct sockaddr*)&tcp_struct->servaddr, sizeof(tcp_struct->servaddr)) < 0) {
        
        return (1);
    }
    if (listen(tcp_struct->sockfd, 5) < 0) {
        perror("listen");
        return (1);
    }
    if (pthread_create(&tcp_struct->tcp_thread, NULL, tcp_thread_func, (void*)tcp_struct) == -1) {

        return (1);
    }
    return (0);
}

void send_tcp_message(t_client *client, const char *msg)
{
	int         client_sockfd;
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sockfd < 0) return ;

	if (connect(client_sockfd, (struct sockaddr*)&client->addr, sizeof(client->addr)) < 0) {
		close(client_sockfd);
		return ;
	}

	write(client_sockfd, msg, strlen(msg));
	close(client_sockfd);
}
