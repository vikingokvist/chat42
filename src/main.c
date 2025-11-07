#include "../inc/chat42.h"
#include <sys/select.h>

pthread_mutex_t			hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			colour_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			msg_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client				**users_table;
t_manager 				*manager;
volatile sig_atomic_t	shutdown_requested = 0;
void signal_handler(int signo) {(void)signo;shutdown_requested = 1;}


int main(int argc, char **argv) {

    if (argc < 2)
		return (printf(HELP_MSG), 1);
	else if (strncmp(argv[1], "--connect", 9))
		return (1);


    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	manager = calloc(1, sizeof(t_manager));
	if (!manager)
		return (1);
	if (init_manager())
		return (free(manager), 1);
	users_table = manager->users_table;



    if (udp_struct_init(manager))
		return (cleanup_and_exit(), 1);
    if (tcp_struct_init(manager))
		return (cleanup_and_exit(), 1);


	char line[BUF_SIZE]; 
	while (!shutdown_requested) { 

		printf("> ");
		fflush(stdout);
		fgets(line, BUF_SIZE, stdin); 
		size_t line_len = strcspn(line, "\n"); 
		line[line_len] = 0; 
		if (strncmp(line, "chat42 --disconnect", 19) == 0)
			break ; 
		handle_commands(line, manager->tcp); 
	}
    cleanup_and_exit();
    return (0);
}

