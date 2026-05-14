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
				mssg.cpp

OBJS		= $(SRCS:.cpp=.o)


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "\033[32m[SUCCESS] ft_irc compiled successfully!\033[0m"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@echo "\033[33m[CLEAN] Object files removed.\033[0m"

fclean: clean
	$(RM) $(NAME)
	@echo "\033[31m[FCLEAN] Executable removed.\033[0m"

re: fclean all

.PHONY: all clean fclean re