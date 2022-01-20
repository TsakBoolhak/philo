/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 18:23:30 by acabiac           #+#    #+#             */
/*   Updated: 2022/01/20 18:24:03 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int ac, char **av)
{
	t_sim	sim;
	int		ret;

	ret = init_sim(&sim, ac, av);
	if (ret != 0)
		return (ret);
	free_sim(&sim, sim.config[PHILO_NB], sim.config[PHILO_NB]);
	return (0);
}
