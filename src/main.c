#include "../inc/chat42.h"


pthread_mutex_t			hash_table_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			colour_mutex 	   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			autocomplete_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client			  **users_table;
t_manager 			   *manager;


int		main(void) {

	signal(SIGINT, &signal_handler);
	signal(SIGTERM, &signal_handler);

	manager = calloc(1, sizeof(t_manager));
	if (!manager) {
		return (1);
	}
	if (init_manager()) {
		return (free(manager), 1);
	}
	users_table = manager->users_table;
    if (udp_struct_init(manager)) {
		return (exit_manager(), 1);
	}
    if (tcp_struct_init(manager)) {
		return (exit_manager(), 1);
	}


	if (pthread_create(&manager->udp->send_thread, NULL, udp_send, (void*)manager->udp) == -1) {
        return (perror("udp send thread"), exit_manager(), 1);
    }
    if (pthread_create(&manager->udp->receive_thread, NULL, udp_receive, (void*)manager->udp) == -1) {
        return (perror("udp send thread"), exit_manager(), 1);
    }
	if (pthread_create(&manager->tcp->send_thread, NULL, tcp_thread_func, (void*)manager->tcp) == -1) {
        return (perror("tcp thread"), exit_manager(), 1);
    }

	command_loop();
    exit_manager();
    return (0);
}

