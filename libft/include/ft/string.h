/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/string.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_STRING_H
# define FT_STRING_H

# include "ft/cdefs.h"

extern void		*ft_memccpy(void *dst, const void *src, int c, size_t n);
extern void		*ft_memchr(const void *s, int c, size_t n);
extern int		ft_memcmp(const void *s1, const void *s2, size_t n);
extern void		*ft_memcpy(void *dst, const void *src, size_t n);
extern void		*ft_memdup(void const *src, size_t n);
extern void		*ft_memmove(void *dst, const void *src, size_t len);
extern void		*ft_memrcpy(void *dst, void const *src, size_t n);
extern void		*ft_memset(void *s, int c, size_t n);

extern char		*ft_stpcpy(char *dest, char const *src);
extern char		*ft_strbstr(char const *s1, char const *s2);
extern char		*ft_strcat(char *dest, char const *src);
extern char		*ft_strchr(char const *s, int c);
extern int		ft_strcmp(char const *s1, char const *s2);
extern char		*ft_strcpy(char *dest, char const *src);
extern size_t	ft_strcspn(const char *s, const char *reject);
extern char		*ft_strdup(char const *src);
extern char		*ft_strestr(char const *s1, char const *s2);
extern size_t	ft_strlcat(char *dest, char const *src, size_t size);
extern size_t	ft_strlcpy(char *dst, char const *src, size_t size);
extern size_t	ft_strlen(char const *str);
extern char		*ft_strmchr(char const *s, char const *c);
extern char		*ft_strncat(char *dest, char const *src, size_t nb);
extern char		*ft_strnchr(char const *s, int c, size_t n);
extern int		ft_strncmp(char const *s1, char const *s2, size_t n);
extern char		*ft_strncpy(char *dest, char const *src, size_t n);
extern char		*ft_strndup(char const *src, size_t n);
extern size_t	ft_strnlen(char const *str, size_t size);
extern char		*ft_strnstr(char const *str, char const *f, size_t l);
extern char		*ft_strrchr(char const *s, int c);
extern char		*ft_strscpy(char **dst, size_t *n, char *src);
extern size_t	ft_strspn(const char *s, const char *accept);
extern char		*ft_strstr(char const *haystack, char const *needle);

#endif
