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

# define CHILD 0 //자식프로세스가 받는 pid (fork() 반환 값이 0 이면 자식 프로세스)

typedef struct s_philosopher //철학자 구조체
{
	int					id; //구분번호
	int					eat; //식사 횟수
	long long			last_eat; //마지막 식사 시간
	struct s_config		*config; //설정 정보
	pthread_t			thread_id; //죽음 감지 스레드의 스레드 아이디
	pid_t				pid; //부모 프로세스가 사용 할 자식프로세스의 pid
}				t_philosopher;

typedef struct s_config
{
	int					p_cnt; //철학자 수
	int					time_die; //배고픔을 참는 시간
	int					time_eat; //식사하는데 걸리는 시간
	int					time_sleep; //자는데 걸리는 시간
	int					eat_limit; //한 명당 최대 먹을 수 있는 횟수
	long long			start_time; //전원 행동 시작 시각
	t_philosopher		phers[200]; //철학자 좌석 수(200명까지 지원)
	sem_t				*sig_section; //신호 세마포어 (슬롯 1개)
	sem_t				*forks; //포크 세마포어 (포크 갯수만큼)
	sem_t				*print_section; //출력 세마포어 (슬롯 1개)
	struct timeval		val; //시각을 담는 구조체
}				t_config;

void		clean_exit(char *str, t_config *c, int pid_cnt);
void		chk_and_init(t_config *c, int argc, char **argv);
void		print_lock(t_config *c, int id, char *str);
long long	get_time(t_config *c);
int			ft_atoi(const char *str);

#endif
