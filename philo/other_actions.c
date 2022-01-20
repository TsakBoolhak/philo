/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   other_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:41:54 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 17:43:57 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int	philo_sleep(t_philo *philo)
{
	unsigned long	time;
	unsigned long	timestamp;
	int				ret;

	if (get_time(&time))
		return (-1);
	timestamp = time - philo->starting_time;
	pthread_mutex_lock(philo->msg);
	ret = handle_death(philo);
	if (ret == 2)
		printf("[%lu ms]Philo #%d has died\n", timestamp, philo->id);
	else if (ret == 0)
		printf("[%lu ms]Philo #%d is sleeping\n", timestamp, philo->id);
	pthread_mutex_unlock(philo->msg);
	if (ret)
		return (1);
	time -= philo->last_meal_time;
	time += (unsigned long)(philo->config[TIME_TO_SLEEP]);
	if (time > (unsigned long)(philo->config[TIME_TO_DIE]))
		time = philo->config[TIME_TO_DIE] - time;
	else
		time = philo->config[TIME_TO_SLEEP];
	usleep(time * 1000);
	return (0);
}

int	philo_think(t_philo *philo)
{
	unsigned long	time;
	int				ret;

	if (get_time(&time))
		return (-1);
	time -= philo->starting_time;
	pthread_mutex_lock(philo->msg);
	ret = handle_death(philo);
	if (ret == 2)
		printf("[%lu ms]Philo #%d has died\n", time, philo->id);
	else if (ret == 0)
	{
		printf("[%lu ms]Philo #%d is thinking\n", time, philo->id);
	}
	pthread_mutex_unlock(philo->msg);
	if (ret)
		return (1);
	else
		usleep(1000);
	return (0);
}

int	get_time(unsigned long *time)
{
	struct timeval	tv;
	unsigned long	ret;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	ret = 1000 * (unsigned long)(tv.tv_sec);
	ret += (unsigned long)(tv.tv_usec) / 1000;
	*time = ret;
	return (0);
}
