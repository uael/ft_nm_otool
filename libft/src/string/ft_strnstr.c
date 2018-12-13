/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strnstr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	const char	*o_needle = needle;

	if (!*needle)
		return ((char *)haystack);
	while (len-- && *haystack && *needle)
		if (*haystack++ == *needle)
			needle++;
		else
		{
			haystack -= needle - o_needle;
			len += needle - o_needle;
			needle = o_needle;
		}
	return (*needle ? 0 : (char *)(haystack - (needle - o_needle)));
}
