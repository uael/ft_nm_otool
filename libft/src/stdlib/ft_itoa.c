/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdlib/ft_itoa.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ctype.h"
#include "ft/stdlib.h"

#include <stdlib.h>

inline char	*ft_itoa(int nb)
{
	int		tmp;
	int		pow;
	char	*str;
	int		mod;

	tmp = nb;
	mod = nb < 0;
	pow = 1;
	while (tmp /= 10)
		pow++;
	str = (char *)malloc((size_t)(pow + mod + 1));
	str[pow + mod] = 0;
	if (mod)
		*str++ = '-';
	else
		nb = -nb;
	while (pow--)
	{
		str[pow] = (char)(-(nb % 10) + '0');
		nb /= 10;
	}
	return (str - mod);
}
