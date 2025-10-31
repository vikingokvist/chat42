#include "../inc/chat42.h"


int udp_struct_init(t_udp  *udp) {

    udp->OWN_USERNAME = get_user_name();
    udp->OWN_USER_MACHINE_ID = get_user_info(1);
    udp->OWN_USER_MACHINE_LEN = strlen(udp->OWN_USER_MACHINE_ID);
    udp->sockfd = -1;
    udp->users_table = users_table;
    
    if ((udp->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

        perror("udp socket");
        return (1);
    }
    udp->opt = 1;

    setsockopt(udp->sockfd, SOL_SOCKET, SO_REUSEADDR, &udp->opt, sizeof(udp->opt));
    setsockopt(udp->sockfd, SOL_SOCKET, SO_BROADCAST, &udp->opt, sizeof(udp->opt));    

    memset(&udp->receive_addr, 0, sizeof(udp->receive_addr));
    udp->receive_addr.sin_family = AF_INET;
    udp->receive_addr.sin_addr.s_addr = INADDR_ANY;
    udp->receive_addr.sin_port = htons(UDP_PORT);

    if (bind(udp->sockfd, (struct sockaddr*)&udp->receive_addr, sizeof(udp->receive_addr)) < 0) {
    
        perror("udp bind");
        return (1);
    }

	memset(&udp->send_addr, 0, sizeof(udp->send_addr));
	udp->send_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, SEND_IP, &udp->send_addr.sin_addr) <= 0) {
        perror("inet_pton for send_addr failed");
        return (1);
    }
	udp->send_addr.sin_port = htons(UDP_PORT);

    if (pthread_create(&udp->thread, NULL, udp_thread_func, (void*)udp) == -1) {

        perror("udp thread");
        return (1);
    }
    return (0);
}

void    *udp_thread_func(void* arg) {

    t_udp              *udp = (t_udp *)arg;
    char               clients_buffer[BUF_SIZE];
    struct sockaddr_in new_cliaddr;
    socklen_t          addr_len = sizeof(new_cliaddr);

    while (1) {

        sendto(udp->sockfd, udp->OWN_USER_MACHINE_ID, udp->OWN_USER_MACHINE_LEN, 0, (struct sockaddr*)&udp->send_addr, sizeof(udp->send_addr));
        ssize_t n = recvfrom(udp->sockfd, clients_buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&new_cliaddr, &addr_len);
        if (n > 0) {
            
            clients_buffer[n] = '\0';
            char status, machine_id[64], username[64];
            int tcp_port;

            if  (sscanf(clients_buffer, "%c;%63[^;];%63[^;];%d", &status, machine_id, username, &tcp_port) == 4) {
                    if (status == '1') {

                        if (udp_add_user(udp, &new_cliaddr, machine_id, username, tcp_port)) continue ;
                    }
                    else if (status == '0') {

                        hashtable_delete(udp->users_table, username); continue ;
                    }
            }
        }
        
    }
}

void    udp_delete_user(t_udp *udp,  char *username) {

    pthread_mutex_lock(&hash_table_mutex);
    hashtable_delete(udp->users_table, username);
    pthread_mutex_unlock(&hash_table_mutex);
}

int    udp_add_user(t_udp *udp, struct sockaddr_in *new_cliaddr, char *machine_id, char *username, int tcp_port) {

    pthread_mutex_lock(&hash_table_mutex);
    if (hashtable_search(udp->users_table, username) == NULL) {

        t_client *new_user = hashtable_add(new_cliaddr, username, machine_id, tcp_port);
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

