#include "../inc/chat42.h"


int udp_struct_init(void *manager) {

    t_manager *man = (t_manager*)manager;
    t_udp *udp = man->udp;
    udp->OWN_USER_ID = build_user_info(man->OWN_MACHINE_ID, man->OWN_USERNAME);
    udp->users_table = man->users_table;
    if ((udp->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return (perror("udp socket"), 1);
    }

    udp->opt = 1;
    setsockopt(udp->sockfd, SOL_SOCKET, SO_REUSEADDR, &udp->opt, sizeof(udp->opt));
    setsockopt(udp->sockfd, SOL_SOCKET, SO_BROADCAST, &udp->opt, sizeof(udp->opt));    


    memset(&udp->receive_addr, 0, sizeof(udp->receive_addr));
    udp->receive_addr.sin_family = AF_INET;
    udp->receive_addr.sin_addr.s_addr = INADDR_ANY;
    udp->receive_addr.sin_port = htons(UDP_PORT);


    if (bind(udp->sockfd, (struct sockaddr*)&udp->receive_addr, sizeof(udp->receive_addr)) < 0) {
        return (perror("udp bind"), 1);
    }

	memset(&udp->send_addr, 0, sizeof(udp->send_addr));
	udp->send_addr.sin_family = AF_INET;
	udp->send_addr.sin_port = htons(UDP_PORT);
    if (inet_pton(AF_INET, SEND_IP, &udp->send_addr.sin_addr) <= 0) {
        return (perror("inet_pton for send_addr failed"), 1);
    }


    if (pthread_create(&udp->send_thread, NULL, udp_send, (void*)udp) == -1) {
        return (perror("udp send thread"), 1);
    }
    if (pthread_create(&udp->receive_thread, NULL, udp_receive, (void*)udp) == -1) {
        return (perror("udp send thread"), 1);
    }

    return (0);
}

void    *udp_send(void* arg) {

    t_udp    *udp = (t_udp *)arg;
    size_t    user_len = strlen(udp->OWN_USER_ID);

    while (1) {

        sendto(udp->sockfd, udp->OWN_USER_ID, user_len, 
                0, (struct sockaddr*)&udp->send_addr, sizeof(udp->send_addr));
        sleep(3);
    }
    return (NULL);
}

void    *udp_receive(void* arg) {

    t_udp              *udp = (t_udp *)arg;
    char               clients_buffer[BUF_SIZE];
    struct sockaddr_in new_cliaddr;
    socklen_t          addr_len = sizeof(new_cliaddr);
    ssize_t            n;

    while (1) {

        if ((n = recvfrom(udp->sockfd, clients_buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&new_cliaddr, &addr_len)) <= 0)
            continue;

        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &new_cliaddr.sin_addr, sender_ip, sizeof(sender_ip));
        if (!strcmp(sender_ip, inet_ntoa(udp->send_addr.sin_addr)) && new_cliaddr.sin_port == udp->receive_addr.sin_port)
            continue;

        clients_buffer[n] = '\0';  
        char machine_id[64], username[64], colour_a[64], colour_b[64]; 
        int status;

        if (sscanf(clients_buffer, UDP_REGEX, &status, machine_id, username, colour_a, colour_b) == 5) {

            pthread_mutex_lock(&hash_table_mutex);
            udp_handle_user(status, udp, &new_cliaddr, machine_id, username, colour_a, colour_b);
            pthread_mutex_unlock(&hash_table_mutex);
        }
    }
    return (NULL);      
}

void    udp_handle_user(int status, t_udp *udp, struct sockaddr_in *new_cliaddr, char *username, char *machine_id, char *colour_a, char *colour_b) {

    if (status == 0) {

        hashtable_delete(udp->users_table, username, machine_id);
        return ;
    }
    else if (status == 1) {

        if (hashtable_search(udp->users_table, username) == NULL) {

            t_client *new = hashtable_add(new_cliaddr, username, machine_id);
            if (!new)
                return ;

            if (hashtable_insert(udp->users_table, new)) {
                free(new);
                return ;
            }
            printf("%s%s%s::%s%s%s %sis now online.%s\n", 
            get_color(colour_a), new->MACHINE_ID, RESET, 
            get_color(colour_b), new->USERNAME, RESET, GREEN, RESET);

            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &new->CLIENT_ADDR.sin_addr, ip, sizeof(ip));
            printf("(%s:%d)\n", ip, ntohs(new->CLIENT_ADDR.sin_port));
        }
    }
    return ;
}



