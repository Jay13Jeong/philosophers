#include "philosophers_bonus.h"

void	*death_monitor(void *p_void)
{
	long long		curr_time;
	t_philosopher	*p;

	p = (t_philosopher *)p_void;
	while (1)
	{
		curr_time = get_time(p->config);
		if ((curr_time - (p->last_eat)) > (p->config->time_die))
		{
			if (getppid() == 1)
				exit(0);
			sem_wait(p->config->print_section);
			printf("%lli ", get_time(p->config) - (p->config->start_time));
			printf("%i %s\n", p->id + 1, "died");
			sem_wait(p->config->sig_section);
			kill(getppid(), 9);
			sem_post(p->config->sig_section);
			sem_post(p->config->print_section);
			exit(0);
		}
		usleep(1000);
	}
}

void	eat_spaghetti(t_philosopher *p)
{
	t_config		*c;

	c = p->config;
	sem_wait(c->forks);
	print_lock(c, p->id, "has taken a fork");
	sem_wait(c->forks);
	print_lock(c, p->id, "has taken a fork");
	p->last_eat = get_time(c);
	print_lock(c, p->id, "is eating");
	p->eat += 1;
	if ((c->eat_limit) >= 0 && ((p->eat) >= (c->eat_limit)))
	{
		sem_post(c->forks);
		sem_post(c->forks);
		exit(0);
	}
	while ((get_time(c) - p->last_eat) < c->time_eat)
		usleep(100);
	sem_post(c->forks);
	sem_post(c->forks);
}

void	*philosopher(t_philosopher *p)
{
	t_config		*c;
	long long		sleep_start;

	c = p->config;
	p->last_eat = get_time(c);
	pthread_create(&(p->thread_id), 0, death_monitor, p);
	while (c->p_cnt == 1)
		sleep(1);
	if (p->id % 2)
		usleep(15000);
	while (1)
	{
		eat_spaghetti(p);
		print_lock(c, p->id, "is sleeping");
		sleep_start = get_time(c);
		while ((get_time(c) - sleep_start) < c->time_sleep)
			usleep(100);
		print_lock(c, p->id, "is thinking");
		
	}
}

void	clean_exit(char *str, t_config *c, int pid_cnt)
{
	int	i;

	i = -1;
	while (++i < pid_cnt)
		kill(c->phers[i].pid, 9);
	if (i >= 0)
	{
		sem_close(c->forks);
		sem_close(c->print_section);
		sem_unlink("/pair_forks");
		sem_unlink("/print_section");
	}
	printf("%s", str);
	exit(0);
}

int	main(int argc, char **argv)
{
	t_config		c;
	int				i;

	chk_and_init(&c, argc, argv);
	i = -1;
	c.start_time = get_time(&c);
	while (++i < c.p_cnt)
	{
		c.phers[i].pid = fork();
		if (c.phers[i].pid == -1)
			clean_exit("Process Creat Error\n", &c, i);
		c.phers[i].last_eat = get_time(&c);
		if (c.phers[i].pid == CHILD)
			philosopher(&(c.phers[i]));
	}
	pause();
	return (0);
}
