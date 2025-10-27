#include "../inc/chat42.h"


void    *udp_thread_func(void* udp_struct) {

    t_udp              *udp = (t_udp *)udp_struct;
    char               clients_buffer[BUF_SIZE];
    struct sockaddr_in cliaddr;
    socklen_t          client_len = sizeof(cliaddr);
    

    while (1) {

        ssize_t n = recvfrom(udp->sockfd, clients_buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&cliaddr, &client_len);
        if (n > 0) {
            
            clients_buffer[n] = '\0';
            pthread_mutex_lock(&hash_table_mutex);
            if (strncmp("1;", clients_buffer, 2) == 0) {

                if (hashtable_search(udp->users_table, clients_buffer) == NULL) {
                    
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
                      print_client(udp->users_table);
                  }

            }
            else if (strncmp("0;", clients_buffer, 2) == 0) {

                hashtable_delete(udp->users_table, clients_buffer);
                pthread_mutex_unlock(&hash_table_mutex);
                print_client(udp->users_table);
                continue ;
            }
            pthread_mutex_unlock(&hash_table_mutex);
        }
        sendto(udp->sockfd, udp->PLACE_USER_ID, udp->USER_ID_LEN, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

    }
}


int udp_struct_init(t_udp  *udp_struct) {

    udp_struct->USER_ID = get_user_info(0);
    udp_struct->PLACE_USER_ID = get_user_info(1);
    udp_struct->USER_ID_LEN = strlen(udp_struct->PLACE_USER_ID);
    udp_struct->sockfd = -1;
    udp_struct->users_table = users_table;
    udp_struct->opt = 1;

    udp_struct->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_struct->sockfd < 0) {

        return (1);
    }
    setsockopt(udp_struct->sockfd, SOL_SOCKET, SO_BROADCAST, &udp_struct->opt, sizeof(udp_struct->opt));
    memset(&udp_struct->servaddr, 0, sizeof(udp_struct->servaddr));
    udp_struct->servaddr.sin_family = AF_INET;
    udp_struct->servaddr.sin_addr.s_addr = INADDR_ANY;
    udp_struct->servaddr.sin_port = htons(UDP_PORT);
    if (bind(udp_struct->sockfd, (struct sockaddr*)&udp_struct->servaddr, sizeof(udp_struct->servaddr)) < 0) {
    
        return (1);
    }

    if (pthread_create(&udp_struct->udp_thread, NULL, udp_thread_func, (void*)udp_struct) == -1) {

        return (1);
    }
    return (0);
}
