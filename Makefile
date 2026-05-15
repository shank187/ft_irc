NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

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

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@printf "\033[33m[CLEAN] Object files removed.\033[0m\n"

fclean: clean
	$(RM) $(NAME)
	@printf "\033[31m[FCLEAN] Executable removed.\033[0m\n"
re: fclean all

.PHONY: all clean fclean re