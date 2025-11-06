#include "../inc/chat42.h"
#include <sys/select.h>

pthread_mutex_t			hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client				**users_table;
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
	manager->colour_a = BOLD_BLUE;
	manager->colour_b = BOLD_YELLOW;
	manager->OWN_USERNAME = get_user_name();
	manager->OWN_MACHINE_ID = get_machine_id();
	return (0);
}

int main(int argc, char **argv) {

    if (argc < 2) return (printf(HELP_MSG), 1);
    else if (strncmp("--connect", argv[1], 9) != 0) return (1);

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

void handle_commands(const char *input, t_tcp *tcp) {

	char cmd[32], arg[128], msg[BUF_SIZE]; 
	int offset = 0; 


	if (sscanf(input, "%31s%n", cmd, &offset) != 1) return ; 
	if (strcmp(cmd, "chat42") != 0) return ; 
	input += offset; 
	if (sscanf(input, "%127s%n", arg, &offset) != 1) return ; 
	input += offset; 
	while (*input == ' ') 
		input++;
	if (strcmp(arg, "--disconnect") == 0) { 
		cleanup_and_exit(); 
		return ; 
	} 
	if (*input == '\0') { 
		printf("usage: chat42 <username> \"message\"\n"); 
		fflush(stdout); 
		return ; 
	} 
	strncpy(msg, input, BUF_SIZE - 1); 
	msg[BUF_SIZE - 1] = '\0'; 
	pthread_mutex_lock(&hash_table_mutex); 
	t_client *client = hashtable_search(users_table, arg); 
	if (!client) { 
		printf("User '%s' not found\n", arg); 
		fflush(stdout);
		pthread_mutex_unlock(&hash_table_mutex); 
		return ; 
	} 
	//printf("\"%s\" - sent to %s::%s\n", msg, client->MACHINE_ID, client->USERNAME); 
	fflush(stdout); 
	send_tcp_message(client, msg, tcp->OWN_USER_ID); 
	pthread_mutex_unlock(&hash_table_mutex);
}



