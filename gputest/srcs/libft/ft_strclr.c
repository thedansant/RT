/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strclr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: momartin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/05 06:56:20 by momartin          #+#    #+#             */
/*   Updated: 2016/11/09 10:42:37 by momartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_strclr(char *s)
{
	size_t l;
	size_t i;

	if (!s)
		return ;
	l = ft_strlen(s);
	i = 0;
	while (i < l)
	{
		s[i] = '\0';
		i++;
	}
	s[i] = '\0';
}
