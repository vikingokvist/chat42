#include "../inc/chat42.h"

void    hashtable_init(t_client **users_table) {

    for (int i = 0; i < TABLE_MAX_SIZE; i++) {
        users_table[i] = NULL;
    }
}

void    hashtable_clear(t_client **users_table)
{
	for (int i = 0; i < TABLE_MAX_SIZE; i++) {
		t_client *cur = users_table[i];
		t_client *next;

		while (cur) {
			next = cur->next;
			free(cur);
			cur = next;
		}
		users_table[i] = NULL;
	}
}


void print_client(t_client **users_table) {

    for (int i = 0; i < TABLE_MAX_SIZE; i++) {
        t_client *cur = users_table[i];
        if (cur == NULL)
            printf("[%d] = NULL\n", i);
        while (cur) {
            if (cur == NULL)
                printf("[%d] = NULL\n", i);
            else
                printf("[%d] = 1;%s;%s\n", i, users_table[i]->MACHINE_ID, users_table[i]->USERNAME);
            cur = cur->next;
        }
    }
}

ssize_t hashtable_hash(char *username) {

    if (!username)
        return (-1);
    size_t hash_value = 5381;
    size_t len = strlen(username);
    for (size_t i = 0; username[i]; i++) {

        hash_value *= 33;
        hash_value = (hash_value + username[i]) % TABLE_MAX_SIZE;
    }
    return (hash_value);
}


int hashtable_insert(t_client **users_table, t_client *new_user) {

    if (!new_user || !new_user->USERNAME[0])
        return (1);
    int index = hashtable_hash(new_user->USERNAME);
    t_client *cur = users_table[index];
    new_user->next = NULL;
    if (cur == NULL) {
        users_table[index] = new_user;
        return (0);
    }
    while (cur->next)
        cur = cur->next;
    cur->next = new_user;
    return (0);
}

t_client *hashtable_search(t_client **users_table, char *username) {

    if (!username)
        return (NULL);
    int index = hashtable_hash(username);
    t_client *cur = users_table[index];
    if (users_table[index] == NULL)
        return (NULL);

    while (cur) {
        if (!strcmp(username, cur->USERNAME))
            return (cur);
        cur = cur->next;
    }
    printf("new username[%d] connected = %s\n", index , username);
    return (NULL);
}

void    hashtable_delete(t_client **users_table,  char *username) {

    if (!username)
        return ;
    int index = hashtable_hash(username);
    t_client *cur = users_table[index];
    t_client *prev = NULL;

    while (cur) {

        if (strcmp(cur->USERNAME, username) == 0) {
            if (prev == NULL)
                users_table[index] = cur->next;
            else
                prev->next = cur->next;
            free(cur);
        }
        prev = cur;
        cur = cur->next;
    }

}

t_client *hashtable_add(struct sockaddr_in *new_cliaddr, char *username, char *machine_id)
{
    if (!new_cliaddr || !username || !machine_id)
        return (NULL);
    t_client *new_user = malloc(sizeof(t_client));
    if (!new_user)
        return (NULL);
    new_user->next = NULL;


    new_user->CLIENT_ADDR.sin_family = AF_INET;
    new_user->CLIENT_ADDR.sin_addr.s_addr = new_cliaddr->sin_addr.s_addr;
    new_user->CLIENT_ADDR.sin_port = htons(TCP_PORT);

    strncpy(new_user->USERNAME, username, 63);
    new_user->USERNAME[63] = '\0';
    strncpy(new_user->MACHINE_ID, machine_id, 63);
    new_user->MACHINE_ID[63] = '\0';
    


    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &new_user->CLIENT_ADDR.sin_addr, ip, sizeof(ip));
    printf("Saved client %s (%s:%d)\n", new_user->USERNAME, ip, ntohs(new_user->CLIENT_ADDR.sin_port));


    return (new_user);
}