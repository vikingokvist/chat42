#include "../inc/chat42.h"


void cleanup_and_exit() {


	pthread_mutex_unlock(&hash_table_mutex);
	pthread_mutex_lock(&hash_table_mutex);

	t_udp *UDP = manager->udp;
	t_tcp *TCP = manager->tcp;
	size_t user_len = strlen(UDP->OWN_USER_ID);
    memcpy(UDP->OWN_USER_ID, "0;", 2);

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
	free(manager);

	printf("\nServer shutdown cleanly.\n");
	exit(0);
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