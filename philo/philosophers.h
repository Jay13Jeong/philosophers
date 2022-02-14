#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_philosopher //철학자 구조체
{
	int					id; //구분번호
	int					eat; //식사 횟수
	int					left_fork; //왼쪽 포크번호
	int					right_fork; //오른쪽 포크번호
	long long			last_eat; //마지막 식사 시간
	struct s_config		*config; //설정 정보
	pthread_t			thread_id; //철학자 스레드의 스레드 아이디
}				t_philosopher;

typedef struct s_config
{
	int					p_cnt; //철학자 수
	int					time_die; //배고픔을 참는 시간
	int					time_eat; //식사 걸리는 시간
	int					time_sleep; //잠 걸리는 시간
	int					survival; //살아 있으면 1 아니면 0
	int					eat_limit; //제한된 최대 식사 횟수
	long long			start_time; //행동 시작 시각
	t_philosopher		*phers; //철학자들
	pthread_mutex_t		*forks; //포크들
	pthread_mutex_t		print_section; //출력영역
	struct timeval		val; //시각을 담는 구조체
}				t_config;

void		clean_exit(char *str, t_config *c, int dyn);
void		chk_and_init(t_config *c, int argc, char **argv);
void		print_lock(t_config *c, int id, char *str, int force);
long long	get_time(t_config *c);
int			ft_atoi(const char *str);

#endif
