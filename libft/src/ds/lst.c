/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ds/lst.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/07 09:53:34 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ds.h"

void	ft_lstctor(t_lst *lst)
{
	ft_bzero(lst, sizeof(t_lst));
	lst->tail = (struct s_node *)lst;
	lst->head = (struct s_node *)lst;
}

t_node	*ft_lstpush(t_lst *lst, t_node *node)
{
	++lst->len;
	return (ft_nodelput((t_node *)lst, node));
}

t_node	*ft_lstusht(t_lst *lst, t_node *node)
{
	++lst->len;
	return (ft_noderput((t_node *)lst, node));
}

t_node	*ft_lstpop(t_lst *lst)
{
	if (!lst->len)
		return (NULL);
	return (ft_nodeulink(lst->tail, lst->tail->prev, lst->tail->next));
}

t_node	*ft_lstsht(t_lst *lst)
{
	if (!lst->len)
		return (NULL);
	return (ft_nodeulink(lst->head, lst->head->prev, lst->head->next));
}
