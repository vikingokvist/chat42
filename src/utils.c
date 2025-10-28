#include "../inc/chat42.h"

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


const char *get_color_code(const char *name) {
	if (!name) return WHITE;
	if (strncmp(name, "BLACK", 6) == 0) return BLACK;
	if (strncmp(name, "RED", 4) == 0) return RED;
	if (strncmp(name, "GREEN", 6) == 0) return GREEN;
	if (strncmp(name, "YELLOW", 7) == 0) return YELLOW;
	if (strncmp(name, "BLUE", 5) == 0) return BLUE;
	if (strncmp(name, "MAGENTA", 8) == 0) return MAGENTA;
	if (strncmp(name, "CYAN", 5) == 0) return CYAN;
	if (strncmp(name, "WHITE", 6) == 0) return WHITE;

	if (strncmp(name, "BOLD_BLACK", 10) == 0) return BOLD_BLACK;
	if (strncmp(name, "BOLD_RED", 9) == 0) return BOLD_RED;
	if (strncmp(name, "BOLD_GREEN", 11) == 0) return BOLD_GREEN;
	if (strncmp(name, "BOLD_YELLOW", 12) == 0) return BOLD_YELLOW;
	if (strncmp(name, "BOLD_BLUE", 10) == 0) return BOLD_BLUE;
	if (strncmp(name, "BOLD_MAGENTA", 13) == 0) return BOLD_MAGENTA;
	if (strncmp(name, "BOLD_CYAN", 10) == 0) return BOLD_CYAN;
	if (strncmp(name, "BOLD_WHITE", 11) == 0) return BOLD_WHITE;

	return WHITE;
}

char *get_user_info(int mode)
{
	char host[BUF_SIZE];
	char *user;
	char *result;
	size_t len;

	if (gethostname(host, sizeof(host) - 1) != 0)
		return (NULL);
	host[sizeof(host) - 1] = '\0';

	user = getenv("USER");
	if (!user)
		user = getenv("LOGNAME");
	if (!user)
		user = "unknown";

	len = (mode ? 2 + 1 : 0) + strlen(host) + 2 + strlen(user) + 1;

	result = (char *)malloc(len);
	if (!result)
		return (NULL);

	if (mode)
		snprintf(result, len, "1;%s::%s", host, user);
	else
		snprintf(result, len, "%s::%s", host, user);

	return (result);
}


char *get_user_name(void)
{
	char *user;
	char *result;
	size_t len;
	char *space_user;

	user = getenv("USER");
	if (!user)
		user = getenv("LOGNAME");
	if (!user)
		user = "unknown";

	len = strlen(user) + 1;
	result = (char *)malloc(len);
	if (!result)
		return (NULL);

	space_user = result;
	for (size_t i = 0; user[i]; i++) {
		*space_user++ = user[i];
	}
	*space_user = '\0';
	return (result);
}


void cleanup_and_exit()
{
	usleep(100000);
	pthread_mutex_unlock(&hash_table_mutex);
	pthread_mutex_lock(&hash_table_mutex);
    memcpy(udp_struct.OWN_USER_MACHINE_ID, "0;", 2);
	for (int i = 0; i < TABLE_MAX_SIZE; i++) {

		if (users_table[i]) {
            sendto(udp_struct.sockfd, udp_struct.OWN_USER_MACHINE_ID, udp_struct.OWN_USER_MACHINE_LEN, 0, 
                (struct sockaddr*)&users_table[i]->CLIENT_ADDR, sizeof(users_table[i]->CLIENT_ADDR));
			free(users_table[i]);
			users_table[i] = NULL;
		}
	}
	pthread_mutex_unlock(&hash_table_mutex);

	if (udp_struct.sockfd)
		close(udp_struct.sockfd);
	if (tcp_struct.sockfd)
		close(tcp_struct.sockfd);
	if (udp_struct.OWN_USERNAME)
		free(udp_struct.OWN_USERNAME);
	if (udp_struct.OWN_USER_MACHINE_ID)
		free(udp_struct.OWN_USER_MACHINE_ID);
	if (tcp_struct.OWN_USERNAME)
		free(tcp_struct.OWN_USERNAME);
	if (tcp_struct.OWN_USER_MACHINE_ID)
		free(tcp_struct.OWN_USER_MACHINE_ID);

	pthread_cancel(udp_struct.udp_thread);
    pthread_cancel(tcp_struct.tcp_thread);
    pthread_join(udp_struct.udp_thread, NULL);
    pthread_join(tcp_struct.tcp_thread, NULL);

	printf("\nServer shutdown cleanly.\n");
	exit(0);
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
		memcpy(udp_struct.OWN_USER_MACHINE_ID, "0;", 2);
		cleanup_and_exit();
		return ;
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