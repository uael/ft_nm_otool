/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/18 09:54:36 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/hash.h"

t_hasher const	g_ihash = { (void *)ft_ihash, (void *)ft_ieq };
t_hasher const	g_uhash = { (void *)ft_uhash, (void *)ft_ueq };
t_hasher const	g_lhash = { (void *)ft_lhash, (void *)ft_leq };
t_hasher const	g_ulhash = { (void *)ft_ulhash, (void *)ft_uleq };
t_hasher const	g_llhash = { (void *)ft_llhash, (void *)ft_lleq };
t_hasher const	g_ullhash = { (void *)ft_ullhash, (void *)ft_ulleq };
t_hasher const	g_szhash = { (void *)ft_szhash, (void *)ft_szeq };
t_hasher const	g_uszhash = { (void *)ft_uszhash, (void *)ft_uszeq };
t_hasher const	g_strhash = { (void *)ft_strhash, (void *)ft_streq };

inline uint32_t	ft_ihash(int i)
{
	return (IHASH(i));
}

inline uint32_t	ft_uhash(unsigned int u)
{
	return (IHASH(u));
}

inline uint32_t	ft_lhash(long l)
{
	return (LHASH(l));
}

inline uint32_t	ft_ulhash(unsigned long ul)
{
	return (LHASH(ul));
}

inline uint32_t	ft_llhash(long long int ll)
{
	return (LLHASH(ll));
}
