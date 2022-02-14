#include "philosophers_bonus.h"

//철학자의 죽음을 실시간으로 검사하는 함수
void	*death_monitor(void *p_void)
{
	long long		curr_time;
	t_philosopher	*p;

	p = (t_philosopher *)p_void; //스레드를 생성하면서 넘긴 void포인트를 원래 형식으로 되돌리기
	while (1)
	{
		curr_time = get_time(p->config); //식사시간을 검사하기위한 기준시각이 필요하므로 현재시각을 기준으로 삼는다
		if ((curr_time - (p->last_eat)) > (p->config->time_die)) //만약 최대 공복시간을 넘어가면 죽음 처리한다
		{
			if (getppid() == 1) //부모프로세스가 사라지면 자식프로세스는 1을 가리킴, 1을 가리키는 건 모든 행동이 종료 됨을 의미
				exit(0);
			sem_wait(p->config->print_section); /*세마포어로 출력영역을 잠그고 죽음상태를 출력 */
			printf("%lli ", get_time(p->config) - (p->config->start_time));
			printf("%i %s\n", p->id + 1, "died");
			sem_wait(p->config->sig_section); //신호역역을 잠그고 부모프로세스를 종료한다.
			kill(getppid(), 9);
			sem_post(p->config->sig_section);
			sem_post(p->config->print_section);
			exit(0);
		}
		usleep(1000);
	}
}

//스파게티 식사 함수
void	eat_spaghetti(t_philosopher *p)
{
	t_config		*c;

	c = p->config;
	sem_wait(c->forks); //세마포어로 포크영역에 1만큼 락을 건다
	print_lock(c, p->id, "has taken a fork"); //두개 중 하나를 집는다.
	sem_wait(c->forks); //추가로 세마포어로 포크영역에 1만큼 락을 건다
	print_lock(c, p->id, "has taken a fork"); //하나를 더 집는다.
	p->last_eat = get_time(c); //마지막 식사시간을 업데이트
	print_lock(c, p->id, "is eating"); //식사 상태를 출력
	p->eat += 1; //현재 철학자의 식사 횟수 +1
	if ((c->eat_limit) >= 0 && ((p->eat) >= (c->eat_limit))) //식사 횟수가 제한된 횟수를 넘기면 포크를 반납하고 종료 
	{
		sem_post(c->forks);
		sem_post(c->forks);
		exit(0);
	}
	while ((get_time(c) - p->last_eat) < c->time_eat) //지정한 시간동안 식사
		usleep(100);
	sem_post(c->forks); //식사가 끝났으니 포크 모두 반납
	sem_post(c->forks);
}

//철학자의 행동 처리하는 함수
void	*philosopher(t_philosopher *p)
{
	t_config		*c;
	long long		sleep_start;

	c = p->config;
	p->last_eat = get_time(c); //철학자의 마지막 식사시간을 현재 시각으로 초기화
	pthread_create(&(p->thread_id), 0, death_monitor, p); //스레드를 만들어서 상태를 실시간으로 검사
	while (c->p_cnt == 1) //혼자면 포크가 1개라서 식사를 못하므로 그냥 기다리다가 죽는다.
		sleep(1);
	if (p->id % 2) //짝수 인덱스의 철학자들은 잠시 기다렸다가 행동을 시작한다.
		usleep(15000);
	while (1)
	{
		eat_spaghetti(p); //스파게티 먹기
		print_lock(c, p->id, "is sleeping"); /* 식사를 끝내고 잠에 든다 */
		sleep_start = get_time(c);
		while ((get_time(c) - sleep_start) < c->time_sleep)
			usleep(100);
		print_lock(c, p->id, "is thinking"); //일어났으면 생각한다.
		
	}
}

//생성했던 세마포어를 제거하고 모든 프로세스를 종료하는 함수
void	clean_exit(char *str, t_config *c, int pid_cnt)
{
	int	i;

	i = -1;
	while (++i < pid_cnt)
		kill(c->phers[i].pid, 9); //자식프로세를 모두 종료
	if (i >= 0)
	{
		sem_close(c->forks); /*세마포어를 닫고 삭제 */
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

	chk_and_init(&c, argc, argv); //매개변수가 올바른지 검사하고 구조체 초기화
	i = -1;
	c.start_time = get_time(&c); //행동 시작 시각을 마이크로초로 변수에 저장
	while (++i < c.p_cnt)
	{
		c.phers[i].pid = fork(); //자식프로세스는 0을 얻고, 부모프로세스는 자식프로세스의 pid를 얻음
		if (c.phers[i].pid == -1) //프로세스 생성 실패하면 에러처리
			clean_exit("Process Creat Error\n", &c, i);
		// c.phers[i].last_eat = get_time(&c);
		if (c.phers[i].pid == CHILD) //현재 프로세스가 자식이면 먹고 자고 생각한다.
			philosopher(&(c.phers[i]));
	}
	pause(); //신호 대기
	return (0);
}
