#include "philosophers.h"

void	init_people(t_config *c)
{
	int	i;

	c->phers = malloc(sizeof(t_philosopher) * c->p_cnt);
	if (c->phers == NULL)
	{
		free(c->forks);
		clean_exit("Malloc Error\n", c, 0);
	}
	i = -1;
	while (++i < (c->p_cnt))
	{
		c->phers[i].id = i;
		c->phers[i].eat = 0;
		c->phers[i].left_fork = i;
		c->phers[i].right_fork = (i + 1) % c->p_cnt;
		c->phers[i].last_eat = 0;
		c->phers[i].config = c;
		if (pthread_mutex_init(&(c->forks[i]), NULL))
			clean_exit("mutex init error\n", c, 1);
	}
}

void	chk_and_init(t_config *c, int argc, char **argv)
{
	if (5 > argc || argc > 6)
		clean_exit("Invalid Args\n", c, 0);
	c->p_cnt = ft_atoi(argv[1]);
	c->time_die = ft_atoi(argv[2]);
	c->time_eat = ft_atoi(argv[3]);
	c->time_sleep = ft_atoi(argv[4]);
	c->survival = 1;
	if (c->p_cnt < 1 || c->p_cnt > 200 || c->time_die < 0
		|| c->time_eat < 0 || c->time_sleep < 0)
		clean_exit("Invalid Args\n", c, 0);
	c->eat_limit = -1;
	if (argv[5])
	{
		c->eat_limit = ft_atoi(argv[5]);
		if (c->eat_limit <= 0)
			clean_exit("Invalid Args\n", c, 0);
	}
	c->forks = malloc(sizeof(pthread_mutex_t) * c->p_cnt);
	if (c->forks == NULL)
		clean_exit("Malloc Error\n", c, 0);
	init_people(c);
	if (pthread_mutex_init(&(c->print_section), NULL))
		clean_exit("mutex init error\n", c, 1);
}

long long	get_time(t_config *c)
{
	gettimeofday(&c->val, NULL);
	return ((c->val.tv_sec * 1000) + (c->val.tv_usec / 1000));
}

void	print_lock(t_config *c, int id, char *str, int force)
{
	long long	event_time;

	pthread_mutex_lock(&c->print_section);
	event_time = get_time(c) - c->start_time;
	if (c->survival || force)
		printf("%lli %i %s\n", event_time, id + 1, str);
	pthread_mutex_unlock(&c->print_section);
	return ;
}

int	ft_atoi(char const *str)
{
	int			minus;
	long long	total;

	minus = 1;
	total = 0;
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '+' || *str == '-')
		if (*(str++) == '-')
			minus *= -1;
	while (*str >= '0' && *str <= '9')
	{
		total *= 10;
		total += *(str++) - '0';
	}
	if (total < 0)
	{
		if (minus < 0)
			return (0);
		else
			return (-1);
	}
	return (total * minus);
}
