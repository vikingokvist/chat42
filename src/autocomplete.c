#include "../inc/chat42.h"


char **init_autocomplete(const char *text, int start, int end) {

	(void)end;

	if (start == 0)
		return (NULL);

	char *line = strdup(rl_line_buffer);
	if (!line)
		return (NULL);

	char *first = strtok(line, " \t");
	if (!first || strcmp(first, "chat42") != 0) {
		free(line);
		return (NULL);
	}


	char *second = strtok(NULL, " \t");
	char **matches = NULL;
	if (!second || start >= (int)(second - line)) {
		matches = rl_completion_matches(text, find_autocomplete);
	}

	free(line);
	return (matches);
}

char	*find_autocomplete(const char *text, int state) {

	static int				cmd_index;
	static t_client_names	*cur_user = NULL;
	t_client_names			*head;
	char					*match = NULL;
	static const char		*server_cmds[] = {
		"--help", "--disconnect", "--colour-list", "--colour-set", "--version", NULL
	};
	
	pthread_mutex_lock(&autocomplete_mutex);
	head = manager->client_table_names;

	if (state == 0) {
		cmd_index = 0;
		cur_user = head;
	}

	while (server_cmds[cmd_index]) {

		if (!strncmp(server_cmds[cmd_index], text, strlen(text))) {

			match = strdup(server_cmds[cmd_index]);
			cmd_index++;
			pthread_mutex_unlock(&autocomplete_mutex);
			return (match);
		}
		cmd_index++;
	}

	while (cur_user) {

		if (!strncmp(cur_user->name, text, strlen(text))) {

			match = strdup(cur_user->name);
			cur_user = cur_user->next;
			pthread_mutex_unlock(&autocomplete_mutex);
			return (match);
		}
		cur_user = cur_user->next;
	}

	pthread_mutex_unlock(&autocomplete_mutex);
	return (NULL);
}



void	remove_user_autocomplete(const char *username) {

	t_client_names *cur, *prev = NULL;

	pthread_mutex_lock(&autocomplete_mutex);
	if (!manager || !username) {
		pthread_mutex_unlock(&autocomplete_mutex);
		return;
	}

	cur = manager->client_table_names;
	while (cur) {
		if (strcmp(cur->name, username) == 0) {
			if (prev)
				prev->next = cur->next;
			else
				manager->client_table_names = cur->next;
			free(cur->name);
			free(cur);
			pthread_mutex_unlock(&autocomplete_mutex);
			return;
		}
		prev = cur;
		cur = cur->next;
	}
	pthread_mutex_unlock(&autocomplete_mutex);
}

void    add_user_autocomplete(const char *username) {

	t_client_names *new_node;
	t_client_names *cur;

	pthread_mutex_lock(&autocomplete_mutex);
	if (!manager || !username || !*username) {
		pthread_mutex_unlock(&autocomplete_mutex);
		return;
	}

	cur = manager->client_table_names;
	while (cur) {
		if (strcmp(cur->name, username) == 0) {
			pthread_mutex_unlock(&autocomplete_mutex);
			return;
		}
		cur = cur->next;
	}
	new_node = malloc(sizeof(t_client_names));
	if (!new_node) {
		pthread_mutex_unlock(&autocomplete_mutex);
		return;
	}
	new_node->name = strdup(username);
	if (!new_node->name) {
		free(new_node);
		pthread_mutex_unlock(&autocomplete_mutex);
		return;
	}
	new_node->next = NULL;
	if (!manager->client_table_names)
		manager->client_table_names = new_node;
	else {
		cur = manager->client_table_names;
		while (cur->next)
			cur = cur->next;
		cur->next = new_node;
	}
	pthread_mutex_unlock(&autocomplete_mutex);
}


void	free_autocomplete(t_client_names **head) {

	t_client_names *cur;
	t_client_names *next;

	pthread_mutex_lock(&autocomplete_mutex);
	if (!head || !*head) {
		pthread_mutex_unlock(&autocomplete_mutex);
		return;
	}

	cur = *head;
	while (cur) {
		next = cur->next;
		if (cur->name)
			free(cur->name);
		free(cur);
		cur = next;
	}
	*head = NULL;
	pthread_mutex_unlock(&autocomplete_mutex);
}