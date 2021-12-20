#include "philosophers_bonus.h"

void	init_meta(t_config *c)
{
	int	i;

	sem_unlink("/pair_forks");
	sem_unlink("/print_section");
	c->forks = sem_open("/pair_forks", O_CREAT, S_IRWXU, c->p_cnt);
	if (c->forks == SEM_FAILED)
		clean_exit("Fork Semaphore Init Error\n", c, -1);
	c->print_section = sem_open("/print_section", O_CREAT, S_IRWXU, 1);
	if (c->print_section == SEM_FAILED)
		clean_exit("Print Semaphore Init Error\n", c, 0);
	i = -1;
	while (++i < (c->p_cnt))
	{
		c->phers[i].id = i;
		c->phers[i].eat = 0;
		c->phers[i].last_eat = 0;
		c->phers[i].config = c;
	}
}

void	chk_and_init(t_config *c, int argc, char **argv)
{
	if (5 > argc || argc > 6)
		clean_exit("Invalid Args\n", c, -1);
	c->p_cnt = ft_atoi(argv[1]);
	c->time_die = ft_atoi(argv[2]);
	c->time_eat = ft_atoi(argv[3]);
	c->time_sleep = ft_atoi(argv[4]);
	if (c->p_cnt < 1 || c->p_cnt > 200 || c->time_die < 0
		|| c->time_eat < 0 || c->time_sleep < 0)
		clean_exit("Invalid Args\n", c, -1);
	c->eat_limit = -1;
	if (argv[5])
	{
		c->eat_limit = ft_atoi(argv[5]);
		if (c->eat_limit <= 0)
			clean_exit("Invalid Args\n", c, -1);
	}
	init_meta(c);
}

long long	get_time(t_config *c)
{
	gettimeofday(&c->val, NULL);
	return ((c->val.tv_sec * 1000) + (c->val.tv_usec / 1000));
}

void	print_lock(t_config *c, int id, char *str)
{
	long long	event_time;

	sem_wait(c->print_section);
	if (getppid() == 1)
	{
		sem_post(c->print_section);
		exit(0);
	}
	event_time = get_time(c) - c->start_time;
	printf("%lli %i %s\n", event_time, id + 1, str);
	sem_post(c->print_section);
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
