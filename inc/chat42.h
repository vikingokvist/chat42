#ifndef CHAT42_H
# define CHAT42_H

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>

extern char            *own_user_id;

#define HELP_MSG  "\
usage: chat42 [--version] [--help] [--login] [<username> + <message>]\n\
              [--colour-list] [--colour-set (+ <colour>::<colour>)]\n\
              [--block + <username>]\n\n\
These are common chat42 commands used in various situations:\n\n\
  --help                         Print help page.\n\
  --version                      State current version.\n\
  --login                        Start looking for connected clients.\n\
  --block <username>             Block communications from client.\n\
  --colour-list                  List all ANSI colours.\n\
  --colour-set <colour::colour>  Set profile colours.\n\
  <username> + <message>         Send message to username.\n"




#define VERSION_MSG "chat42 pre-pre-alpha 0.0.1\n"



#define RESET       "\x1b[0m"
#define BLACK       "\x1b[0;30m"
#define RED         "\x1b[0;31m"
#define GREEN       "\x1b[0;32m"
#define YELLOW      "\x1b[0;33m"
#define BLUE        "\x1b[0;34m"
#define MAGENTA     "\x1b[0;35m"
#define CYAN        "\x1b[0;36m"
#define WHITE       "\x1b[0;37m"
#define BOLD_BLACK   "\x1b[1;30m"
#define BOLD_RED     "\x1b[1;31m"
#define BOLD_GREEN   "\x1b[1;32m"
#define BOLD_YELLOW  "\x1b[1;33m"
#define BOLD_BLUE    "\x1b[1;34m"
#define BOLD_MAGENTA "\x1b[1;35m"
#define BOLD_CYAN    "\x1b[1;36m"
#define BOLD_WHITE   "\x1b[1;37m"
#define BG_BLACK     "\x1b[40m"
#define BG_RED       "\x1b[41m"
#define BG_GREEN     "\x1b[42m"
#define BG_YELLOW    "\x1b[43m"
#define BG_BLUE      "\x1b[44m"
#define BG_MAGENTA   "\x1b[45m"
#define BG_CYAN      "\x1b[46m"
#define BG_WHITE     "\x1b[47m"

char            *get_user_id(void);
const char      *get_color_code(const char *name);



#define BUF_SIZE 1024
#define TABLE_MAX_SIZE 6

typedef struct s_client {

    struct sockaddr_in  addr;
    char                name_loc[BUF_SIZE];
}   t_client;

extern t_client *users_table[TABLE_MAX_SIZE];

void            hashtable_init(t_client *users_table[TABLE_MAX_SIZE]);
void            print_client(t_client *users_table[TABLE_MAX_SIZE]);
ssize_t         hashtable_hash(char *user_id);
int             hashtable_insert(t_client *users_table[TABLE_MAX_SIZE], t_client *new_user);
t_client        *hashtable_search(t_client *users_table[TABLE_MAX_SIZE], char *user_id);
t_client        *hashtable_add(char *user_id, struct sockaddr_in *addr);
void            hashtable_delete(t_client *users_table[TABLE_MAX_SIZE], char *user_id);



#define UDP_PORT 50001
#define TCP_PORT 50002

extern pthread_mutex_t hash_table_mutex;
extern int             udp_sockfd;
void            *tcp_thread_func(void* arg);
void            *udp_thread_func(void* arg);
void send_tcp_message(t_client *client, const char *msg);


void            handle_commands(const char *input);
void            cleanup_and_exit(int signo);

#endif
