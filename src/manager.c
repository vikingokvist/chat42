#include "../inc/chat42.h"

int	init_manager(void) {

	manager->users_table = calloc(TABLE_MAX_SIZE, sizeof(t_client *));
	if (!manager->users_table)
		return (1);
    hashtable_init(manager->users_table);
	manager->udp = calloc(1, sizeof(t_udp));
	if (!manager->udp)
		return (free(manager->users_table), 1);
	manager->tcp = calloc(1, sizeof(t_tcp));
	if (!manager->tcp)
		return (free(manager->udp), free(manager->users_table), 1);
	get_usr_file_colours();
	manager->OWN_USERNAME = get_user_name();
	manager->OWN_MACHINE_ID = get_machine_id();
	return (0);
}

void cleanup_and_exit() {


	pthread_mutex_unlock(&hash_table_mutex);
	pthread_mutex_lock(&hash_table_mutex);

	t_udp *UDP = manager->udp;
	t_tcp *TCP = manager->tcp;
    memcpy(UDP->OWN_USER_ID, "0;", 2);
	size_t user_len = strlen(UDP->OWN_USER_ID);

	for (int i = 0; i < TABLE_MAX_SIZE; i++) {
		t_client *cur = UDP->users_table[i];
		while (cur) {
			
            sendto(UDP->sockfd, UDP->OWN_USER_ID, user_len, 0, 
                (struct sockaddr*)&cur->CLIENT_ADDR, sizeof(cur->CLIENT_ADDR));
			cur = cur->next;
		}
	}
	hashtable_clear(UDP->users_table);
	pthread_mutex_unlock(&hash_table_mutex);

	pthread_cancel(UDP->receive_thread);
    pthread_cancel(UDP->send_thread);
    pthread_cancel(TCP->send_thread);
	pthread_join(UDP->receive_thread, NULL);
    pthread_join(UDP->send_thread, NULL);
    pthread_join(TCP->send_thread, NULL);
	
	if (UDP->sockfd)
		close(UDP->sockfd);
	if (TCP->sockfd)
		close(TCP->sockfd);
	free(UDP->OWN_USER_ID);
	free(TCP->OWN_USER_ID);
	free(manager->OWN_MACHINE_ID);
	free(manager->OWN_USERNAME);
	free(manager->tcp);
	free(manager->udp);
	pthread_mutex_lock(&autocomplete_list);
	free_client_names(&manager->client_table_names);
	pthread_mutex_unlock(&autocomplete_list);
	free(manager);
	printf("\nServer shutdown cleanly.\n");
	exit(0);
}

void	free_client_names(t_client_names **head)
{
	t_client_names *cur;
	t_client_names *next;

	if (!head || !*head)
		return;

	cur = *head;
	while (cur) {
		next = cur->next;
		if (cur->name)
			free(cur->name);
		free(cur);
		cur = next;
	}
	*head = NULL;
}


void	set_default_colours() {

	FILE *fp = fopen(CONFIG_PATH, "w");
	if (!fp)
		return ;
	fprintf(fp, "COLOURS=BOLD_WHITE::BOLD_WHITE\n");
	fclose(fp);
	manager->colour_a = BOLD_WHITE;
	manager->colour_b = BOLD_WHITE;	
}

void	get_usr_file_colours() {

	FILE *fp = fopen(CONFIG_PATH, "r");
	if (!fp) {
		set_default_colours();
		return ;
	}
	char line[BUF_SIZE];
	char colours[2][64];
	char *p;
	if (!fgets(line, sizeof(line), fp)) {
		fclose(fp);
		set_default_colours();
	}
	fclose(fp);
	if (strncmp(line, "COLOURS=", 8) != 0) {
		set_default_colours();
		return;
	}
	p = line + 8;
	if (sscanf(p, "%63[^:]::%63s", colours[0], colours[1]) != 2) {
		set_default_colours();
		return;
	}
	manager->colour_a = get_colour(colours[0]);
	manager->colour_b = get_colour(colours[1]);
	return ;
}


char **chat42_completion(const char *text, int start, int end) {

	(void)end;
	if (start == 0)
		return rl_completion_matches(text, user_generator);
	return (NULL);
}

char	*user_generator(const char *text, int state) {

	static t_client_names *cur = NULL;
	t_client_names *head;

	pthread_mutex_lock(&autocomplete_list);
	head = manager->client_table_names;

	if (!state) {
		cur = head;
	}

	while (cur) {
		if (strncmp(cur->name, text, strlen(text)) == 0) {
			char *match = cur->name;
			cur = cur->next;
			pthread_mutex_unlock(&autocomplete_list);
			return match;
		}
		cur = cur->next;
	}

	pthread_mutex_unlock(&autocomplete_list);
	return (NULL);
}

void	remove_name_autocomplete(t_manager *manager, const char *username)
{
	t_client_names *cur, *prev = NULL;

	if (!manager || !username)
		return;

	cur = manager->client_table_names;
	while (cur) {
		if (strcmp(cur->name, username) == 0) {
			if (prev)
				prev->next = cur->next;
			else
				manager->client_table_names = cur->next;
			free(cur->name);
			free(cur);
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

void    insert_name_autocomplete(t_manager *manager, const char *username)
{
	t_client_names *new_node;
	t_client_names *cur;

	if (!manager || !username || !*username)
		return;

	cur = manager->client_table_names;
	while (cur) {
		if (strcmp(cur->name, username) == 0)
			return;
		cur = cur->next;
	}
	new_node = malloc(sizeof(t_client_names));
	if (!new_node)
		return;
	new_node->name = strdup(username);
	if (!new_node->name) {
		free(new_node);
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
}


