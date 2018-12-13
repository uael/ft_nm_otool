/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strcspn.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline size_t	ft_strcspn(const char *s1, const char *s2)
{
	size_t ret;

	ret = 0;
	while (*s1)
		if (ft_strchr(s2, *s1))
			return (ret);
		else
		{
			++s1;
			++ret;
		}
	return (ret);
}
