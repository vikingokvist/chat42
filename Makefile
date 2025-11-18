NAME = chat42

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lpthread -lreadline -lncurses

RM = rm -rf

WHITE = \033[1;37m
GREEN = \033[1;32m
RED  =  \033[1;31m
ORANGE  = \033[1;38;5;208m
RESET = \033[0m


SRCS = src/main.c \
	src/tcp_thread.c \
	src/udp_thread.c \
	src/utils.c \
	src/hash_table.c \
	src/manager.c \
	src/commands.c \
	src/autocomplete.c

OBJDIR = objs
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)



SEND_IP = $(shell ip -4 addr show enp4s0f0 | grep -oP '(?<=brd\s)\d+(\.\d+){3}')
TEST_IP = $(shell ip -4 addr show wlp13s0 | grep -oP '(?<=brd\s)\d+(\.\d+){3}')



all: CFLAGS += -DSEND_IP=\"$(SEND_IP)\"
all: $(NAME)
	@echo "Using IP: $(SEND_IP)"
	@echo "$(WHITE)Execute with ./chat42$(RESET)"

local: CFLAGS += -DSEND_IP=\"$(TEST_IP)\"
local: $(NAME)
	@echo "Using IP: $(TEST_IP)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(GREEN)Files Compiled$(RESET)"
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	@echo "$(ORANGE)Executable Removed$(RESET)"
	@$(RM) $(OBJDIR)

fclean:
	@echo "$(ORANGE)Executable Removed$(RESET)"
	@echo "$(RED)Object Files Removed$(RESET)"
	@$(RM) $(OBJDIR) $(NAME)

re: fclean clean all

-include $(DEPS)
