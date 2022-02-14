#include "philosophers.h"

//구조체의 철학자부분 초기화 함수 
void	init_people(t_config *c)
{
	int	i;

	c->phers = malloc(sizeof(t_philosopher) * c->p_cnt); //철학자들 동적할당
	if (c->phers == NULL)
	{
		free(c->forks);
		clean_exit("Malloc Error\n", c, 0);
	}
	i = -1;
	while (++i < (c->p_cnt)) //모든 동적할당된 철학자를 초기화
	{
		c->phers[i].id = i;
		c->phers[i].eat = 0;
		c->phers[i].left_fork = i;
		c->phers[i].right_fork = (i + 1) % c->p_cnt;
		c->phers[i].last_eat = 0;
		c->phers[i].config = c;
		if (pthread_mutex_init(&(c->forks[i]), NULL)) //철학자 수 만큼 포크도 동시에 뮤텍스 생성한다.
			clean_exit("mutex init error\n", c, 1);
	}
}

//main에서 받아온 인자를 검사하고 구조체를 초기화하는 함수
void	chk_and_init(t_config *c, int argc, char **argv)
{
	if (5 > argc || argc > 6) //인자가 4 ~ 5개가 아니면 에러처리
		clean_exit("Invalid Args\n", c, 0);
	c->p_cnt = ft_atoi(argv[1]);
	c->time_die = ft_atoi(argv[2]);
	c->time_eat = ft_atoi(argv[3]);
	c->time_sleep = ft_atoi(argv[4]);
	c->survival = 1;
	if (c->p_cnt < 1 || c->p_cnt > 200 || c->time_die < 0 //구조체에 넣은 정보 검사
		|| c->time_eat < 0 || c->time_sleep < 0)
		clean_exit("Invalid Args\n", c, 0);
	c->eat_limit = -1; //최대 식사횟수 제한은 옵션이므로 5번째 인자가 없으면 -1
	if (argv[5]) //5번째 인자가 있다면 최대 식사횟수를 설정한다.
	{
		c->eat_limit = ft_atoi(argv[5]);
		if (c->eat_limit <= 0)
			clean_exit("Invalid Args\n", c, 0);
	}
	c->forks = malloc(sizeof(pthread_mutex_t) * c->p_cnt); //철학자 수 만큼 포크 동적할당
	if (c->forks == NULL)
		clean_exit("Malloc Error\n", c, 0);
	init_people(c); //철학자 구조체를 초기화
	if (pthread_mutex_init(&(c->print_section), NULL)) //출력영역 뮤텍스 생성
		clean_exit("mutex init error\n", c, 1);
}

//현시각을 마이크로초 반환하는 함수
long long	get_time(t_config *c)
{
	gettimeofday(&c->val, NULL); //현시각 초, 마이크로초가 담긴 구조체를 반환
	return ((c->val.tv_sec * 1000) + (c->val.tv_usec / 1000)); //하나의 마이크로초로 변환
}

//출력 영역을 잠그고 매개변수로 받아온 문자열을 출력하는 함수
void	print_lock(t_config *c, int id, char *str, int force)
{
	long long	event_time;

	pthread_mutex_lock(&c->print_section); //출력영역 뮤텍스 걸기
	event_time = get_time(c) - c->start_time;
	if (c->survival || force) //철학자들이 모두 살아 있거나 강제출력모드 일 때만 출력
		printf("%lli %i %s\n", event_time, id + 1, str); 
	pthread_mutex_unlock(&c->print_section);//출력영역 뮤텍스 해제
	return ;
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
