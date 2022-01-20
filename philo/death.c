/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   death.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:34:14 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 17:36:43 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "philo.h"
#include <stdio.h>

int	check_death(t_philo *philo)
{
	int	ret;

	ret = 0;
	pthread_mutex_lock(&philo->quit->quitlock);
	if (philo->quit->status != 0)
		ret = 1;
	pthread_mutex_unlock(&philo->quit->quitlock);
	return (ret);
}

int	handle_death(t_philo *philo)
{
	unsigned long	time;
	int				ret;

	ret = 0;
	pthread_mutex_lock(&philo->quit->quitlock);
	ret = get_time(&time);
	time -= philo->last_meal_time;
	if (ret == -1)
		;
	else if (philo->quit->status > 0)
		ret = 1;
	else if (philo->quit->status == -1 * philo->config[PHILO_NB])
		ret = 3;
	else if (time >= (unsigned long)philo->config[TIME_TO_DIE])
	{
		if (philo->quit->status <= 0)
			philo->quit->status = philo->id;
		ret = 2;
	}
	pthread_mutex_unlock(&philo->quit->quitlock);
	return (ret);
}

int	check_end(t_philo *philo)
{
	unsigned long	time;
	int				ret;

	if (get_time(&time))
		return (-1);
	time -= philo->starting_time;
	ret = handle_death(philo);
	if (ret == 2)
	{
		pthread_mutex_lock(philo->msg);
		printf("[%lu ms]Philo #%d has died\n", time, philo->id);
		pthread_mutex_unlock(philo->msg);
	}
	return (ret);
}
