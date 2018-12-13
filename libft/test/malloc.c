/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test/malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft/malloc.h>
#include <ft/stdio.h>
#include <assert.h>

static int test(t_mpool pool)
{
	void *ptrs[1024], *ptr1 = NULL, *ptr2 = NULL, *begin = NULL;

	for (size_t i = 0; i < 128; ++i) {
		ptrs[i] = ft_palloc(pool, i);
		ft_printf("%p\n", ptrs[i]);
		if (!begin) begin = ptrs[i];
	}

	ptr1 = ft_palloc(pool, 4000);
	ft_printf("%p\n", ptr1);
	ptr2 = ft_palloc(pool, 4000);
	ft_printf("%p\n", ptr2);

	ft_pfree(pool, ptr2);
	ft_pfree(pool, ptr1);

	for (size_t i = 0; i < 128; ++i)
		ft_pfree(pool, ptrs[i]);

	return 0;
}

int main(void)
{
	static uint8_t mem[UINT16_MAX];
	t_mpool pool;

	ft_mpool(g_stack_def,
			 &(struct s_stack_conf){ .mem = mem, .size = sizeof(mem) },
			 &pool);

	assert(0 == test(pool));

	ft_mpool(g_heap_def, NULL, &pool);

	assert(0 == test(pool));

	return 0;
}
