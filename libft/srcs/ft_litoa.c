/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_litoa.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvivaudo <fvivaudo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/11/05 18:13:15 by fvivaudo          #+#    #+#             */
/*   Updated: 2014/11/09 13:23:44 by fvivaudo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdlib.h>
#include <limits.h>

void	ft_litoa2(char *res, int *nb_char, long int *n, char puis)
{
	if ((unsigned long int)(*n) > LONG_MAX)
	{
		*res = '-';
		++(*res);
		++(*nb_char);
		(*n) = (*n) * (-1);
	}
	while (puis != 1)
	{
		*res = (unsigned long int)(*n) / puis + '0';
		++res;
		(*n) = (unsigned long int)(*n) % puis;
		puis = puis / 10;
	}
}

char	*ft_litoa(long int n)
{
	long int		puis;
	int				nb_char;
	char			*res;

	nb_char = 0;
	puis = 1;
	while ((n / puis) > 9 || (n / puis) < -9)
	{
		++nb_char;
		puis = puis * 10;
	}
	res = malloc(nb_char + 2);
	if (res == NULL)
		return (NULL);
	ft_litoa2(res, &nb_char, &n, puis);
	*res = n + '0';
	++res;
	*res = '\0';
	return (res - nb_char - 1);
}
