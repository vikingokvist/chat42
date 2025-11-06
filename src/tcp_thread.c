#include "../inc/chat42.h"


char *strjoin(const char *s1, const char *s2)
{
	size_t len1, len2;
	char *res;

	if (!s1 || !s2)
		return (NULL);

	len1 = strlen(s1);
	len2 = strlen(s2);

	res = malloc(len1 + len2 + 1);
	if (!res)
		return (NULL);

	memcpy(res, s1, len1);
	memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	return (res);
}

int tcp_struct_init(t_tcp  *tcp, t_client **users_table) {

    tcp->OWN_USERNAME = get_user_name();
    tcp->OWN_USER_MACHINE_ID = get_user_info(0);
    tcp->OWN_USER_MACHINE_LEN = strlen(tcp->OWN_USER_MACHINE_ID);
    tcp->sockfd = -1;
    tcp->users_table = users_table;


    if ((tcp->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        perror("tcp socket");
        return (1);
    }
    tcp->opt = 1;
    setsockopt(tcp->sockfd, SOL_SOCKET, SO_REUSEADDR, &tcp->opt, sizeof(tcp->opt));


    memset(&tcp->receive_addr, 0, sizeof(tcp->receive_addr));
    tcp->receive_addr.sin_family = AF_INET;
    tcp->receive_addr.sin_addr.s_addr = INADDR_ANY;
    tcp->receive_addr.sin_port = htons(TCP_PORT);


    if (bind(tcp->sockfd, (struct sockaddr*)&tcp->receive_addr, sizeof(tcp->receive_addr)) < 0) {
        
        perror("tcp bind");
        return (1);
    }

    if (listen(tcp->sockfd, 5) < 0) {

        perror("tcp listen");
        return (1);
    }
    
    if (pthread_create(&tcp->send_thread, NULL, tcp_thread_func, (void*)tcp) == -1) {

        perror("tcp thread");
        return (1);
    }

    return (0);
}

void* tcp_thread_func(void* arg) {

    t_tcp               *tcp = (t_tcp *)arg;
    int                 newsockfd;
    struct sockaddr_in  cliaddr;
    socklen_t           clilen = sizeof(cliaddr);
    char                buffer[BUF_SIZE];

    while (1) {

        newsockfd = accept(tcp->sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if (newsockfd < 0) continue;

        int n = read(newsockfd, buffer, BUF_SIZE-1);
        if (n <= 0) {
            close(newsockfd);
            continue;
        }
        buffer[n] = '\0';
        if (n > 0) {
            
            printf("\n[MSG] %s\n> ", buffer);
            fflush(stdout);
            write(newsockfd, "OK", 2);
            close(newsockfd);
        }

    }
}

void send_tcp_message(t_client *client, const char *msg)
{
	int         client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (client_sockfd < 0)
        return ;

	if (connect(client_sockfd, (struct sockaddr*)&client->CLIENT_ADDR, sizeof(client->CLIENT_ADDR)) < 0) {

        perror("tcp connect");
		close(client_sockfd);
		return ;
	}
    char *new_msg = strjoin(client->MACHINE_ID, "::");
    char *tmp = strjoin(new_msg, client->USERNAME);
    free(new_msg);
    char *tmp2 = strjoin(tmp, ": ");
    free(tmp);
    char *tmp3 = strjoin(tmp2, msg);
    free(tmp2);
	write(client_sockfd, tmp3, strlen(tmp3));
	close(client_sockfd);
    free(tmp3);
}
