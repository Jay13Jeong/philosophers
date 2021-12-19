NAME	= philo
SRCS	= main.c utils.c
OBJS	= $(SRCS:.c=.o)
HEADER	= philosophers.h
CC		= gcc
FLAGS	= -Wall -Wextra -Werror

all	: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJS) -I.$(HEADER)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

%.o : %.c
	$(CC) $(FLAGS) -c $< -o $(<:.c=.o)

.PHONY : clean fclean re all
