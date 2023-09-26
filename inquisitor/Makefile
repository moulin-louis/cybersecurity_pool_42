###############
## Variables ##
###############

NAME		=	inquisitor
LIB_INC		= -lpcap
INCLDIR		=	$(addsuffix /inc,$(LIB_INC) .)
BUILDIR		=	build
DEPDIR		=	$(BUILDIR)/.deps
SRCS_DIR	=	$(sort $(dir $(wildcard ./*/)))
SRC			=	$(addprefix ,				\
						$(addprefix src/,	\
							main.c			\
                        )					\
				)

OBJ			=	$(SRC:%.c=$(BUILDIR)/%.o)
DEP			=	$(SRC:%.c=$(DEPDIR)/%.d)
CXX			=	clang
CXXFLAGS	=	-O0 -g3 -Wall -Werror -Wextra -fsanitize=address
CPPFLAGS	=	$(addprefix -I, $(INCLDIR))
DEPFLAGS	=	-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
RM			=	/bin/rm -rf

# FG COLORS
DEFAULT		=	\033[0m
GREEN		=	\033[1;32m
YELLOW		=	\033[1;33m
BLUE		=	\033[1;34m
CYAN 		=	\033[1;36m

# TERMCAPS
UP			=	\033[1A
DELETE		=	\033[2K
DELPREV		=	$(UP)$(DELETE)\r

# EMOJI
CHECK		=	\xE2\x9C\x94

# DISPLAY
HEAD_SIZE	=	32
BODY_WIDTH	=	$(shell printf "$$(($(HEAD_SIZE) - 1))")

###########
## Rules ##
###########
all:
				@$(MAKE) -sk $(NAME)

$(BUILDIR)/%.o:	%.c | $(DEPDIR)
				@mkdir -p build/ $(addprefix build/, $(SRCS_DIR))
				@mkdir -p build/.deps/ $(addprefix build/.deps/, $(SRCS_DIR))
				@printf "$(YELLOW)Compiling $(BLUE)$@$(DEFAULT)$(YELLOW) and generating/checking make dependency file...$(DEFAULT)\n"
				$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
				@printf '$(DELPREV)%-*s$(GREEN)$(CHECK)$(DEFAULT)\n' $(BODY_WIDTH) $(notdir $@)

$(NAME):		$(OBJ)
				@printf "$(YELLOW)Linking source files and generating $(CYAN)$@$(DEFAULT) $(YELLOW)binary...\n$(DEFAULT)"
				$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^
				@printf "$(DELPREV)$(GREEN)Binary generated$(DEFAULT)\n"

$(DEPDIR):
				@printf "$(YELLOW)Creating $(BLUE)$@$(DEFAULT)$(YELLOW) folder...$(DEFAULT)\n"
				@mkdir -p $@
				@printf "$(DELPREV)"

clean:
				@printf "$(YELLOW)Deleting object and dependency files...$(DEFAULT)\n"
				@$(RM) $(OBJ)
				@printf "$(DELPREV)Build files deleted\n"

fclean:			clean
				@printf "$(YELLOW)Deleting build directory...$(DEFAULT)\n"
				$(RM) $(BUILDIR) $(NAME)
				@printf "$(DELPREV)Build directory and binary deleted\n"

re:				fclean
				$(MAKE) -s all

$(DEP):
				-include $(wildcard $(DEP))

.PHONY:			all clean fclean re
