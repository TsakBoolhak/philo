/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_and_routine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 18:15:25 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 18:22:31 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)(arg);
	if (philo->id % 2)
		usleep(10000);
	while (1)
	{
		if (philo_eat(philo))
			return (NULL);
		if (philo_sleep(philo))
			return (NULL);
		if (philo_think(philo))
			return (NULL);
	}
}

void	join_philos(t_philo *philos, int nb_to_join)
{
	int	i;

	i = 0;
	while (i < nb_to_join)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
}

void	free_sim(t_sim *sim, int mtx_dstroy, int nb_to_join)
{
	int	i;

	i = 0;
	join_philos(sim->philos, nb_to_join);
	while (i < sim->config[PHILO_NB])
	{
		if (sim->philos[i].own_fork && i <= mtx_dstroy)
			pthread_mutex_destroy(&sim->philos[i].own_fork->forklock);
		free(sim->philos[i].own_fork);
		i++;
	}
	pthread_mutex_destroy(&sim->quit.quitlock);
	pthread_mutex_destroy(&sim->msg);
	free(sim->philos);
}
