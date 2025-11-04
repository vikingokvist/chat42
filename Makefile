NAME = chat42

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lpthread

RM = rm -rf

SRCS = src/main.c src/tcp_thread.c src/udp_thread.c src/utils.c src/hash_table.c

OBJDIR = objs
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)


OWN_IP := $(shell ip -4 addr show enp4s0f0 | awk '/inet / {print $$2}' | cut -d/ -f1)
SEND_IP := $(shell ip -4 addr show enp4s0f0 | awk '/inet / {for (i=1;i<=NF;i++) if ($$i=="brd") print $$(i+1)}')


CFLAGS += -DOWN_IP=\"$(OWN_IP)\" -DSEND_IP=\"$(SEND_IP)\"

all: $(NAME)
	@echo "Using OWN_IP: $(OWN_IP)"
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