/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft/ds/lst.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/12/06 08:13:57 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_DS_LST_H
# define LIBFT_DS_LST_H

# include "../ex.h"
# include "../mem.h"
# include "../str.h"
# include "../math.h"

struct s_node;

typedef struct		s_node
{
	struct s_node	*prev;
	struct s_node	*next;
}					t_node;

typedef struct		s_lst
{
	struct s_node	*tail;
	struct s_node	*head;
	size_t			len;
}					t_lst;

extern t_node		*ft_nodelink(t_node *node, t_node *prev, t_node *next);
extern t_node		*ft_nodeulink(t_node *node, t_node *prev, t_node *next);
extern t_node		*ft_noderput(t_node *node, t_node *new);
extern t_node		*ft_nodelput(t_node *node, t_node *new);
extern t_node		*ft_nodenswp(t_node *anode);
extern void			ft_lstctor(t_lst *lst);
extern t_node		*ft_lstpush(t_lst *lst, t_node *node);
extern t_node		*ft_lstusht(t_lst *lst, t_node *node);
extern t_node		*ft_lstpop(t_lst *lst);
extern t_node		*ft_lstsht(t_lst *lst);

#endif
