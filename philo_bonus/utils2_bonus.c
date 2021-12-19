#include "philosophers_bonus.h"

void	clean_exit(char *str, t_config *c, int pid_cnt)
{
	int	i;

	i = -1;
	while (++i < pid_cnt)
		kill(c->phers[i].pid, 9);
	if (i >= 0)
	{
		sem_close(c->sig_section);
		sem_close(c->forks);
		sem_close(c->print_section);
		sem_unlink("/sig_section");
		sem_unlink("/pair_forks");
		sem_unlink("/print_section");
	}
	printf("%s", str);
	exit(0);
}

void	dummy(int sig)
{
	sig = 0;
}