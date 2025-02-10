NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./inc

RM = rm -rf

OBJDIR = obj

SRC = src/main.cpp src/utils.cpp src/Server.cpp src/Channel.cpp src/Client.cpp src/CommandHandler.cpp src/Commands.cpp
INC = ./inc
OBJ	= $(SRC:src/%.cpp=$(OBJDIR)/%.o)

GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m

all: $(NAME)

$(OBJDIR)/%.o: src/%.cpp $(INC)
	@mkdir -p $(OBJDIR)
	@echo "Compiling $< ..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

$(NAME):  $(OBJ)
	@echo "$(GREEN)Compiled $(NAME) successfully!$(NC)"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

clean:
	@echo "$(RED)Clearing files and obj$(NC)"
	@$(RM) $(OBJDIR)

fclean: clean
	@echo "$(RED)Removing $(NAME) ...$(NC)"
	@$(RM) $(NAME)
	@$(RM) .vscode

re: fclean all
