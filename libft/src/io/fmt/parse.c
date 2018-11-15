/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt/parse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <limits.h>

#include "../fmt.h"

static int	atoio(char **s)
{
	int i;

	i = 0;
	while (ft_isdigit(**s))
	{
		if ((size_t)i > INT_MAX / 10U || (**s - '0') > INT_MAX - 10 * i)
			i = -1;
		else
			i = 10 * i + (**s - '0');
		++*s;
	}
	return (i);
}

static void	parsep(t_fmt *f, char **sp, va_list ap)
{
	if (**sp == '.')
	{
		if (*(*sp + 1) == '*')
		{
			f->p = va_arg(ap, int);
			*sp += 2;
			f->xp = f->p >= 0;
		}
		else
		{
			++*sp;
			f->p = atoio(sp);
			f->xp = 1;
		}
	}
	else
		f->p = -1;
}

int			iofmt_parse(t_fmt *f, char **sp, va_list ap)
{
	ft_bzero(f, sizeof(t_fmt));
	while (**sp && (**sp - ' ') < 32 && (FLAGMASK & (1U << (**sp - ' '))))
		f->fl |= 1U << (*(*sp)++ - ' ');
	if ((f->w = atoio(sp)) < 0)
		return (-1);
	if (**sp == '*')
	{
		if ((f->w = (int16_t)va_arg(ap, int)) < 0)
		{
			f->fl |= LEFT_ADJ;
			f->w = -f->w;
		}
		++*sp;
		if (ft_isdigit(**sp) && (f->w = atoio(sp)) < 0)
			return (-1);
	}
	parsep(f, sp, ap);
	return (0);
}
