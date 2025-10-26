#include "../inc/chat42.h"

pthread_mutex_t hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client        *users_table[TABLE_MAX_SIZE];
char            *own_user_id;
int             udp_sockfd;

void cleanup_and_exit(int signo)
{
    (void)signo;
    memcpy(own_user_id, "off;", 4);
    size_t id_len = strlen(own_user_id);
	pthread_mutex_lock(&hash_table_mutex);

	for (int i = 0; i < TABLE_MAX_SIZE; i++) {

		if (users_table[i]) {
			sendto(udp_sockfd, own_user_id, id_len, 0,(struct sockaddr*)&users_table[i]->addr, sizeof(users_table[i]->addr));
			free(users_table[i]);
			users_table[i] = NULL;
		}
	}

	pthread_mutex_unlock(&hash_table_mutex);

    if (udp_sockfd)
	    close(udp_sockfd);
    free(own_user_id);
	printf("\nServer shutdown cleanly.\n");
	exit(0);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        return (printf(HELP_MSG), 1);
    }

    if (strncmp("--connect", argv[1], 9) != 0) {
        handle_commands(argv[1]);
    }
    else {

        own_user_id = get_user_id();
        if (!own_user_id)
            return (1);

        pthread_t       udp_thread;
        pthread_t       tcp_thread;


        hashtable_init(users_table);

        signal(SIGINT, cleanup_and_exit);
        if (pthread_create(&udp_thread, NULL, udp_thread_func, own_user_id) == -1)
            return (cleanup_and_exit(1), 1);
        if (pthread_create(&tcp_thread, NULL, tcp_thread_func, NULL) == -1)
            return (pthread_cancel(udp_thread), cleanup_and_exit(1), 1);

        char line[BUF_SIZE];
        while (fgets(line, BUF_SIZE, stdin)) {

            size_t line_len = strcspn(line, "\n");
            line[line_len] = 0;
            if (strncmp(line, "chat42 --disconnect", 19) == 0)
                break ;
            handle_commands(line);
        }
        pthread_cancel(udp_thread);
        pthread_cancel(tcp_thread);
        pthread_join(udp_thread, NULL);
        pthread_join(tcp_thread, NULL);
        cleanup_and_exit(1);
    }
    return (0);
}