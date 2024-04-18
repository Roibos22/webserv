NAME = webserv
CC	= c++
FLAGS	= -Wall -Werror -Wextra -g3 -std=c++98
SRCS	= main.cpp \
		 request.cpp \
		 response.cpp \
		 connection.cpp \
		 response_util.cpp \
		 response_util2.cpp \
		 post-del-new.cpp \
		 cgi.cpp \
		./Config/configParser.cpp \
		./Config/Lexir.cpp \
		./Config/readFile.cpp \
		./Config/utils.cpp \

OBJS = $(SRCS:.cpp=.o)
all: $(NAME)
%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(NAME): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all

.PHONY: all clean fclean re