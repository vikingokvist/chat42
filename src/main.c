#include "../inc/chat42.h"
#include <sys/select.h>

pthread_mutex_t			hash_table_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			colour_mutex 	   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t			autocomplete_mutex  = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t	shutdown_requested = 0;
t_client			  **users_table;
t_manager 			   *manager;

void signal_handler(int signo) {(void)signo;shutdown_requested = 1;rl_done = 1;write(STDOUT_FILENO, "\n", 1); }

int		main(int argc, char **argv) {

    if (argc < 2) {

		return (printf(HELP_MSG), 1);
	}
	else if (strncmp(argv[1], "--connect", 9)) {

		return (1);
	}

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
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

	rl_attempted_completion_function = init_autocomplete;
	rl_bind_key('\t', rl_complete);
	while (!shutdown_requested) { 

		char *input = readline("");
 		if (shutdown_requested) {
			if (input)
            	free(input);
            break ;
        }
		if (*input)
			add_history(input);
		handle_commands(input, manager->tcp);
		free(input);
	}
    exit_manager();
    return (0);
}

