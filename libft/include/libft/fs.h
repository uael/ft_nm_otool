/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft/fs.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/12/05 15:47:15 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_FS_H
# define LIBFT_FS_H

# include "io.h"

# define CLR_RESET "\033[0m"
# define CLR_RED "\033[31m"
# define CLR_YELLOW "\033[33m"
# define CLR_CYAN "\033[36m"
# define CLR_BOLD "\033[1m"
# define CLR_BRED CLR_BOLD"\033[31m"
# define CLR_BYELLOW CLR_BOLD"\033[33m"
# define CLR_BCYAN CLR_BOLD"\033[36m"

extern char	const	*ft_basename(char const *path);
extern t_bool		ft_isdots(char const *path);
extern char			*ft_pathjoin(char const *p1, char const *p2);
extern char			*ft_pathcat(char *dst, char const *src);

#endif
