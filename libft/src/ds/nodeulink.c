/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ds/nodeulink.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/07 09:53:34 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ds.h"

inline t_node	*ft_nodeulink(t_node *node, t_node *prev, t_node *next)
{
	next->prev = prev;
	prev->next = next;
	node->prev = node;
	node->next = node;
	return (node);
}
