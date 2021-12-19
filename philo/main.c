#include "philosophers.h"

void	thread_loop(t_config *c)
{
	int			i;
	int			eat_cnt;

	while (1)
	{
		eat_cnt = 0;
		i = -1;
		while (++i < c->p_cnt)
		{
			if ((get_time(c) - c->phers[i].last_eat) > c->time_die)
			{
				c->survival = 0;
				print_lock(c, i, "died", 1);
				clean_exit("", c, 1);
			}
			if (c->eat_limit >= 0 && (c->phers[i].eat >= c->eat_limit))
				eat_cnt += 1;
		}
		if (eat_cnt == c->p_cnt)
			c->survival = 0;
		if (!(c->survival))
			clean_exit("", c, 1);
		usleep(1000);
	}
}

void	eat_spaghetti(t_philosopher *p)
{
	t_config	*c;

	c = p->config;
	pthread_mutex_lock(&(c->forks[p->left_fork]));
	print_lock(c, p->id, "has taken a fork", 0);
	pthread_mutex_lock(&(c->forks[p->right_fork]));
	print_lock(c, p->id, "has taken a fork", 0);
	print_lock(c, p->id, "is eating", 0);
	p->eat += 1;
	p->last_eat = get_time(c);
	while ((get_time(c) - p->last_eat) < c->time_eat)
		usleep(100);
	pthread_mutex_unlock(&(c->forks[p->left_fork]));
	pthread_mutex_unlock(&(c->forks[p->right_fork]));
}

void	*philo_thread(void *p_void)
{
	t_philosopher	*p;
	t_config		*c;
	long long		sleep_start;

	p = (t_philosopher *)p_void;
	if (p->left_fork == p->right_fork)
		return (NULL);
	c = p->config;
	if (p->id % 2)
		usleep(500);
	while ((c->eat_limit) == -1 || (p->eat) < (c->eat_limit))
	{
		eat_spaghetti(p);
		print_lock(c, p->id, "is sleeping", 0);
		sleep_start = get_time(c);
		while ((get_time(c) - sleep_start) < c->time_sleep)
			usleep(100);
		print_lock(c, p->id, "is thinking", 0);
	}
	return (NULL);
}

void	clean_exit(char *str, t_config *c, int dyn)
{
	printf("%s", str);
	if (dyn)
	{
		free(c->forks);
		free(c->phers);
	}
	exit(0);
}

int	main(int argc, char **argv)
{
	t_config		c;
	t_philosopher	*p;
	int				i;

	chk_and_init(&c, argc, argv);
	p = c.phers;
	i = -1;
	c.start_time = get_time(&c);
	while (++i < c.p_cnt)
	{
		if (pthread_create(&(p[i].thread_id), NULL, philo_thread, &(p[i])))
			clean_exit("Thread Creat Error\n", &c, 1);
		p[i].last_eat = get_time(&c);
	}
	thread_loop(&c);
	return (0);
}
