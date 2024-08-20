CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRCS = ./ping.c
OBJS = $(SRCS:.c=.o)
NAME = ft_ping

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : 
	make fclean
	make all

.PHONY : all clean fclean re
