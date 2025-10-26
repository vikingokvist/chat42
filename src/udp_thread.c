#include "../inc/chat42.h"


void    *udp_thread_func(void* user) {

    char               user_id[BUF_SIZE];
	strncpy(user_id, (char *)user, BUF_SIZE - 1);
	user_id[BUF_SIZE - 1] = '\0';
	size_t user_id_len = strlen(user_id);


    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    char               clients_buffer[BUF_SIZE];
    socklen_t          len = sizeof(cliaddr);
    

    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0)
        return (NULL);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);


    if (bind(udp_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        return (NULL);

    while (1) {

        ssize_t n = recvfrom(udp_sockfd, clients_buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&cliaddr, &len);
        if (n > 0) {
            
            clients_buffer[n] = '\0';
            pthread_mutex_lock(&hash_table_mutex);
            if (strncmp("onn;", clients_buffer, 4) == 0) {
                if (hashtable_search(users_table, clients_buffer) == NULL) {
                      t_client *new_user = hashtable_add(clients_buffer, &cliaddr);
                      if (!new_user) {
                          pthread_mutex_unlock(&hash_table_mutex);
                          continue ;
                      }
                      if (hashtable_insert(users_table, new_user)) {
                          free(new_user);
                          pthread_mutex_unlock(&hash_table_mutex);
                          continue ;
                      }
                      print_client(users_table);
                  }

            }
            else if (strncmp("off;", clients_buffer, 4) == 0) {
                hashtable_delete(users_table, user);
                pthread_mutex_unlock(&hash_table_mutex);
                print_client(users_table);
                continue ;
            }
            pthread_mutex_unlock(&hash_table_mutex);
        }
        sendto(udp_sockfd, user_id, user_id_len, 0, (struct sockaddr*)&cliaddr, len);

    }

    close(udp_sockfd);
}