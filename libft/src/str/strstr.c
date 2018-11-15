/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline char	*ft_strstr(char const *haystack, char const *needle)
{
	char const *beg;
	char const *ptr;

	if (!*needle)
		return ((char *)haystack);
	while (*haystack)
	{
		if (*haystack == *needle)
		{
			beg = haystack + 1;
			ptr = needle + 1;
			while (*ptr == *beg && *ptr)
			{
				++ptr;
				++beg;
			}
			if (!*ptr)
				return ((char *)haystack);
		}
		++haystack;
	}
	return (NULL);
}
