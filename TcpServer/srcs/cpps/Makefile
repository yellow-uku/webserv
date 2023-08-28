NAME		=	server
ifeq ($(OS), Windows_NT)
	NAME	=	server.exe
endif
CPP			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -g -ggdb3 -fsanitize=address #-std=c++98

SRCS		=	$(wildcard *.cpp)

HPPS		=	$(wildcard ../headers/*.hpp)

OBJS		=	$(SRCS:.cpp=.o)
ifeq ($(OS),Windows_NT)
	RM		=   del
else
	RM		=	rm -rf
endif
%.o:%.cpp $(HPPS)
	$(CPP) $(CXXFLAGS) -c $< -o $@

all:	$(NAME)

$(NAME):	$(OBJS)
			$(CPP) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:	all clean fclean re
