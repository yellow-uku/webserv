NAME		= webserv
MODULES		= $(shell cd src && find * -type d)

SDIR		= src/
ODIR		= obj/
INCDIR		= include/
SRCDIR		= $(addprefix $(SDIR), $(MODULES))
OBJDIR		= $(addprefix $(ODIR), $(MODULES))

CXXFLAGS	= -Wall -Wextra -g -std=c++98# -fsanitize=address# -Werror
INCLUDES	= $(addprefix -I $(INCDIR), $(MODULES))

SRCS		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
TEMP		= $(subst $(SDIR),$(ODIR),$(SRCS))
OBJS		= $(subst .cpp,.o,$(TEMP))

vpath %.cpp $(SRCDIR)

define compile
$1/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $$< -o $$@
endef 

$(foreach dir, $(OBJDIR), $(eval $(call compile, $(dir))))

all: mkdir $(NAME)

mkdir:
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re mkdir
