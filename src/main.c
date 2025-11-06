#include "../inc/chat42.h"

pthread_mutex_t			hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_manager 				*manager;
volatile sig_atomic_t	shutdown_requested = 0;

void signal_handler(int signo) {(void)signo;shutdown_requested = 1;}

int	init_manager(void) {

	manager->users_table = calloc(TABLE_MAX_SIZE, sizeof(t_client *));
	if (!manager->users_table)
		return (1);
    hashtable_init(manager->users_table);
	manager->udp = calloc(1, sizeof(t_udp));
	if (!manager->udp)
		return (free(manager->users_table), 1);
	manager->tcp = calloc(1, sizeof(t_tcp));
	if (!manager->tcp)
		return (free(manager->udp), free(manager->users_table), 1);
	return (0);
}

int main(int argc, char **argv) {

    if (argc < 2) return (printf(HELP_MSG), 1);
    else if (strncmp("--connect", argv[1], 9) != 0) return (handle_commands(argv[1]), 0);

    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	manager = calloc(1, sizeof(t_manager));
	if (!manager)
		return (1);
	if (init_manager())
		return (free(manager), 1);
	
	
    if (udp_struct_init(manager->udp, users_table))
		return (cleanup_and_exit(), 1);
    if (tcp_struct_init(manager->tcp, users_table))
		return (cleanup_and_exit(), 1);

	handle_commands();
    cleanup_and_exit();
    return (0);
}

