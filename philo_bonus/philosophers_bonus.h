#ifndef PHILOSOPHERS_BONUS_H
# define PHILOSOPHERS_BONUS_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <semaphore.h>
# include <fcntl.h>
# include <signal.h>

# define CHILD 0

typedef struct s_philosopher
{
	int					id;
	int					eat;
	long long			last_eat;
	struct s_config		*config;
	pthread_t			thread_id;
	pid_t				pid;
}				t_philosopher;

typedef struct s_config
{
	int					p_cnt;
	int					time_die;
	int					time_eat;
	int					time_sleep;
	int					eat_limit;
	long long			start_time;
	t_philosopher		phers[200];
	sem_t				*sig_section;
	sem_t				*forks;
	sem_t				*print_section;
	struct timeval		val;
}				t_config;

void		clean_exit(char *str, t_config *c, int pid_cnt);
void		chk_and_init(t_config *c, int argc, char **argv);
void		print_lock(t_config *c, int id, char *str);
long long	get_time(t_config *c);
int			ft_atoi(const char *str);

#endif
