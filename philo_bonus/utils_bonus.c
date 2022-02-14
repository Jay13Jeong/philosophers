#include "philosophers_bonus.h"

//구조체를 초기화하는 함수
void	init_meta(t_config *c)
{
	int	i;

	sem_unlink("/pair_forks"); //만들려는 세마포어파일이 있을 지도 모르니 unlink로 미리 삭제
	sem_unlink("/print_section");
	c->forks = sem_open("/pair_forks", O_CREAT, S_IRWXU, c->p_cnt); //철학자 수 만큼 포크 세마포어 파일 생성
	if (c->forks == SEM_FAILED)
		clean_exit("Fork Semaphore Init Error\n", c, -1); //생성 실패시 에러처리
	c->print_section = sem_open("/print_section", O_CREAT, S_IRWXU, 1); //출력영역 세마포어 생성 (단일슬롯)
	if (c->print_section == SEM_FAILED)
		clean_exit("Print Semaphore Init Error\n", c, 0); //생성 실패시 에러처리
	i = -1;
	while (++i < (c->p_cnt)) //철학자 수 만큼 철학자의 좌석들도 초기화
	{
		c->phers[i].id = i;
		c->phers[i].eat = 0;
		c->phers[i].last_eat = 0;
		c->phers[i].config = c;
	}
}

//main의 인자를 검사하고 구조체 초기화하는 함수 
void	chk_and_init(t_config *c, int argc, char **argv)
{
	if (5 > argc || argc > 6)
		clean_exit("Invalid Args\n", c, -1); //인자가 4 ~ 5개가 아니면 에러처리
	c->p_cnt = ft_atoi(argv[1]);
	c->time_die = ft_atoi(argv[2]);
	c->time_eat = ft_atoi(argv[3]);
	c->time_sleep = ft_atoi(argv[4]);
	if (c->p_cnt < 1 || c->p_cnt > 200 || c->time_die < 0 //구조체에 넣은 정보를 검사
		|| c->time_eat < 0 || c->time_sleep < 0)
		clean_exit("Invalid Args\n", c, -1);
	c->eat_limit = -1; //최대 식사횟수 제한은 옵션이므로 5번째 인자가 없으면 -1
	if (argv[5]) //5번째 인자가 있다면 최대 식사횟수를 설정한다.
	{
		c->eat_limit = ft_atoi(argv[5]);
		if (c->eat_limit <= 0)
			clean_exit("Invalid Args\n", c, -1);
	}
	init_meta(c); //구조체 나머지부분 초기화
}

//현시각을 마이크로초 반환하는 함수
long long	get_time(t_config *c)
{
	gettimeofday(&c->val, NULL); //현시각 초, 마이크로초가 담긴 구조체를 반환 
	return ((c->val.tv_sec * 1000) + (c->val.tv_usec / 1000)); //하나의 마이크로초로 변환
}

//세마포어로 출력영역을 잠그고 출력하는 함수
void	print_lock(t_config *c, int id, char *str)
{
	long long	event_time;

	sem_wait(c->print_section);//세마포어로 출력영역 잠그기
	if (getppid() == 1) //부모프로세스가 종료되었다면 현재 프로세스도 그냥 종료
	{
		sem_post(c->print_section);
		exit(0);
	}
	event_time = get_time(c) - c->start_time;
	printf("%lli %i %s\n", event_time, id + 1, str); //행동개시 경과 시간과 매개변수로 받은 문자열을 출력한다.
	sem_post(c->print_section); //작업이 끝나면 영역 해제
}

//문자열을 int자료형으로 변환하는 함수
int	ft_atoi(char const *str)
{
	int			minus;
	long long	total;

	minus = 1;
	total = 0;
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '+' || *str == '-')
		if (*(str++) == '-')
			minus *= -1;
	while (*str >= '0' && *str <= '9')
	{
		total *= 10;
		total += *(str++) - '0';
	}
	if (total < 0)
	{
		if (minus < 0)
			return (0);
		else
			return (-1);
	}
	return (total * minus);
}
