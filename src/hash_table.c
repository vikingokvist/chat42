#include "../inc/chat42.h"

void    hashtable_init(t_client *users_table[TABLE_MAX_SIZE]) {

    for (int i = 0; i < TABLE_MAX_SIZE; i++) {
        users_table[i] = NULL;
    }
}

void print_client(t_client *users_table[TABLE_MAX_SIZE]) {

    for (int i = 0; i < TABLE_MAX_SIZE; i++) {

        if (users_table[i] == NULL)
            printf("[%d] = NULL\n", i);
        else
            printf("[%d] = %s::%s\n", i, users_table[i]->MACHINE_ID, users_table[i]->USERNAME);
    }
}

ssize_t hashtable_hash(char *username) {

    if (!username)
        return (-1);
    size_t hash_value = 0;
    for (size_t i = 0; i < strlen(username); i++) {

        hash_value += username[i];
        hash_value = (hash_value * username[i]) % TABLE_MAX_SIZE;
    }
    return (hash_value);
}


int hashtable_insert(t_client *users_table[TABLE_MAX_SIZE], t_client *new_user) {

    int index = hashtable_hash(new_user->USERNAME);
    if (users_table[index] != NULL) {

        return (1);
    }
    users_table[index] = new_user;
    return (0);
}

t_client *hashtable_search(t_client *users_table[TABLE_MAX_SIZE], char *username) {

    if (!username)
        return (NULL);
    int index = hashtable_hash(username);
    if (users_table[index] != NULL && strncmp(users_table[index]->USERNAME, username, BUF_SIZE) == 0) {

        return (users_table[index]);
    }
    return (NULL);
}

void    hashtable_delete(t_client *users_table[TABLE_MAX_SIZE], char *username) {

    if (!username)
        return ;
    int index = hashtable_hash(username);
    if (users_table[index] != NULL && strncmp(users_table[index]->USERNAME, username, BUF_SIZE) == 0) {

        free(users_table[index]);
        users_table[index] = NULL;
    }
}

t_client *hashtable_add(struct sockaddr_in *new_cliaddr, char *username, char *machine_id)
{
    if (!new_cliaddr || !username || !machine_id)
        return (NULL);
    t_client *new_user = malloc(sizeof(t_client));
    if (!new_user)
        return (NULL);

    memset(&new_user->CLIENT_ADDR, 0, sizeof(new_user->CLIENT_ADDR));
    new_user->CLIENT_ADDR.sin_family = AF_INET;
    new_user->CLIENT_ADDR.sin_addr.s_addr = new_cliaddr->sin_addr.s_addr;
    new_user->CLIENT_ADDR.sin_port = htons(TCP_PORT);

    strncpy(new_user->USERNAME, username, 64 - 1);
    new_user->USERNAME[63] = '\0';
    strncpy(new_user->MACHINE_ID, machine_id, 64 - 1);
    new_user->MACHINE_ID[63] = '\0';

    return (new_user);
}