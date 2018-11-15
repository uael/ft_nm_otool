/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ds/nodelink.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/07 09:53:34 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ds.h"

inline t_node	*ft_nodelink(t_node *node, t_node *prev, t_node *next)
{
	node->next = prev->next;
	node->prev = next->prev;
	prev->next = node;
	next->prev = node;
	return (node);
}
