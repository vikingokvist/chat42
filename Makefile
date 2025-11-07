NAME = chat42

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lpthread

RM = rm -rf

SRCS = src/main.c \
	src/tcp_thread.c \
	src/udp_thread.c \
	src/utils.c \
	src/hash_table.c \
	src/exit.c  \
	src/commands.c

OBJDIR = objs
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)



SEND_IP = $(shell ip -4 addr show enp4s0f0 | grep -oP '(?<=brd\s)\d+(\.\d+){3}')
TEST_IP = $(shell ip -4 addr show wlp13s0 | grep -oP '(?<=brd\s)\d+(\.\d+){3}')



all: CFLAGS += -DSEND_IP=\"$(SEND_IP)\"
all: $(NAME)
	@echo "Using SEND_IP: $(SEND_IP)"


test: CFLAGS += -DSEND_IP=\"$(TEST_IP)\"
test: $(NAME)
	@echo "Using TEST_IP: $(TEST_IP)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "Compiling files..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "DONE"

clean:
	@echo "Removing executable..."
	@$(RM) $(OBJDIR)
	@echo "DONE"

fclean:
	@echo "Removing object files..."
	@echo "Removing executable..."
	@$(RM) $(OBJDIR) $(NAME)
	@echo "DONE"

re: fclean clean all

-include $(DEPS)
