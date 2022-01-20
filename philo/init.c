/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:30:28 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 17:45:53 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <limits.h>
#include <stdlib.h>

int	init_sim_first_step(t_sim *sim, int ac, char **av)
{
	int		i;
	long	value;
	char	*end;

	if (ac != 5 && ac != 6)
		return (1);
	sim->config[4] = -1;
	i = 1;
	while (i < ac)
	{
		value = ft_strtoll(av[i], &end);
		if (end == av[i] || *end != '\0' || value < 0 || value > INT_MAX)
			return (3 + i);
		sim->config[i - 1] = ft_strtoll(av[i], &end);
		i++;
	}
	if (pthread_mutex_init(&sim->quit.quitlock, NULL) != 0)
		return (2);
	sim->quit.status = 0;
	if (pthread_mutex_init(&sim->msg, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->quit.quitlock);
		return (3);
	}
	return (0);
}

int	init_sim_second_step(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config[PHILO_NB])
	{
		sim->philos[i].own_fork = malloc(sizeof(t_fork));
		if (sim->philos[i].own_fork == NULL)
		{
			free_sim(sim, i - 1, 0);
			return (11);
		}
		if (pthread_mutex_init(&sim->philos[i].own_fork->forklock, NULL) != 0)
		{
			free_sim(sim, i, 0);
			return (12);
		}
		sim->philos[i].own_fork->status = UNLOCKED;
		sim->philos[i].id = i + 1;
		sim->philos[i].quit = &sim->quit;
		sim->philos[i].msg = &sim->msg;
		ft_memcpy(sim->philos[i].config, sim->config, sizeof(int) * 5);
		sim->philos[i].meals = 0;
		i++;
	}
	return (0);
}

int	init_sim_last_step(t_sim *sim)
{
	int			i;
	int			index;
	int			ret;
	pthread_t	*thread;

	if (get_time(&sim->starting_time) != 0)
		return (13);
	i = 0;
	while (i < sim->config[PHILO_NB])
	{
		index = (sim->philos[i].id != sim->config[PHILO_NB]) * (i + 1);
		thread = &sim->philos[i].thread;
		sim->philos[i].last_meal_time = sim->starting_time;
		sim->philos[i].starting_time = sim->starting_time;
		sim->philos[i].borrowed_fork = sim->philos[index].own_fork;
		ret = pthread_create(thread, NULL, &routine, &sim->philos[i]);
		if (ret != 0)
		{
			free_sim(sim, sim->config[PHILO_NB], i -1);
			return (14);
		}
		i++;
	}
	return (0);
}

int	init_sim(t_sim *sim, int ac, char **av)
{
	int	ret;

	ft_memset(sim, 0, sizeof(*sim));
	ret = init_sim_first_step(sim, ac, av);
	if (ret)
		return (ret);
	sim->philos = malloc(sizeof(t_philo) * ((sim->config[PHILO_NB])));
	if (sim->philos == NULL)
		return (10);
	ft_memset(sim->philos, 0, sizeof(t_philo) * ((sim->config[PHILO_NB])));
	ret = init_sim_second_step(sim);
	if (ret)
		return (ret);
	ret = init_sim_last_step(sim);
	return (ret);
}
