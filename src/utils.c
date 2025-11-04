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


    user = getenv("USER");
    if (!user)
        user = getenv("LOGNAME");
    if (!user)
        user = "unknown";


    char *session = getenv("SESSION_MANAGER");
    char session_id[BUF_SIZE];
    session_id[0] = '\0';

    if (session) {

        char *p = strchr(session, '/');
        if (p) {
            p++;
            char *end = strchr(p, '.');
            if (!end)
                end = strchr(p, ':');
            if (!end)
                end = strchr(p, '/');
            if (end && end > p) {
                size_t token_len = (size_t)(end - p);
                if (token_len >= sizeof(session_id))
                    token_len = sizeof(session_id) - 1;
                memcpy(session_id, p, token_len);
                session_id[token_len] = '\0';
            } else {
   
                strncpy(session_id, p, sizeof(session_id) - 1);
                session_id[sizeof(session_id) - 1] = '\0';
            }
        }
    }

    if (session_id[0] == '\0') {
        if (gethostname(host, sizeof(host) - 1) != 0)
            return (NULL);
        host[sizeof(host) - 1] = '\0';
        strncpy(session_id, host, sizeof(session_id) - 1);
        session_id[sizeof(session_id) - 1] = '\0';
    }
    len = (mode ? 2 : 0) + strlen(session_id) + 1 + strlen(user) + 1;

    result = malloc(len);
    if (!result)
        return (NULL);

    if (mode)
		snprintf(result, len, "1;%s;%s", session_id, user);
    else
		snprintf(result, len, "%s;%s", session_id, user);

    return result;
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
    memcpy(udp.OWN_USER_MACHINE_ID, "0;", 2);
	for (int i = 0; i < TABLE_MAX_SIZE; i++) {

		if (users_table[i]) {
            sendto(udp.sockfd, udp.OWN_USER_MACHINE_ID, udp.OWN_USER_MACHINE_LEN, 0, 
                (struct sockaddr*)&users_table[i]->CLIENT_ADDR, sizeof(users_table[i]->CLIENT_ADDR));
			free(users_table[i]);
			users_table[i] = NULL;
		}
	}
	pthread_mutex_unlock(&hash_table_mutex);

	if (udp.sockfd)
		close(udp.sockfd);
	if (tcp.sockfd)
		close(tcp.sockfd);
	if (udp.OWN_USERNAME)
		free(udp.OWN_USERNAME);
	if (udp.OWN_USER_MACHINE_ID)
		free(udp.OWN_USER_MACHINE_ID);
	if (tcp.OWN_USERNAME)
		free(tcp.OWN_USERNAME);
	if (tcp.OWN_USER_MACHINE_ID)
		free(tcp.OWN_USER_MACHINE_ID);

	pthread_cancel(udp.send_thread);
    pthread_cancel(tcp.send_thread);
	pthread_cancel(tcp.receive_thread);
	pthread_join(udp.receive_thread, NULL);
    pthread_join(udp.send_thread, NULL);
    pthread_join(tcp.send_thread, NULL);

	printf("\nServer shutdown cleanly.\n");
	exit(0);
}

