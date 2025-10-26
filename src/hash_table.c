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
            printf("[%d] = %s\n", i, users_table[i]->name_loc);
    }
}

ssize_t hashtable_hash(char *user_id) {

    if (!user_id)
        return (-1);
    size_t hash_value = 0;
    for (size_t i = 0; i < strlen(user_id); i++) {

        hash_value += user_id[i];
        hash_value = (hash_value * user_id[i]) % TABLE_MAX_SIZE;
    }
    return (hash_value);
}


int hashtable_insert(t_client *users_table[TABLE_MAX_SIZE], t_client *new_user) {

    if (new_user == NULL)
        return (1);
    int index = hashtable_hash(new_user->name_loc);
    if (users_table[index] != NULL) {
        return (1);
    }
    users_table[index] = new_user;
    return (0);
}

t_client *hashtable_search(t_client *users_table[TABLE_MAX_SIZE], char *user_id) {

    if (!user_id || strlen(user_id) < 5)
        return (NULL);
    char *trimmed_id = user_id + 4;
    int index = hashtable_hash(trimmed_id);
    if (users_table[index] != NULL && strncmp(users_table[index]->name_loc, trimmed_id, BUF_SIZE) == 0)
        return (users_table[index]);
    else
        return (NULL);
}

void    hashtable_delete(t_client *users_table[TABLE_MAX_SIZE], char *user_id) {

    if (!user_id || strlen(user_id) < 5)
        return ;
    char *trimmed_id = user_id + 4;
    int index = hashtable_hash(trimmed_id);
    if (users_table[index] != NULL && strncmp(users_table[index]->name_loc, trimmed_id, BUF_SIZE) == 0) {
        free(users_table[index]);
        users_table[index] = NULL;
    }
}

t_client *hashtable_add(char *user_id, struct sockaddr_in *addr)
{
    if (!addr || !user_id || strlen(user_id) < 5)
        return (NULL);
    t_client *new_user = malloc(sizeof(t_client));
    if (!new_user)
        return (NULL);
    char *trimmed_id = user_id + 4;
    memcpy(&new_user->addr, addr, sizeof(struct sockaddr_in));
    strncpy(new_user->name_loc, trimmed_id, BUF_SIZE - 1);
    new_user->name_loc[BUF_SIZE - 1] = '\0';
    return (new_user);
}