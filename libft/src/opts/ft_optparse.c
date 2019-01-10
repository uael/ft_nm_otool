/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts/ft_optparse.c                                 :+:      :+:    :+:   */
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

#include <stdbool.h>

#define UNKNOWN_OPTION (-1)
#define NO_OPT (-2)

static int	show_err(int err, char *name, char *option, size_t len)
{
	if (!err)
		return (0);
	if (err == UNKNOWN_OPTION)
		ft_fprintf(g_stderr, "%s: illegal option -- %.*s\n",
			name, (unsigned)len, option);
	else if (err == NO_OPT)
		ft_fprintf(g_stderr, "%s: argument required -- %.*s\n",
			name, (unsigned)len, option);
	return (1);
}

static int	get_value(char **c_opt, const t_opt opts[], char *optv[], int i[2])
{
	char	*opt;
	char	*value;

	opt = *c_opt + i[1];
	if (opts->type == FT_OPT_BOOLEAN)
		*(int *)opts->value ^= opts->flag;
	else if (opts->type == FT_OPT_STRING || opts->type == FT_OPT_INTEGER)
	{
		value = NULL;
		if (*opt == '=')
		{
			value = opt + 1;
			*c_opt += i[1] + ft_strlen(opt) - 1;
		}
		else if (!*opt)
			value = optv[++i[0]];
		if (!value)
			return (NO_OPT);
		if (opts->type == FT_OPT_STRING)
			*(char **)opts->value = value;
		else if (opts->type == FT_OPT_INTEGER)
			*(int *)opts->value = 0;
	}
	return (0);
}

static int	parse_short(char **c_opt, const t_opt opt[], char *optv[], int *i)
{
	int	indexes[2];
	int	err;

	while (opt->type != FT_OPT_END)
	{
		if (opt->short_name == **c_opt)
		{
			indexes[0] = *i;
			indexes[1] = 1;
			err = get_value(c_opt, opt, optv, indexes);
			*i = *indexes;
			return (show_err(err, optv[0], *c_opt - 1, 1));
		}
		opt++;
	}
	return (show_err(UNKNOWN_OPTION, optv[0], *c_opt, 1));
}

static int	parse_long(char **c_opt, const t_opt opt[], char *optv[], int *i)
{
	size_t	len;
	char	*res;
	int		indexes[2];
	int		err;

	if ((res = ft_strchr(*c_opt, '=')))
	{
		len = res - *c_opt;
	}
	else
		len = ft_strlen(*c_opt);
	while (opt->type != FT_OPT_END)
	{
		if (ft_strncmp(opt->long_name, *c_opt, len) == 0)
		{
			indexes[0] = *i;
			indexes[1] = (int)len;
			err = get_value(c_opt, opt, optv, indexes);
			*i = *indexes;
			return (show_err(err, optv[0], *c_opt - len, len));
		}
		opt++;
	}
	return (show_err(UNKNOWN_OPTION, optv[0], *c_opt, ft_strlen(*c_opt)));
}

int			ft_optparse(const t_opt opts[], int *idx, int optc, char *optv[])
{
	int		err;
	char	*opt;
	char	*s_opt;

	err = 0;
	while (*idx < optc)
	{
		opt = optv[*idx];
		s_opt = opt;
		if (opt[0] != '-' || !opt[1])
			break ;
		if (opt[1] != '-')
			while (*++s_opt)
				err |= parse_short(&s_opt, opts, optv, idx);
		else if (!opt[2])
		{
			++*idx;
			break ;
		}
		else if (s_opt += 2)
			err |= parse_long(&s_opt, opts, optv, idx);
		++*idx;
	}
	return (err ? -1 : 0);
}
