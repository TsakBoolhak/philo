/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:37:46 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 17:40:01 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "philo.h"

void	print_eat_msg(int ret, t_philo *philo, unsigned long time)
{
	if (ret == 2)
		printf("[%lu ms]Philo #%d has died\n", time, philo->id);
	else if (ret == 0)
	{
		printf("[%lu ms]Philo #%d has taken a fork\n", time, philo->id);
		printf("[%lu ms]Philo #%d has taken a fork\n", time, philo->id);
		printf("[%lu ms]Philo #%d is eating\n", time, philo->id);
	}
}

int	eat_action(t_philo *philo)
{
	unsigned long	time;
	int				ret;

	if (get_time(&time))
		return (-1);
	pthread_mutex_lock(philo->msg);
	ret = handle_death(philo);
	print_eat_msg(ret, philo, time - philo->starting_time);
	pthread_mutex_unlock(philo->msg);
	if (ret != 0)
		return (1);
	philo->last_meal_time = time;
	philo->meals++;
	if (philo->meals == philo->config[MEALS_GOAL])
	{
		pthread_mutex_lock(&philo->quit->quitlock);
		philo->quit->status--;
		pthread_mutex_unlock(&philo->quit->quitlock);
	}
	time = philo->config[TIME_TO_EAT];
	if (philo->config[TIME_TO_EAT] > philo->config[TIME_TO_DIE])
		time = philo->config[TIME_TO_DIE];
	usleep(time * 1000);
	return (0);
}

int	eat_cycle(t_philo *philo, t_fork *fork1, t_fork *fork2)
{
	if (check_end(philo))
		return (-1);
	if (take_fork(fork1) == 0)
	{
		usleep(500);
		return (1);
	}
	if (take_fork(fork2) == 0)
	{
		put_down_fork(fork1);
		usleep(500);
		return (1);
	}
	if (eat_action(philo))
	{
		put_down_both_forks(philo);
		return (-1);
	}
	put_down_both_forks(philo);
	return (0);
}

int	philo_eat(t_philo *philo)
{
	t_fork	*fork1;
	t_fork	*fork2;
	int		ret;

	if (check_end(philo))
		return (-1);
	assign_forks(&fork1, &fork2, philo);
	if (check_end(philo))
		return (-1);
	ret = eat_cycle(philo, fork1, fork2);
	while (ret == 1)
	{
		ret = eat_cycle(philo, fork1, fork2);
	}
	return (ret);
}
