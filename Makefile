NAME		= webserv
ROOT		= $(shell pwd)/www/
MODULES		= $(shell cd src && find * -type d)

SDIR		= src/
ODIR		= obj/
INCDIR		= include/
OBJDIR_NAME	= obj/
SRCDIR		= $(addprefix $(SDIR), $(MODULES))
OBJDIR		= $(addprefix $(ODIR), $(MODULES))

CXXFLAGS	= -Wall -Wextra -g3 -std=c++98 -D DEFAULT_ROOT=\"$(ROOT)\" -fsanitize=address # -Werror
INCLUDES	= $(addprefix -I $(INCDIR), $(MODULES))

SRCS		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
TEMP		= $(subst $(SDIR),$(ODIR),$(SRCS))
OBJS		= $(subst .cpp,.o,$(TEMP))
HEADERS		= $(foreach dir, $(INCLUDES), $(wildcard $(dir)/*.hpp))

vpath %.cpp $(SRCDIR)

define compile
$1/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $$< -o $$@
endef

$(foreach dir, $(OBJDIR), $(eval $(call compile, $(dir))))

all: mkdir $(NAME)

mkdir:
	@if [ ! -d "$(OBJDIR_NAME)" ]; then mkdir -p $(OBJDIR) ; fi

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re mkdir
