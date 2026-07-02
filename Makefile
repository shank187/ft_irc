NAME		= ircserv
BOT_NAME = bot

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

BOT_SRCS = bonus/bot.cpp
BOT_OBJS = $(BOT_SRCS:.cpp=.o)

RM			= rm -f

SRCS		= main.cpp \
				Server.cpp \
				Core.cpp \
				process_input.cpp \
				Client.cpp \
				Channel.cpp \
				mssg.cpp \
				cmds/cmd_invite.cpp \
				cmds/cmd_join.cpp\
				cmds/cmd_kick.cpp\
				cmds/cmd_mode.cpp\
				cmds/cmd_part.cpp\
				cmds/cmd_privmsg.cpp\
				cmds/cmd_topic.cpp

OBJS		= $(SRCS:.cpp=.o)


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@printf "\033[32m[SUCCESS] ft_irc compiled successfully!\033[0m\n"

bonus: $(BOT_NAME)

$(BOT_NAME): $(BOT_OBJS)
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $(BOT_NAME)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(BOT_OBJS)
	@printf "\033[33m[CLEAN] Object files removed.\033[0m\n"

fclean: clean
	$(RM) $(NAME) $(RM) $(BOT_NAME)
	@printf "\033[31m[FCLEAN] Executable removed.\033[0m\n"
re: fclean all

.PHONY: all clean fclean re