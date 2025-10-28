#include "../inc/chat42.h"

pthread_mutex_t hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client        *users_table[TABLE_MAX_SIZE];
t_udp    udp_struct;
t_tcp    tcp_struct;
volatile sig_atomic_t shutdown_requested = 0;


void signal_handler(int signo) {
    (void)signo;
    shutdown_requested = 1;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        return (printf(HELP_MSG), 1);
    }
    else if (strncmp("--connect", argv[1], 9) != 0) {
        return (handle_commands(argv[1]), 0);
    }
    signal(SIGINT, cleanup_and_exit);
	signal(SIGTERM, signal_handler);
    hashtable_init(users_table);

    if (udp_struct_init(&udp_struct))
        return (cleanup_and_exit(), 1);
    // if (tcp_struct_init(&tcp_struct))
    //     return (cleanup_and_exit(1), 1);
    
    char line[BUF_SIZE];

    while (!shutdown_requested) {

		fgets(line, BUF_SIZE, stdin);
        size_t line_len = strcspn(line, "\n");
        line[line_len] = 0;
        if (strncmp(line, "chat42 --disconnect", 19) == 0)
            break ;
        handle_commands(line);

    }
    cleanup_and_exit();
    return (0);
}