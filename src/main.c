#include "../inc/chat42.h"
#define BUF_SIZE 1024

void    *tcp_thread_func(void* arg);
void    *udp_thread_func(void* arg);
char    *get_user_id(void);
void    handle_commands(char *strs);


int main(int argc, char **argv) {

    if (argc < 2) {
        return (printf(HELP_MSG), 1);
    }

    if (strncmp("--connect", argv[1], 10) != 0) {
        handle_commands(argv[1]);
    }
    else {

        char *user_id = get_user_id();
        if (!user_id)
            return (1);
        pthread_t udp_thread;
        pthread_t tcp_thread;

        if (pthread_create(&udp_thread, NULL, udp_thread_func, user_id) == -1)
            return (free(user_id), 1);
        if (pthread_create(&tcp_thread, NULL, tcp_thread_func, NULL) == -1)
            return (pthread_cancel(udp_thread), free(user_id), 1);

        char line[BUF_SIZE];
        while (fgets(line, BUF_SIZE, stdin)) {

            size_t line_len = strcspn(line, "\n");
            line[line_len] = 0;
            if (strncmp(line, "chat42 --disconnect", 20) == 0)
                break ;
            handle_commands(line);
        }
        pthread_cancel(udp_thread);
        pthread_cancel(tcp_thread);
        pthread_join(udp_thread, NULL);
        pthread_join(tcp_thread, NULL);
    }
    return (0);
}