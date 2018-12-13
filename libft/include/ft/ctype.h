/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/ctype.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CTYPE_H
# define FT_CTYPE_H

# include "ft/cdefs.h"

extern int	ft_isalnum(int c);
extern int	ft_isascii(int c);
extern int	ft_isalpha(int c);
extern int	ft_isdigit(int c);
extern int	ft_islower(int c);
extern int	ft_isprint(int c);
extern int	ft_isupper(int c);
extern int	ft_isspace(int c);
extern int	ft_iscntrl(int c);
extern int	ft_tolower(int c);
extern int	ft_toupper(int c);

#endif
