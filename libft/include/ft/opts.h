/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/opts.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_OPTS_H
# define FT_OPTS_H

enum			e_ft_opt {
	FT_OPT_END = 0,
	FT_OPT_BOOLEAN,
	FT_OPT_STRING,
	FT_OPT_INTEGER
};

typedef struct	s_opt {
	enum e_ft_opt	type;
	const char		short_name;
	const char		*long_name;
	void			*value;
	const char		*help;
	int				flag;
}				t_opt;

int				ft_optparse(const t_opt opts[], int *idx, int ac, char *av[]);
int				ft_optusage(const t_opt opts[], char *name, char *u, char *dsc);

#endif
