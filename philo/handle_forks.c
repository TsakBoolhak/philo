/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_forks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 17:27:39 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 18:12:42 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "philo.h"

void	put_down_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->forklock);
	fork->status = UNLOCKED;
	pthread_mutex_unlock(&fork->forklock);
}

void	put_down_both_forks(t_philo *philo)
{
	put_down_fork(philo->own_fork);
	put_down_fork(philo->borrowed_fork);
}

int	take_fork(t_fork *fork)
{
	int	ret;

	ret = 0;
	pthread_mutex_lock(&fork->forklock);
	if (fork->status == UNLOCKED)
	{
		fork->status = LOCKED;
		ret = 1;
	}
	pthread_mutex_unlock(&fork->forklock);
	return (ret);
}

void	assign_forks(t_fork **fork1, t_fork **fork2, t_philo *philo)
{
	*fork1 = philo->own_fork;
	if (philo->id % 2)
		*fork1 = philo->borrowed_fork;
	*fork2 = philo->borrowed_fork;
	if (philo->id % 2)
		*fork1 = philo->own_fork;
}
