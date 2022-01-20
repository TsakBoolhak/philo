/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:08:37 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 17:20:06 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stddef.h>

typedef enum e_config_index
{
	PHILO_NB = 0,
	TIME_TO_DIE = 1,
	TIME_TO_EAT = 2,
	TIME_TO_SLEEP = 3,
	MEALS_GOAL = 4
}t_config_index;

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

void		*ft_memset(void *b, int c, size_t len);
void		*ft_memcpy(void *dst, const void *src, size_t n);
int			ft_isdigit(int c);
int			ft_isspace(int c);
long long	ft_strtoll(const char *str, char **end);
int			get_time(unsigned long *time);
int			check_death(t_philo *philo);
int			handle_death(t_philo *philo);
void		put_down_fork(t_fork *fork);
void		put_down_both_forks(t_philo *philo);
int			take_fork(t_fork *fork);
void		print_eat_msg(int ret, t_philo *philo, unsigned long time);
int			eat_action(t_philo *philo);
void		assign_forks(t_fork **fork1, t_fork **fork2, t_philo *philo);
int			check_end(t_philo *philo);
int			eat_cycle(t_philo *philo, t_fork *fork1, t_fork *fork2);
int			philo_eat(t_philo *philo);
int			philo_sleep(t_philo *philo);
int			philo_think(t_philo *philo);
void		*routine(void *arg);
int			init_sim_first_step(t_sim *sim, int ac, char **av);
int			init_sim_second_step(t_sim *sim);
int			init_sim_last_step(t_sim *sim);
void		join_philos(t_philo *philos, int nb_to_join);
void		free_sim(t_sim *sim, int mtx_dstroy, int nb_to_join);
int			init_sim(t_sim *sim, int ac, char **av);

#endif
