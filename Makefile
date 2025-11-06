NAME = chat42

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lpthread

RM = rm -rf

SRCS = src/main.c src/tcp_thread.c src/udp_thread.c src/utils.c src/hash_table.c src/commands.c

OBJDIR = objs
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)


SEND_IP := $(shell ip -4 addr show enp4s0f0 | grep -oP '(?<=brd\s)\d+(\.\d+){3}')

CFLAGS += -DSEND_IP=\"$(SEND_IP)\"

all: $(NAME)
	@echo "Using SEND_IP: $(SEND_IP)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

#echo -n "hello" | nc -u -b 10.11.255.255 6000
#nc -u -l 6000