#include "philosophers_bonus.h"

void	*death_monitor(void *p_void)
{
	long long		curr_time;
	t_philosopher	*p;
	int				sig[1];

	p = (t_philosopher *)p_void;
	while (1)
	{
		curr_time = get_time(p->config);
		if ((p->config->eat_limit) >= 0 && (p->eat >= (p->config->eat_limit)))
			break ;
		else if ((curr_time - (p->last_eat)) > (p->config->time_die))
		{
			sem_wait(p->config->print_section);
			printf("%lli ", get_time(p->config) - (p->config->start_time));
			printf("%i %s\n", p->id + 1, "died");
			sem_wait(p->config->sig_section);
			sig[0] = 500;
			write((p->config->pipe[1]), sig, sizeof(unsigned char));
			kill(getppid(), SIGUSR1);
			break ;
		}
		usleep(1000);
	}
	sleep(1);
	exit(0);
}

void	eat_spaghetti(t_philosopher *p)
{
	t_config		*c;
	int				sig[1];

	c = p->config;
	sem_wait(c->forks);
	print_lock(c, p->id, "has taken a fork");
	print_lock(c, p->id, "has taken a fork");
	p->last_eat = get_time(c);
	print_lock(c, p->id, "is eating");
	p->eat += 1;
	if ((c->eat_limit) >= 0 && ((p->eat) >= (c->eat_limit)))
	{
		sem_wait(c->sig_section);
		sig[0] = 1;
		write(c->pipe[1], sig, sizeof(int));
		kill(getppid(), SIGUSR1);
		sem_post(c->sig_section);
	}
	while ((get_time(c) - p->last_eat) < c->time_eat)
		usleep(100);
	sem_post(c->forks);
	while ((c->eat_limit) >= 0 && ((p->eat) >= (c->eat_limit)))
		sleep(1);
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
		usleep(500);
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

void	exchange_pipe(t_config *c)
{
	int				temp[1];
	int				sig[1];

	sig[0] = 0;
	while (1)
	{
		pause();
		read(c->pipe[0], temp, sizeof(int));
		sig[0] += temp[0];
		if (sig[0] >= c->p_cnt)
			clean_exit("", c, c->p_cnt);
	}
}

int	main(int argc, char **argv)
{
	t_config		c;
	int				i;


	chk_and_init(&c, argc, argv);
	pipe(c.pipe);
	signal(SIGUSR1, dummy);
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
	exchange_pipe(&c);
	return (0);
}
