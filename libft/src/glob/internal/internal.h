/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/internal.h                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOB_INTERNAL_INTERNAL_H
# define GLOB_INTERNAL_INTERNAL_H

# include "ft/glob.h"
# include "ft/malloc.h"
# include "ft/string.h"
# include "ft/stdlib.h"

# include <dirent.h>
# include <limits.h>
# include <stdbool.h>

# include <sys/types.h>

# ifndef PATH_MAX
#  ifdef __MACOSX__
#   include <sys/syslimits.h>
#  endif
#  ifdef __linux__
#   include <linux/limits.h>
#  endif
# endif

# define IS_DIR(d) ((d)->d_type == DT_DIR || (d)->d_type == DT_LNK)

# define FT_GLOB_BOOM_BABY 5
# define FT_GLOB_NOBODY_GIVES_A_DAMN 6

# define MAX_DEPTH			(1 << 8)

typedef unsigned char	t_byte;

/*
** Structure describing a glob match.
*/
typedef struct s_match		t_match;
struct						s_match
{
	t_match		*next;
	t_byte		buf[sizeof(void *)];
};

/*
** Structure describing a glob tree climbing environment.
*/
typedef struct s_glob_env	t_glob_env;
struct						s_glob_env
{
	t_match		*match_list;
	char const	*pattern;
	char		sub_pat_buf[NAME_MAX + 1];
	char		magic_buf[NAME_MAX + 1];
	int			*flags;
};

/*
** in glob_is_magic.c:
**
** Check if a pattern contains any magic char, otherwise return NULL.
** If a magic char is found, a pointer to the last non-magic (muggle?)
** directory is returned.
**
** This function is actually here to avoid calling glob_match() too much
**
** @pat: Shell-style pattern to match, e.g. "*.[ch]".
** @flags: cf ft_glob.h (FT_GLOB_NOESCAPE)
*/
char const					*is_magic(char *magic_buf, char const *pattern, \
								int *flags);
char const					*is_there_a_closing_bracket(char const *pattern, \
													int flags, char c, char o);

/*
** in glob_list.c:
*/
t_match						*matchctor(char const *path, size_t len, int esc);
void						matchdtor(t_match *match);
int							matchctoradd(char const *path, int escape, \
										int sort, t_match **match_list);
size_t						list_len(t_match *match_list);

/*
** in glob_dir.c:
*/
int							glob_count_depth(char const *pattern);
int							glob_open_dir(DIR **dir, char const *dir_name, \
										int flags);
int							glob_close_dir(DIR *dir, int flags);
void						glob_init_dir_name(char const **dirname, \
									char *path_buf, char const *pattern);
int							glob_actually_open_some_folder(t_glob_env *e, \
													char const *dirname, \
													int depth, DIR **dir);

/*
** in glob_path.c:
*/
bool						glob_get_sub_pattern(char *sub_pat_buf, \
											char const *pattern, \
											int depth, int flags);
char const					*glob_get_folder_name(char const *path);
int							glob_append_file_name(char *path_buf, \
												char const *new_file);
int							glob_store_dir_name(char *path_buf, \
											char const *prev_dir, \
											char const *new_dir);
void						remove_last_dir_from_path(char *path_buf);

/*
** in glob_sanitize.c
*/
char						*glob_sanitize_pattern(char *dst, const char *src);

/*
**  in glob_show.c
*/
int							show_hidden_files(int flags, char pat_start);
int							show_files(int *flags, char const *pattern);

/*
** in glob_finder.c
*/
char const					*glob_find_comma(char const *pat, \
											char const *pat_end);
char const					*glob_find_opening_brace(char const *pat, \
													char const *pat_start);
void						sprglbcp(char *dst, char const *path);

/*
** Perform shell-style glob matching, returning true (1) if the match
** succeeds, or false (0) if it fails.
**
** @pat: Shell-style pattern to match, e.g. "*.[ch]".
** @str: String to match.	The pattern must match the entire string.
** @flags: cf ft_glob.h (FT_GLOB_NOESCAPE | FT_GLOB_PERIOD | FT_GLOB_BRACE)
*/
int							glob_match(char const *pat, char const *str, \
									int flags);

/*
** Recursively check PAT on the matching directory tree
**
** @pat: Shell-style pattern to match, e.g. "*.[ch]".
*/
int							glob_climb_tree(t_glob_env *glob_env);

/*
** TODO: doc
*/
int							glob_read_dir(t_glob_env *glob_env, int depth, \
										char const *dir_name);

/*
** in glob_brace.c
*/
int							glob_brace(t_glob_env *e);

#endif
