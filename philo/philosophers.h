#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_philosopher
{
	int					id;
	int					eat;
	int					left_fork;
	int					right_fork;
	long long			last_eat;
	struct s_config		*config;
	pthread_t			thread_id;
}				t_philosopher;

typedef struct s_config
{
	int					p_cnt;
	int					time_die;
	int					time_eat;
	int					time_sleep;
	int					survival;
	int					eat_limit;
	long long			start_time;
	t_philosopher		*phers;
	pthread_mutex_t		*forks;
	pthread_mutex_t		print_section;
	struct timeval		val;
}				t_config;

void		clean_exit(char *str, t_config *c, int dyn);
void		chk_and_init(t_config *c, int argc, char **argv);
void		print_lock(t_config *c, int id, char *str, int force);
long long	get_time(t_config *c);
int			ft_atoi(const char *str);

#endif
