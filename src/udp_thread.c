#include "../inc/chat42.h"


int udp_struct_init(t_udp  *udp_struct) {

    udp_struct->OWN_USERNAME = get_user_info(0);
    udp_struct->OWN_USER_MACHINE_ID = get_user_info(1);
    udp_struct->OWN_USER_MACHINE_LEN = strlen(udp_struct->OWN_USER_MACHINE_ID);
    udp_struct->sockfd = 1;
    udp_struct->users_table = users_table;
    udp_struct->opt = 1;

    udp_struct->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_struct->sockfd < 0) {
        perror("udp socket");
        return (1);
    }
    setsockopt(udp_struct->sockfd, SOL_SOCKET, SO_REUSEADDR, &udp_struct->opt, sizeof(udp_struct->opt));
    setsockopt(udp_struct->sockfd, SOL_SOCKET, SO_BROADCAST, &udp_struct->opt, sizeof(udp_struct->opt));    
    memset(&udp_struct->servaddr, 0, sizeof(udp_struct->servaddr));
    udp_struct->servaddr.sin_family = AF_INET;
    udp_struct->servaddr.sin_addr.s_addr = INADDR_ANY;
    udp_struct->servaddr.sin_port = htons(UDP_PORT);
    if (bind(udp_struct->sockfd, (struct sockaddr*)&udp_struct->servaddr, sizeof(udp_struct->servaddr)) < 0) {
    
        perror("udp bind");
        return (1);
    }

    if (pthread_create(&udp_struct->udp_thread, NULL, udp_thread_func, (void*)udp_struct) == -1) {
        perror("udp thread");
        return (1);
    }
    return (0);
}

void    *udp_thread_func(void* udp_struct) {

    t_udp              *udp = (t_udp *)udp_struct;
    char               clients_buffer[BUF_SIZE];
    struct sockaddr_in new_cliaddr;
    socklen_t          client_len = sizeof(new_cliaddr);
    
    while (1) {

        ssize_t n = recvfrom(udp->sockfd, clients_buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&new_cliaddr, &client_len);
        if (n > 0) {
            
            clients_buffer[n] = '\0';
            char status;
            char new_machine_id[64];
            char new_username[64];

            if  (sscanf(clients_buffer,  "%c;%63[^:]:%*[:]%63s", &status, new_machine_id, new_username) == 3) {
                
                    if (status == '1') {

                        if (udp_add_user(udp, &new_cliaddr, new_machine_id, new_username)) continue ;
                    }
                    else if (status == '0') {

                        hashtable_delete(udp->users_table, new_username); continue;
                    }
            }
        }
        sendto(udp->sockfd, udp->OWN_USER_MACHINE_ID, udp->OWN_USER_MACHINE_LEN, 0, (struct sockaddr*)&new_cliaddr, sizeof(new_cliaddr));
        sleep(5);
    }
}

void    udp_delete_user(t_udp *udp,  char *new_username) {

    pthread_mutex_lock(&hash_table_mutex);
    hashtable_delete(udp->users_table, new_username);
    pthread_mutex_unlock(&hash_table_mutex);
}

int    udp_add_user(t_udp *udp, struct sockaddr_in *new_cliaddr, char *new_machine_id, char *new_username) {

    pthread_mutex_lock(&hash_table_mutex);
    if (hashtable_search(udp->users_table, new_username) == NULL) {

        t_client *new_user = hashtable_add(new_cliaddr, new_username, new_machine_id);
        if (!new_user) {

            pthread_mutex_unlock(&hash_table_mutex);
            return (1) ;
        }
        if (hashtable_insert(udp->users_table, new_user)) {

            free(new_user);
            pthread_mutex_unlock(&hash_table_mutex);
            return (1);
        }
        print_client(udp->users_table);
        pthread_mutex_unlock(&hash_table_mutex);
        return (0);
    }
    pthread_mutex_unlock(&hash_table_mutex);
    return (1);
}

