#include "philosophers.h"

//스레드들을 돌리면서 죽음 상태도 검사하는 함수 
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
			if ((get_time(c) - c->phers[i].last_eat) > c->time_die) //공복시간이 초과됐는지 검사
			{
				c->survival = 0; /*초과되면 죽음처리한다 */
				print_lock(c, i, "died", 1);
				clean_exit("", c, 1);
			}
			if (c->eat_limit >= 0 && (c->phers[i].eat >= c->eat_limit)) //식사 최대치까지 먹은 철학자 카운트
				eat_cnt += 1;
		}
		if (eat_cnt == c->p_cnt) //모든 철학자가 최대치까지 먹으면 모든 행동을 종료한다
			c->survival = 0;
		if (!(c->survival))
			clean_exit("", c, 1);
		usleep(1000);
	}
}

//식사 함수
void	eat_spaghetti(t_philosopher *p)
{
	t_config	*c;

	c = p->config;
	pthread_mutex_lock(&(c->forks[p->left_fork])); //왼쪽포크에 뮤텍스를 걸고 포크를 집는다.
	print_lock(c, p->id, "has taken a fork", 0);
	pthread_mutex_lock(&(c->forks[p->right_fork])); //오른쪽포크에 뮤텍스를 걸고 포크를 집는다.
	print_lock(c, p->id, "has taken a fork", 0);
	print_lock(c, p->id, "is eating", 0); //출력영역에 뮤텍스를 걸고 식사 상태 출력.
	p->eat += 1; //현재 철학자의 식사횟수 +1
	p->last_eat = get_time(c); //마지막 식사시간 업데이트
	while ((get_time(c) - p->last_eat) < c->time_eat) //지정된 시간동안 식사
		usleep(100);
	pthread_mutex_unlock(&(c->forks[p->left_fork])); //식사가 끝나면 모든 포크 뮤텍스 해제
	pthread_mutex_unlock(&(c->forks[p->right_fork]));
}

//생성된 스레드가 할 행동 함수
void	*philo_thread(void *p_void)
{
	t_philosopher	*p;
	t_config		*c;
	long long		sleep_start;

	p = (t_philosopher *)p_void; //void철학자 구조체를 원래 형태로 캐스팅
	if (p->left_fork == p->right_fork) //철학자가 혼자면 아무것도 안함
		return (NULL);
	c = p->config;
	if (p->id % 2) //짝수 인덱스 철학자는 데드락을 예방하기 위해 잠시 대기
		usleep(15000);
	while ((c->eat_limit) == -1 || (p->eat) < (c->eat_limit)) //최대 식사횟수가 초과하기 전까지 행동, -1은 최대치가 설정 되지 않은 상태
	{
		eat_spaghetti(p); //스파게티 식사
		print_lock(c, p->id, "is sleeping", 0); //잠에 드는 상태 출력
		sleep_start = get_time(c);
		while ((get_time(c) - sleep_start) < c->time_sleep) //지정된 시간동안 잔다
			usleep(100);
		print_lock(c, p->id, "is thinking", 0); //일어난 상태 출력
	}
	return (NULL);
}

//동적할당 된 포크와 철학자의 메모리 해제하는 함수
void	clean_exit(char *str, t_config *c, int dyn)
{
	printf("%s", str); //매개변수로 받은 문자열 출력
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

	chk_and_init(&c, argc, argv); //인자를 검사하고 구조체 초기화
	p = c.phers;
	i = -1;
	c.start_time = get_time(&c); //행동 시작 시각을 마이크로초로 변수에 저장 
	while (++i < c.p_cnt) //철학자 수 만큼 스레드를 생성
	{
		if (pthread_create(&(p[i].thread_id), NULL, philo_thread, &(p[i])))
			clean_exit("Thread Creat Error\n", &c, 1); //생성 실패하면 에러처리
		p[i].last_eat = get_time(&c); //현재 생성된 스레드의 마지막 식사시간 초기화
	}
	thread_loop(&c); //철학자들의 죽음을 검사하면서 스레드들을 동시에 움직인다.
	return (0);
}
