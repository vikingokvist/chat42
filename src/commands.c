#include "../inc/chat42.h"


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


void    handle_commands(const char *input, t_tcp *tcp) {

	char chat42[32], cmd[128], msg[BUF_SIZE];
    char arg1[64], arg2[64];
	int offset = 0;
    command_func_t func;

	if (sscanf(input, "%31s%n", chat42, &offset) != 1)
        return ; 
	if (strcmp(chat42, "chat42") != 0)
        return ; 
	input += offset;
	if (sscanf(input, "%127s%n", cmd, &offset) != 1)
        return ;
	input += offset;
	while (*input == ' ') 
		input++;
    
    if ((func = is_server_command(cmd)) != NULL) {
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
	t_client *client = hashtable_search(users_table, cmd); 
	if (!client) { 
		printf("User '%s' not found\n", cmd); fflush(stdout);
		return ((void)pthread_mutex_unlock(&hash_table_mutex)); 
	} 
	fflush(stdout);
    pthread_mutex_lock(&colour_mutex);
	send_tcp_message(client, msg, tcp);
    pthread_mutex_unlock(&colour_mutex);
	pthread_mutex_unlock(&hash_table_mutex);
}

void    help_command(const char *arg1, const char *arg2) { 
    if (arg1 == NULL && arg2 == NULL)
        printf("%s", HELP_MSG);
    else
        printf("Usage: chat42 --help\n");
}


void    disconnect_command(const char *arg1, const char *arg2) {
    if (arg1 == NULL && arg2 == NULL)
        cleanup_and_exit(); 
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
        manager->udp->OWN_USER_ID = build_user_info(manager->OWN_MACHINE_ID, manager->OWN_USERNAME, get_color(arg1), get_color(arg2));
        manager->tcp->OWN_USER_ID = build_colour_string(manager->OWN_MACHINE_ID, manager->OWN_USERNAME, get_color(arg1), get_color(arg2));
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
        BOLD_CYAN "BOLD_CYAN " RESET BOLD_WHITE "BOLD_WHITE\n" RESET BG_BLACK "BG_BLACK" RESET " "
        BG_RED "BG_RED" RESET " " BG_GREEN "BG_GREEN" RESET " " BG_YELLOW "BG_YELLOW" RESET " "
        BG_BLUE "BG_BLUE" RESET " " BG_MAGENTA "BG_MAGENTA" RESET " " BG_CYAN "BG_CYAN" RESET " "
        BG_WHITE "BG_WHITE" RESET " \n";
    if (arg1 == NULL && arg2 == NULL)
        printf("%s\n", LIST_COLOURS);
    else
        printf("Usage: chat42 --colour-list\n");
}