#include "../inc/chat42.h"

volatile sig_atomic_t	shutdown_requested = 0;

void    signal_handler(int signo) {

	(void)signo;shutdown_requested = 1;
	rl_done = 1;
	write(STDOUT_FILENO, "\n", 1); 
}

void     command_loop(void) {

    rl_attempted_completion_function = init_autocomplete;
	rl_bind_key('\t', rl_complete);

    while (!shutdown_requested) { 

		char *input = readline("");
 		if (shutdown_requested) {
			if (input)
            	free(input);
            return ;
        }
		if (*input)
			add_history(input);
		handle_commands(input, manager->tcp);
		free(input);
	}
    return ;
}

void    handle_commands(const char *input, t_tcp *tcp) {

	char chat42[32], cmd_or_user[128], msg[BUF_SIZE];
    char arg1[64], arg2[64];
	int offset = 0;
    command_func_t func;

	if (sscanf(input, "%31s%n", chat42, &offset) != 1)
        return ; 
	if (strcmp(chat42, "chat42") != 0)
        return ; 
	input += offset;
	if (sscanf(input, "%127s%n", cmd_or_user, &offset) != 1)
        return ;
	input += offset;
	while (*input == ' ') 
		input++;
    
    if ((func = is_server_command(cmd_or_user)) != NULL) {
        if (sscanf(input, "%63[^:]::%63s", arg1, arg2) == 2) {
            func(arg1, arg2);
            return ;
        }
        else
            func(NULL, NULL);
        return ;
    }
	if (*input == '\0') { 

		return ((void)printf("usage: chat42 <username> \"message\"\n"), (void)fflush(stdout)); 
	} 

	strncpy(msg, input, BUF_SIZE - 1); 
	msg[BUF_SIZE - 1] = '\0'; 


	pthread_mutex_lock(&hash_table_mutex); 
	t_client *client = ht_search(users_table, cmd_or_user); 
	if (!client) { 
		printf("User '%s' not found\n", cmd_or_user); fflush(stdout);
		return ((void)pthread_mutex_unlock(&hash_table_mutex)); 
	} 
	fflush(stdout);
    pthread_mutex_lock(&colour_mutex);
	send_tcp_message(client, msg, tcp);
    pthread_mutex_unlock(&colour_mutex);
	pthread_mutex_unlock(&hash_table_mutex);
}

command_func_t    is_server_command(char *cmd) {

    static struct command_entry commands[SERVER_COMMAND_COUNT] = {
	{ "--help", help_command },
	{ "--disconnect", disconnect_command },
	{ "--colour-list", colour_list_command },
	{ "--colour-set", colour_set_command },
	{ "--version", version_command }
    };

    for (int i = 0; i < SERVER_COMMAND_COUNT; i++)
        if (!strcmp(cmd, commands[i].name))
            return (commands[i].func);
    return (NULL);
}

void    help_command(const char *arg1, const char *arg2) { 
    if (arg1 == NULL && arg2 == NULL)
        printf("%s", HELP_MSG);
    else
        printf("Usage: chat42 --help\n");
}


void    disconnect_command(const char *arg1, const char *arg2) {
    if (arg1 == NULL && arg2 == NULL)
        exit_manager(); 
    else
        printf("Usage: chat42 --disconnect\n");
}

void    version_command(const char *arg1, const char *arg2) {
    if (arg1 == NULL && arg2 == NULL)
        printf("%s", VERSION_MSG);
    else
        printf("Usage: chat42 --version\n");
}

void    colour_set_command(const char *arg1, const char *arg2) { 

    if (arg1 != NULL && arg2 != NULL) {
        pthread_mutex_lock(&colour_mutex);
        FILE *fp = fopen(CONFIG_PATH, "w");
        if (!fp) {
            set_default_colours();
		    perror("Unable to set colour\n");
            pthread_mutex_unlock(&colour_mutex);
		    return ;
	    }
        fprintf(fp, "COLOURS=%s::%s\n", arg1, arg2);
        fclose(fp);
        manager->udp->OWN_USER_ID = build_user_info(manager->OWN_MACHINE_ID, manager->OWN_USERNAME, get_colour(arg1), get_colour(arg2));
        manager->tcp->OWN_USER_ID = build_colour_string(manager->OWN_MACHINE_ID, manager->OWN_USERNAME, get_colour(arg1), get_colour(arg2));
        printf("%s(colour set)\n", manager->tcp->OWN_USER_ID);
        pthread_mutex_unlock(&colour_mutex);
    }
    else
        printf("Usage: chat42 --colour-set COLOUR::COLOUR\n");

}

void    colour_list_command(const char *arg1, const char *arg2) { 

    static const char *LIST_COLOURS = 
        BLACK "BLACK " RESET RED "RED " RESET GREEN "GREEN " RESET YELLOW "YELLOW " RESET
        BLUE "BLUE " RESET MAGENTA "MAGENTA " RESET CYAN "CYAN " RESET WHITE "WHITE " RESET
        BOLD_BLACK "BOLD_BLACK " RESET BOLD_RED "BOLD_RED \n" RESET BOLD_GREEN "BOLD_GREEN " RESET
        BOLD_YELLOW "BOLD_YELLOW " RESET BOLD_BLUE "BOLD_BLUE " RESET BOLD_MAGENTA "BOLD_MAGENTA " RESET
        BOLD_CYAN "BOLD_CYAN " RESET BOLD_WHITE "BOLD_WHITE\n";
    if (arg1 == NULL && arg2 == NULL)
        printf("%s\n", LIST_COLOURS);
    else
        printf("Usage: chat42 --colour-list\n");
}