/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/stdlib.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_STDLIB_H
# define FT_STDLIB_H

# include "ft/cdefs.h"

typedef int			t_ncmp(const void *a, const void *b, size_t n);

extern int			ft_abs(int a);
extern int			ft_atoi(const char *s);
extern long			ft_atol(const char *s);
extern long long	ft_atoll(const char *s);
extern char			*ft_itoa(int nb);
extern int			ft_wctomb(char *s, wchar_t wc);
extern void			ft_shsort(void *a, size_t len, size_t esz, t_ncmp *cmp);
extern char			*ft_strerror(int eno);

#endif
