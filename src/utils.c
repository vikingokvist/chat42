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


char *get_user_id(void) {
	
	char host[BUF_SIZE];
	char *user;
	char *result;
	size_t len;

	if (gethostname(host, sizeof(host) - 1) != 0)
		return NULL;
	host[sizeof(host) - 1] = '\0';

	user = getenv("USER");
	if (!user)
		user = getenv("LOGNAME");
	if (!user)
		user = "unknown";

	len = 4 + strlen(host) + 2 + strlen(user) + 1;

	result = (char *)malloc(len);
	if (!result)
		return NULL;

	memcpy(result, "onn;", 4);
	snprintf(result + 4, len - 4, "%s::%s", host, user);

	return (result);
}

void    handle_commands(char *strs) {

    if (strncmp("--help", strs, 7) == 0)
        printf(HELP_MSG);
    else if (strncmp("--version", strs, 10) == 0)
        printf(VERSION_MSG);
    else if (strncmp("--colour-list", strs, 14) == 0)
        printf("%s", LIST_COLOURS);
}
