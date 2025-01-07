NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror #-std=c++98

SRCS = core/main.cpp \
	   core/Server.cpp \
	   modules/Client.cpp \
	   modules/Channel.cpp \
	   commands/CommandHandler.cpp \
	   bonus/Bot.cpp \
	   bonus/FileTransfer.cpp

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

re: fclean all
