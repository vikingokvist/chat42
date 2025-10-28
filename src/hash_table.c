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

ssize_t hashtable_hash(char *new_username) {

    if (!new_username)
        return (-1);
    size_t hash_value = 0;
    for (size_t i = 0; i < strlen(new_username); i++) {

        hash_value += new_username[i];
        hash_value = (hash_value * new_username[i]) % TABLE_MAX_SIZE;
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

t_client *hashtable_search(t_client *users_table[TABLE_MAX_SIZE], char *new_username) {

    if (!new_username)
        return (NULL);
    int index = hashtable_hash(new_username);
    if (users_table[index] != NULL && strncmp(users_table[index]->USERNAME, new_username, BUF_SIZE) == 0) {

        return (users_table[index]);
    }
    return (NULL);
}

void    hashtable_delete(t_client *users_table[TABLE_MAX_SIZE], char *new_username) {

    if (!new_username)
        return ;
    int index = hashtable_hash(new_username);
    if (users_table[index] != NULL && strncmp(users_table[index]->USERNAME, new_username, BUF_SIZE) == 0) {

        free(users_table[index]);
        users_table[index] = NULL;
    }
}

t_client *hashtable_add(struct sockaddr_in *new_cliaddr, char *new_username, char *new_machine_id)
{
    if (!new_cliaddr || !new_username || !new_machine_id)
        return (NULL);
    t_client *new_user = malloc(sizeof(t_client));
    if (!new_user)
        return (NULL);

    struct sockaddr_in NEW_CLIENT_ADDR;

    memset(&NEW_CLIENT_ADDR, 0, sizeof(NEW_CLIENT_ADDR));
    NEW_CLIENT_ADDR.sin_family = AF_INET;
    NEW_CLIENT_ADDR.sin_addr.s_addr = new_cliaddr->sin_addr.s_addr;
    NEW_CLIENT_ADDR.sin_port = htons(TCP_PORT);
    memcpy(&new_user->CLIENT_ADDR, &NEW_CLIENT_ADDR, sizeof(struct sockaddr_in));

    strncpy(new_user->USERNAME, new_username, 64 - 1);
    new_user->USERNAME[63] = '\0';
    strncpy(new_user->MACHINE_ID, new_machine_id, 64 - 1);
    new_user->MACHINE_ID[63] = '\0';

    return (new_user);
}