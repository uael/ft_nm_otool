/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts/ft_optusage.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/opts.h"
#include "ft/string.h"
#include "ft/stdio.h"

int	ft_optusage(const t_opt opts[], char *name, char *usage, char *desc)
{
	ft_printf("Usage: %s [option(s)]... %s\n %s\n The options are:\n",
		name, usage, desc);
	while (opts->type != FT_OPT_END)
	{
		if (opts->short_name && opts->long_name)
			ft_printf("  -%c, --%-16s ", opts->short_name, opts->long_name);
		else if (opts->short_name)
			ft_printf("  -%-21c ", opts->short_name);
		else if (opts->long_name)
			ft_printf("     --%-16s ", opts->long_name);
		ft_printf("%s\n", opts->help);
		opts++;
	}
	return (0);
}
