NAME = bot

CXX = c++
CXXFLAGS = -g -std=c++17 -Wall -Werror -Wextra

# TODO: Add all source files
SRCS = Bot.cpp BotSocket.cpp BotServer.cpp
HEADER = Bot.hpp BotSocket.hpp BotServer.hpp

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

run: all
	./$(NAME)

re: fclean all
