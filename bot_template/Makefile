NAME = bot_template

CXX = c++
CXXFLAGS = -g -std=c++17

# TODO: Add all source files
SRCS = $(shell find . -name "*.cpp")
HEADER = $(shell find . -name "*.hpp")

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
