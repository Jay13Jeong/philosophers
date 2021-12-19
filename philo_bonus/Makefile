NAME	= philo_bonus
SRCS	= main.c utils_bonus.c utils2_bonus.c
OBJS	= $(SRCS:.c=.o)
HEADER	= philosophers_bonus.h
CC		= gcc
FLAGS	= -Wall -Wextra -Werror

all	: $(NAME)

bonus : $(NAME)

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
