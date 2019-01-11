/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OFILE_H
# define OFILE_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

# include <ar.h>
# include <mach-o/arch.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <mach-o/ranlib.h>
# include <mach-o/stab.h>
# include <mach-o/swap.h>

/*
** Public API
*/

# define AR_CIGAM (0x72613C21)
# define AR_MAGIC (0x213C6172)

# define OFILE_NX_HOST (NXArchInfo const *)(-1)
# define OFILE_NX_ALL (NXArchInfo const *)(NULL)

enum					e_ofile
{
	OFILE_MH = 0,
	OFILE_FAT,
	OFILE_AR,
};

typedef struct s_obj	const *t_obj;

struct					s_obj
{
	enum e_ofile		ofile;
	NXArchInfo const	*target;
	uint8_t const		*buf;
	size_t				size;
	char const			*name;
	size_t				name_len;
	bool				m64;
	bool				le;
};

enum					e_ofile_e
{
	OFILE_E_INVAL_MAGIC = 1,
	OFILE_E_INVAL_FATHDR,
	OFILE_E_INVAL_FATARCH,
	OFILE_E_INVAL_ARCHINFO,
	OFILE_E_INVAL_ARCHOBJ,
	OFILE_E_INVAL_MHHDR,
	OFILE_E_INVAL_ARHDR,
	OFILE_E_NO_ARHDR,
	OFILE_E_INVAL_AROBJHDR,
	OFILE_E_INVAL_LC,
	OFILE_E_NOTFOUND_ARCH,
	OFILE_E_MAX
};

char const				*ofile_etoa(int err);
uint16_t				obj_swap16(t_obj obj, uint16_t u);
uint32_t				obj_swap32(t_obj obj, uint32_t u);
uint64_t				obj_swap64(t_obj obj, uint64_t u);
const void				*obj_peek(t_obj obj, size_t off, size_t len);

typedef int				(t_ofile_collector)(t_obj obj, size_t off, void *user);

struct					s_ofile_collector
{
	void						(*load)(t_obj obj, NXArchInfo const *arch_info,
									void *user);
	size_t						ncollector;
	t_ofile_collector *const	collectors[];
};

int						ofile_collect(const char *filename,
							NXArchInfo const *target,
							const struct s_ofile_collector *collector,
							void *user);

/*
** Private API
*/

struct					s_ar_info
{
	char const			*name;
	size_t				name_len;

	uint8_t const		*obj;
	size_t				size;
};

struct					s_loader
{
	uint32_t const		magic;
	bool const			m64;
	bool const			le;
	int					(*const load)(struct s_obj const *,
							struct s_ofile_collector const *, void *);
};

int						load(struct s_obj *obj,
							struct s_ofile_collector const *collector,
							void *user);
int						mh_load(struct s_obj const *obj,
							struct s_ofile_collector const *collector,
							void *user);
int						fat_load(struct s_obj const *obj,
							struct s_ofile_collector const *collector,
							void *user);
int						ar_load(struct s_obj const *obj,
							struct s_ofile_collector const *collector,
							void *user);

#endif
