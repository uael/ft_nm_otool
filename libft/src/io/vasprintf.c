/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/vasprintf.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft/io.h"

int	ft_vasprintf(char **s, char const *fmt, va_list ap)
{
	int		ret;
	va_list	ap2;

	va_copy(ap2, ap);
	ret = ft_vsnprintf(NULL, 0, fmt, ap2);
	va_end(ap2);
	if (ret < 0)
		return (-1);
	return (ft_vsnprintf(*s = ft_malloc(ret + 1U), ret + 1U, fmt, ap));
}
