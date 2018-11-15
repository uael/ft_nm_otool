/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ds/nodeswap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/07 09:53:34 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ds.h"

t_node	*ft_nodenswp(t_node *node)
{
	t_node *next;

	next = node->next;
	node->next = next->next;
	next->next = node;
	next->prev = node->prev;
	node->prev = next;
	next->prev->next = next;
	node->next->prev = node;
	return (node);
}
