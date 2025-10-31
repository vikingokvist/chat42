NAME = chat42

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lpthread

RM = rm -rf

SRCS = src/main.c src/tcp_thread.c src/udp_thread.c src/utils.c src/hash_table.c
OBJDIR = objs

OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)



IP := $(shell ip a show enp4s0f0 | grep 'inet ' | awk '{print $$2}' | cut -d/ -f1)


CFLAGS += -DSEND_IP=\"$(IP)\"


all: $(NAME)
	@echo "Using IP: $(IP)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)
