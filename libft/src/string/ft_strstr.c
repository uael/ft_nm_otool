/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strstr.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strstr(char const *haystack, char const *needle)
{
	const char	*o_needle = needle;

	if (!*needle)
		return ((char *)haystack);
	while (*haystack && *needle)
		if (*haystack++ == *needle)
			needle++;
		else
		{
			haystack -= needle - o_needle;
			needle = o_needle;
		}
	return (*needle ? 0 : (char *)(haystack - (needle - o_needle)));
}
