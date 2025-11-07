#include "../inc/chat42.h"


char *get_machine_id(void)
{
	char host[BUF_SIZE];
	char session_id[BUF_SIZE];
	char *session = getenv("SESSION_MANAGER");
	char *result;

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

	result = strdup(session_id);
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


char *build_user_info(const char *machine_id, const char *user_id, const char *colour_a, const char *colour_b)
{
	char *result;
	size_t len;
	const char *colour_a_name = get_color_name(colour_a);
	const char *colour_b_name = get_color_name(colour_b);

	if (!machine_id || !user_id)
		return (NULL);

	len = 2 + strlen(machine_id) + 1 + strlen(user_id) + 1 +
		strlen(colour_a_name) + 1 + strlen(colour_b_name) + 2;

	result = malloc(len);
	if (!result)
		return (NULL);

	snprintf(result, len, "1;%s;%s;%s;%s;", machine_id, user_id, colour_a_name, colour_b_name);
	return (result);
}


char *build_colour_string(const char *machine_id, const char *username, const char *colour_a, const char *colour_b)
{
	char *result;
	size_t len;

	
	if (!machine_id || !username)
		return (NULL);


	len = strlen(colour_a) + strlen(machine_id) + strlen(RESET) +
	      2 + 
	      strlen(colour_b) + strlen(username) + strlen(RESET) +
	      1 + 
	      2; 

	result = malloc(len);
	if (!result)
		return (NULL);

	snprintf(result, len, "%s%s%s::%s%s%s: ", colour_a, machine_id, RESET, colour_b, username, RESET);
	return (result);
}

char *strjoin(const char *s1, const char *s2)
{
	size_t len1, len2;
	char *res;

	if (!s1 || !s2)
		return (NULL);

	len1 = strlen(s1);
	len2 = strlen(s2);

	res = malloc(len1 + len2 + 1);
	if (!res)
		return (NULL);

	memcpy(res, s1, len1);
	memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	return (res);
}

const char  *get_color(const char *name) {
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

const char *get_color_name(const char *code)
{
	if (!code) return "WHITE";

	if (strcmp(code, BLACK) == 0) return "BLACK";
	if (strcmp(code, RED) == 0) return "RED";
	if (strcmp(code, GREEN) == 0) return "GREEN";
	if (strcmp(code, YELLOW) == 0) return "YELLOW";
	if (strcmp(code, BLUE) == 0) return "BLUE";
	if (strcmp(code, MAGENTA) == 0) return "MAGENTA";
	if (strcmp(code, CYAN) == 0) return "CYAN";
	if (strcmp(code, WHITE) == 0) return "WHITE";

	if (strcmp(code, BOLD_BLACK) == 0) return "BOLD_BLACK";
	if (strcmp(code, BOLD_RED) == 0) return "BOLD_RED";
	if (strcmp(code, BOLD_GREEN) == 0) return "BOLD_GREEN";
	if (strcmp(code, BOLD_YELLOW) == 0) return "BOLD_YELLOW";
	if (strcmp(code, BOLD_BLUE) == 0) return "BOLD_BLUE";
	if (strcmp(code, BOLD_MAGENTA) == 0) return "BOLD_MAGENTA";
	if (strcmp(code, BOLD_CYAN) == 0) return "BOLD_CYAN";
	if (strcmp(code, BOLD_WHITE) == 0) return "BOLD_WHITE";

	return "WHITE";
}
