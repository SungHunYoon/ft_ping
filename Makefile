CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRCS = ./ping.c ./ping_packet.c ./ping_parsecmd.c ./ping_process.c ./ping_utils.c
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
