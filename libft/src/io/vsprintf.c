/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/vsprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>

#include "libft/io.h"

int	ft_vsprintf(char *s, char const *fmt, va_list ap)
{
	return (ft_vsnprintf(s, INT_MAX, fmt, ap));
}
