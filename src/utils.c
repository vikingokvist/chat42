#include "../inc/chat42.h"



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


void cleanup_and_exit(int signo)
{
    (void)signo;
	pthread_mutex_lock(&hash_table_mutex);
    memcpy(udp_struct.PLACE_USER_ID, "0;", 4);
	for (int i = 0; i < TABLE_MAX_SIZE; i++) {

		if (users_table[i]) {
            sendto(udp_struct.sockfd, udp_struct.PLACE_USER_ID, strlen(udp_struct.PLACE_USER_ID), 0, 
                (struct sockaddr*)&users_table[i]->addr, sizeof(users_table[i]->addr));
			free(users_table[i]);
			users_table[i] = NULL;
		}
	}
	pthread_mutex_unlock(&hash_table_mutex);

	if (udp_struct.sockfd)
		close(udp_struct.sockfd);
	if (tcp_struct.sockfd)
		close(tcp_struct.sockfd);
	if (udp_struct.USER_ID)
		free(udp_struct.USER_ID);
	if (udp_struct.PLACE_USER_ID)
		free(udp_struct.PLACE_USER_ID);
	if (tcp_struct.USER_ID)
		free(tcp_struct.USER_ID);
	if (tcp_struct.PLACE_USER_ID)
		free(tcp_struct.PLACE_USER_ID);

	pthread_cancel(udp_struct.udp_thread);
    pthread_cancel(tcp_struct.tcp_thread);
    pthread_join(udp_struct.udp_thread, NULL);
    pthread_join(tcp_struct.tcp_thread, NULL);

	printf("\nServer shutdown cleanly.\n");
	exit(0);
}
