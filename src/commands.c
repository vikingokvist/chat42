#include "../inc/chat42.h"


void handle_commands(void)
{
	char line[BUF_SIZE];
	char cmd[32], arg[128];
	const char *rest;
	int offset = 0;

	while (!shutdown_requested) {
		if (!fgets(line, sizeof(line), stdin))
			break ;

		size_t len = strcspn(line, "\n");
		line[len] = '\0';

		if (strncmp(line, "chat42", 6) != 0)
			continue ;

		if (sscanf(line, "%31s %127s%n", cmd, arg, &offset) < 2)
			continue ;

		rest = line + offset;
		while (*rest == ' ') rest++;

		process_chat42_command(arg, rest);
	}
}

void process_chat42_command(const char *arg, const char *rest)
{

	pthread_mutex_lock(&hash_table_mutex);
	t_client *client = hashtable_search(users_table, (char *)arg);
	if (!client) {
		printf("User '%s' not found\n", arg);
		pthread_mutex_unlock(&hash_table_mutex);
		return ;
	}
	if (!rest || *rest == '\0') {
		printf("usage: chat42 <username> \"message\"\n");
		pthread_mutex_unlock(&hash_table_mutex);
		return ;
	}
	send_tcp_message(client, rest);
	pthread_mutex_unlock(&hash_table_mutex);
}



/*
	if (strcmp(arg, "--help") == 0)
		handle_help();
	else if (strcmp(arg, "--version") == 0)
		handle_version();
	else if (strcmp(arg, "--login") == 0)
		handle_login();
	else if (strcmp(arg, "--disconnect") == 0)
		cleanup_and_exit();
	else if (strcmp(arg, "--colour-list") == 0)
		handle_colour_list();
	else if (strncmp(arg, "--colour-set", 12) == 0)
		handle_colour_set(rest);
	else if (strcmp(arg, "--block") == 0) {
		if (!rest || *rest == '\0')
			printf("usage: chat42 --block <username>\n");
		else
			handle_block(rest);
	}
	else {
*/