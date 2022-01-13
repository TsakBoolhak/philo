#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/time.h>

typedef enum e_config_index
{
	PHILO_NB = 0,
	TIME_TO_DIE = 1,
	TIME_TO_EAT = 2,
	TIME_TO_SLEEP = 3,
	MEALS_GOAL = 4
}t_config_index;

typedef enum e_status
{
	EATING = 0,
	THINKING = 1,
	SLEEPING = 2,
	INIT = 3,
	IDLE = 4,
	DEAD = 5
} t_status;

typedef enum e_lock
{
	LOCKED = 0,
	UNLOCKED = 1
}t_lock;

typedef struct s_fork
{
	pthread_mutex_t	forklock;
	int				status;
}t_fork;

typedef struct s_quit
{
	pthread_mutex_t	quitlock;
	int				status;
}t_quit;

typedef struct s_philo
{
	int				id;
	int				config[5];
	t_fork			*own_fork;
	t_fork			*borrowed_fork;
	unsigned long	starting_time;
	unsigned long	last_meal_time;
	int				meals;
	int				status;
	pthread_t		thread;
	t_quit			*quit;
	pthread_mutex_t	*msg;

}t_philo;

typedef struct s_sim
{
	t_philo			*philos;
	int				config[5];
	t_quit			quit;
	unsigned long	starting_time;
	pthread_mutex_t	msg;
}t_sim;

void	print_config(int *config)
{
	printf("PHILO_NB : %d\n", config[PHILO_NB]);
	printf("TIME_TO_DIE : %d\n", config[TIME_TO_DIE]);
	printf("TIME_TO_EAT : %d\n", config[TIME_TO_EAT]);
	printf("TIME_TO_SLEEP : %d\n", config[TIME_TO_SLEEP]);
	printf("MEALS_GOAL : %d\n", config[MEALS_GOAL]);
}

void	print_philo(t_philo philo)
{
	printf("Philo #%d :\n", philo.id);
	print_config(philo.config);
	printf("Own Fork Address  %p\n", philo.own_fork);
	printf("Borrowed Fork Address  %p\n", philo.borrowed_fork);
	printf("last meal time: %lu\n", philo.last_meal_time);
	printf("meals taken : %d\n", philo.meals);
	printf("status :");
	switch (philo.status)
	{
		case EATING:
			printf("eating");
			break;
		case THINKING:
			printf("thinking");
			break;
		case SLEEPING:
			printf("sleeping");
			break;
		case INIT:
			printf("intialing");
			break;
		case IDLE:
			printf("idling");
			break;
		case DEAD:
			printf("dead");
			break;
		default:
			break;
	}
	printf("\n\n");
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int	ft_isspace(int c)
{
	return ((c > 8 && c < 14) || c == ' ');
}

long long	ft_strtoll(const char *str, char **end)
{
	unsigned long long	result;
	int					i;
	int					j;
	int					sign;

	i = 0;
	j = 0;
	result = 0;
	while (ft_isspace(str[i]))
		i++;
	sign = 1 - (2 * (str[i] == '-'));
	i += (str[i] == '-' || str[i] == '+');
	while (ft_isdigit(str[i + j]))
	{
		result = result * 10 + str[i + j++] - '0';
		if (result > (unsigned long long)(LONG_MAX + (sign == -1)))
			j = 0;
		if (result > (unsigned long long)(LONG_MAX + (sign == -1)))
			break ;
	}
	if (end && !j)
		*end = (char *)str;
	else if (end)
		*end = (char *)(str + i + j);
	return (result * sign);
}

int	get_time(unsigned long *time)
{
	struct timeval tv;
	unsigned long	ret;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	ret = (unsigned long)(tv.tv_sec) * 1000;
	ret += (unsigned long)(tv.tv_usec) / 1000;
	*time = ret;
	return (0);
}

int	check_death(t_philo *philo)
{
	if (pthread_mutex_lock(&philo->quit->quitlock) != 0)
	{
		//handle lock failure
		return (1);
	}
	if (philo->quit->status != 0)
	{
		pthread_mutex_unlock(&philo->quit->quitlock);
		return (1);
	}
	if (pthread_mutex_unlock(&philo->quit->quitlock) != 0)
	{
		//handle unlock failure
		return (1);
	}
	return (0);
}

int	handle_death(t_philo *philo)
{
	unsigned long	time;

	if (check_death(philo) == 1)
		return (1);
	if (get_time(&time) != 0)
		return (-1);
	if (time - philo->last_meal_time >= (unsigned long)philo->config[TIME_TO_DIE])
	{
		if (pthread_mutex_lock(&philo->quit->quitlock) != 0)
			return (-1);
		if (pthread_mutex_lock(philo->msg) != 0)
		{
			//handle lock failure
			return (-1);
		}
		if (philo->quit->status == 0)
		{
			printf("[%lu ms]Philo #%d died\n", time - philo->starting_time, philo->id);
			philo->quit->status = philo->id;
		}
		if (pthread_mutex_unlock(philo->msg) != 0)
		{
			//handle lock failure
			return (-1);
		}
		if (pthread_mutex_unlock(&philo->quit->quitlock) != 0)
		{
			//handle unlock failure
			return (-1);
		}
		return (1);
	}
	return (0);
}

int	put_down_fork(t_fork *fork)
{
	if (pthread_mutex_lock(&fork->forklock))
		return (-1);
	fork->status = UNLOCKED;
	if (pthread_mutex_unlock(&fork->forklock))
		return (-1);
	return (0);
}

int	put_down_both_forks(t_philo *philo)
{
	if (put_down_fork(philo->own_fork))
		return (-1);
	if (put_down_fork(philo->borrowed_fork))
		return (-1);
	return (0);
}

int	take_fork(t_fork *fork)
{
	if (pthread_mutex_lock(&fork->forklock))
		return (-1);
	if (fork->status == LOCKED)
	{
		if (pthread_mutex_unlock(&fork->forklock))
			return (-1);
		return (0);
	}
	else
		fork->status = LOCKED;
	if (pthread_mutex_unlock(&fork->forklock))
		return (-1);
	return (1);
}

int	print_eat_msg(t_philo *philo)
{
	unsigned long	time;

	if (pthread_mutex_lock(philo->msg) != 0)
		return (-1);
	if (get_time(&time))
	{
		if (pthread_mutex_unlock(philo->msg) != 0)
			return (-1);
		return (-1);
	}
	if (handle_death(philo))
	{
		if (pthread_mutex_unlock(philo->msg) != 0)
			return (-1);
		return (-1);
	}
	printf("[%lu ms]Philo #%d has taken a fork\n", time - philo->starting_time, philo->id);
	printf("[%lu ms]Philo #%d has taken a fork\n", time - philo->starting_time, philo->id);
	printf("[%lu ms]Philo #%d is eating\n", time - philo->starting_time, philo->id);
	if (pthread_mutex_unlock(philo->msg) != 0)
		return (-1);
	philo->last_meal_time = time;
	philo->meals++;
	time = philo->config[TIME_TO_EAT];
	if (philo->config[TIME_TO_EAT] > philo->config[TIME_TO_DIE])
		time = philo->config[TIME_TO_DIE];
	usleep(time * 1000);
	if (put_down_both_forks(philo))
		return (-1);
	return (0);
}

int	philo_eat(t_philo *philo)
{
	int	ret;
	while (1)
	{
		if (handle_death(philo))
			return (-1);
		ret = take_fork(philo->own_fork);
		if (ret == -1)
			return (-1);
		else if (ret == 0)
		{
			usleep(500);
			continue;
		}
		ret = take_fork(philo->borrowed_fork);
		if (ret == -1)
			return (-1);
		else if (ret == 0)
		{
			if (put_down_fork(philo->own_fork))
				return (-1);
			usleep(500);
			continue;
		}
		if (print_eat_msg(philo))
			return (-1);
		if (philo->id % 2)
			usleep(10);
		break;
	}
	return (0);
}

int	philo_sleep(t_philo *philo)
{
	unsigned long	time;

	if (pthread_mutex_lock(philo->msg) != 0)
		return (-1);
	if (get_time(&time))
	{
		if (pthread_mutex_unlock(philo->msg) != 0)
			return (-1);
		return (-1);
	}
	printf("[%lu ms]Philo #%d is sleeping\n", time - philo->starting_time, philo->id);
	if (pthread_mutex_unlock(philo->msg) != 0)
		return (-1);
	time -= philo->last_meal_time;
	if ((unsigned long)(time) + (unsigned long)(philo->config[TIME_TO_SLEEP]) > (unsigned long)(philo->config[TIME_TO_DIE]))
		time = philo->config[TIME_TO_DIE] - time;
	else
		time = philo->config[TIME_TO_SLEEP];
	usleep(time * 1000);
	return (0);
}

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)(arg);
	while (1)
	{
		if (handle_death(philo))
			return (NULL);
		if (philo_eat(philo))
			return (NULL);
		if (handle_death(philo))
			return (NULL);
		if (philo_sleep(philo))
			return (NULL);
	}
}

int	init_sim(t_sim *sim, int ac, char **av)
{
	char	*end;
	int		i;

	if (pthread_mutex_init(&sim->quit.quitlock, NULL) != 0)
		return (1);
	sim->quit.status = 0;
	if (pthread_mutex_init(&sim->msg, NULL) != 0)
		return (1);
	if (ac != 5 && ac != 6)
		return (2);
	memset(sim, 0, sizeof(*sim));
	sim->config[4] = -1;
	i = 1;
	while (i < ac)
	{
		sim->config[i - 1] = ft_strtoll(av[i], &end);
		if (end == av[i] || *end != '\0' || sim->config[i - 1] < 0)
			return (3);
		i++;
	}
//	print_config(sim->config);
	sim->philos = malloc(sizeof(t_philo) * (sim->config[PHILO_NB]));
	if (sim->philos == NULL)
		return (4);
	i = 0;
	while (i < sim->config[PHILO_NB])
	{
		sim->philos[i].id = i + 1;
		sim->philos[i].quit = &sim->quit;
		sim->philos[i].msg = &sim->msg;
		sim->philos[i].config[0] = sim->config[0];
		sim->philos[i].config[1] = sim->config[1];
		sim->philos[i].config[2] = sim->config[2];
		sim->philos[i].config[3] = sim->config[3];
		sim->philos[i].config[4] = sim->config[4];
		sim->philos[i].own_fork = malloc(sizeof(t_fork));
		sim->philos[i].meals = 0;
		sim->philos[i].status = INIT;
		if (sim->philos[i].own_fork == NULL)
		{
			return (5); //handle malloc failure here
		}
		sim->philos[i].own_fork->status = UNLOCKED;
		if (pthread_mutex_init(&sim->philos[i].own_fork->forklock, NULL) != 0)
		{
			//handle mutex init failure here
			return (6);
		}
		i++;
	}
	if (get_time(&sim->starting_time) != 0)
		return (6);
	i = 0;
	while (i < sim->config[PHILO_NB])
	{
		sim->philos[i].last_meal_time = sim->starting_time;
		sim->philos[i].starting_time = sim->starting_time;
		sim->philos[i].borrowed_fork = sim->philos[(sim->philos[i].id != sim->config[PHILO_NB]) * (i + 1)].own_fork;
		if (pthread_create(&sim->philos[i].thread, NULL, &routine, &sim->philos[i]) != 0)
		{
			//handle thread creating failure here
			return (7);
		}
	//	print_philo(sim->philos[i]);
		i++;
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_sim	sim;
	int	i;
	int	ret;

	ret = init_sim(&sim, ac, av);
	if (ret != 0)
		return (ret);
	i = 0;
	while (i < sim.config[PHILO_NB])
	{
		if (pthread_join(sim.philos[i].thread, NULL) != 0)
		{
			//handle thread joining failure
			return (8);
		}
		if (pthread_mutex_destroy(&sim.philos[i].own_fork->forklock) != 0)
		{
			//handle mutex destroy failure
			return (9);
		}
		free(sim.philos[i].own_fork);
		i++;
	}
	free(sim.philos);
	if (pthread_mutex_destroy(&sim.quit.quitlock) != 0)
	{
		//handle mutex destroy failure
		return (10);
	}
	if (pthread_mutex_destroy(&sim.msg) != 0)
	{
		//handle mutex destroy failure
		return (11);
	}
	return (0);
}
