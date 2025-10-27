#include "../inc/chat42.h"

pthread_mutex_t hash_table_mutex = PTHREAD_MUTEX_INITIALIZER;
t_client        *users_table[TABLE_MAX_SIZE];
t_udp    udp_struct;
t_tcp    tcp_struct;

const char *LIST_COLOURS = 
BLACK "BLACK " RESET
RED "RED " RESET
GREEN "GREEN " RESET
YELLOW "YELLOW " RESET
BLUE "BLUE " RESET
MAGENTA "MAGENTA " RESET
CYAN "CYAN " RESET
WHITE "WHITE " RESET
BOLD_BLACK "BOLD_BLACK " RESET
BOLD_RED "BOLD_RED \n" RESET
BOLD_GREEN "BOLD_GREEN " RESET
BOLD_YELLOW "BOLD_YELLOW " RESET
BOLD_BLUE "BOLD_BLUE " RESET
BOLD_MAGENTA "BOLD_MAGENTA " RESET
BOLD_CYAN "BOLD_CYAN " RESET
BOLD_WHITE "BOLD_WHITE\n" RESET
BG_BLACK "BG_BLACK" RESET " "
BG_RED "BG_RED" RESET " "
BG_GREEN "BG_GREEN" RESET " "
BG_YELLOW "BG_YELLOW" RESET " "
BG_BLUE "BG_BLUE" RESET " "
BG_MAGENTA "BG_MAGENTA" RESET " "
BG_CYAN "BG_CYAN" RESET " "
BG_WHITE "BG_WHITE" RESET " \n";

void handle_commands(const char *input)
{
	char line_copy[BUF_SIZE];
	char *cmd, *arg, *msg;

	strncpy(line_copy, input, BUF_SIZE - 1);
	line_copy[BUF_SIZE - 1] = '\0';

	cmd = strtok(line_copy, " ");
	if (!cmd || strcmp(cmd, "chat42") != 0)
		return ;

	arg = strtok(NULL, " ");
	if (!arg)
		return ;

	if (strcmp(arg, "--disconnect") == 0) {
		memcpy(udp_struct.PLACE_USER_ID, "0;", 4);
		cleanup_and_exit(0);
	}
	// else if (strcmp(arg, "--help") == 0) {
	// 	printf("%s", HELP_MSG);
	// 	fflush(stdout);
	// 	return ;
	// }
	// else if (strcmp(arg, "--colour-list") == 0) {
	// 	printf("%s", LIST_COLOURS);
	// 	fflush(stdout);
	// 	return ;
	// }
	else {

		msg = strtok(NULL, "");
		if (!msg || !*msg) {
			printf("usage: chat42 <username> \"message\"\n");
			fflush(stdout);
			return ;
		}

		pthread_mutex_lock(&hash_table_mutex);
		t_client *client = hashtable_search(users_table, arg);
		if (!client) {
			printf("User '%s' not found\n", arg);
			fflush(stdout);
			pthread_mutex_unlock(&hash_table_mutex);
			return ;
		}
		send_tcp_message(client, msg);
		pthread_mutex_unlock(&hash_table_mutex);
	}
}


int main(int argc, char **argv) {

    if (argc < 2) {
        return (printf(HELP_MSG), 1);
    }
    else if (strncmp("--connect", argv[1], 9) != 0) {
        return (handle_commands(argv[1]), 0);
    }
    signal(SIGINT, cleanup_and_exit);
    hashtable_init(users_table);

    if (udp_struct_init(&udp_struct))
        return (cleanup_and_exit(1), 1);
    if (tcp_struct_init(&tcp_struct))
        return (cleanup_and_exit(1), 1);
    
    char line[BUF_SIZE];

    while (fgets(line, BUF_SIZE, stdin)) {

        size_t line_len = strcspn(line, "\n");
        line[line_len] = 0;
        if (strncmp(line, "chat42 --disconnect", 19) == 0)
            break ;
        handle_commands(line);

    }

    cleanup_and_exit(1);
    return (0);
}