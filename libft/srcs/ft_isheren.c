/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isheren.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvivaudo <fvivaudo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/01/09 10:42:08 by fvivaudo          #+#    #+#             */
/*   Updated: 2015/01/09 10:42:09 by fvivaudo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int		ft_isheren(const char *str, char c, int n)
{
	int i;

	i = 0;
	while (str[i] != '\0' && n > 0)
	{
		if (str[i] == c)
			return (i);
		++i;
		--n;
	}
	return (-1);
}