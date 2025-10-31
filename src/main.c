#include "../inc/chat42.h"

pthread_mutex_t hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client        *users_table[TABLE_MAX_SIZE];
t_udp           udp;
t_tcp           tcp;
volatile sig_atomic_t shutdown_requested = 0;

void signal_handler(int signo) {(void)signo;shutdown_requested = 1;}

int main(int argc, char **argv) {

    if (argc < 2) {
        return (printf(HELP_MSG), 1);
    }
    else if (strncmp("--connect", argv[1], 9) != 0) {
        return (handle_commands(argv[1]), 0);
    }

    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    hashtable_init(users_table);

    if (udp_struct_init(&udp))
        return (cleanup_and_exit(), 1);
    // if (tcp_struct_init(&tcp))
    //     return (cleanup_and_exit(1), 1);
    
    char line[BUF_SIZE];

    while (fgets(line, BUF_SIZE, stdin)) {

        size_t line_len = strcspn(line, "\n");
        line[line_len] = 0;
        if (strncmp(line, "chat42 --disconnect", 19) == 0)
            break ;
        handle_commands(line);

    }
    cleanup_and_exit();
    return (0);
}

void handle_commands(const char *input)
{
	char cmd[32], arg[128], msg[BUF_SIZE];
	int offset = 0;

	if (sscanf(input, "%31s%n", cmd, &offset) != 1)
		return ;

	if (strcmp(cmd, "chat42") != 0)
		return ;

	input += offset;

	if (sscanf(input, "%127s%n", arg, &offset) != 1)
		return ;

	input += offset;
	while (*input == ' ') input++;

	if (strcmp(arg, "--disconnect") == 0) {
		memcpy(udp.OWN_USER_MACHINE_ID, "0;", 2);
		cleanup_and_exit();
		return ;
	}
	else {
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
		printf("\"%s\" - sent to %s::%s\n", msg, client->MACHINE_ID, client->USERNAME);
		fflush(stdout);
		send_tcp_message(client, msg);
		pthread_mutex_unlock(&hash_table_mutex);
	}
}