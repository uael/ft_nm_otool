/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/glob.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Long ago, in UNIX V6, there was a program /etc/glob that would expand
** wildcard patterns. Soon afterward this became a shell built-in.
**
** These days there is also a library routine glob(3) that will
** perform this function for a user program.
**
** Now we are writing a clone of this. Things got a little weird, ok?
*/

#ifndef FT_GLOB_H
# define FT_GLOB_H

# include "ft/cdefs.h"

/*
** Bits set in the FLAGS argument to `globctor'.
** FT_GLOB_ERR (1 << 0) // Return on read errors.
** FT_GLOB_MARK (1 << 1) // Append a slash to each name.
** FT_GLOB_NOSORT (1 << 2) // Don't sort the names.
** FT_GLOB_DOOFFS (1 << 3) // Insert PGLOB->gl_offs NULLs.
** FT_GLOB_NOCHECK (1 << 4) // If nothing matches, return the pattern.
** FT_GLOB_APPEND (1 << 5) // Append to results of a previous call.
** FT_GLOB_NOESCAPE (1 << 6) // Backslashes don't quote metacharacters.
** FT_GLOB_PERIOD (1 << 7) // Leading `.' can be matched by metachars.
**
** FT_GLOB_MAGCHAR (1 << 8) // Set in gl_flags if any metachars seen.
** FT_GLOB_ALTDIRFUNC (1 << 9) // Use gl_opendir et al functions.
** FT_GLOB_BRACE (1 << 10) // Expand "{a,b}" to "a" "b".
** FT_GLOB_NOMAGIC (1 << 11) // If no magic chars, return the pattern.
** FT_GLOB_TILDE (1 << 12) // Expand ~user and ~ to home directories.
** FT_GLOB_ONLYDIR (1 << 13) // Match only directories.
** FT_GLOB_TILDE_CHECK (1 << 14) // Like FT_GLOB_TILDE but return an err
**										if the user name is not available.
*/
# define FT_GLOB_ERR (1 << 0)
# define FT_GLOB_MARK (1 << 1)
# define FT_GLOB_NOSORT (1 << 2)
# define FT_GLOB_DOOFFS (1 << 3)
# define FT_GLOB_NOCHECK (1 << 4)
# define FT_GLOB_APPEND (1 << 5)
# define FT_GLOB_NOESCAPE (1 << 6)
# define FT_GLOB_PERIOD (1 << 7)

# define FT_GLOB_MAGCHAR (1 << 8)
# define FT_GLOB_NOMATCH_FLAG (1 << 9)
# define FT_GLOB_BRACE (1 << 10)
# define FT_GLOB_NOMAGIC (1 << 11)
# define FT_GLOB_TILDE (1 << 12)
# define FT_GLOB_ONLYDIR (1 << 13)
# define FT_GLOB_TILDE_CHECK (1 << 14)

/*
** Not implemented: (handled in ush)
** FT_GLOB_TILDE | FT_GLOB_TILDE_CHECK
*/
# define FLAGS_A (FT_GLOB_ERR|FT_GLOB_MARK|FT_GLOB_NOSORT|FT_GLOB_ONLYDIR)
# define FLAGS_B (FT_GLOB_DOOFFS|FT_GLOB_NOESCAPE|FT_GLOB_NOCHECK)
# define FLAGS_C (FT_GLOB_PERIOD|FT_GLOB_BRACE|FT_GLOB_NOMAGIC|FT_GLOB_APPEND)
# define __FT_GLOB_FLAGS (FLAGS_A|FLAGS_B|FLAGS_C|FT_GLOB_NOMATCH_FLAG)

/*
** FT_GLOB_SUCCESS 0 // Yay
** FT_GLOB_NOSPACE 1   // Ran out of memory.
** FT_GLOB_ABORTED 2   // Read error.
** FT_GLOB_NOMATCH 3   // No matches found.
** FT_GLOB_NOSYS 4 // Not implemented.
** FT_GLOB_ABEND FT_GLOB_ABORTED // Compat
*/
# define FT_GLOB_SUCCESS 0
# define FT_GLOB_NOSPACE 1
# define FT_GLOB_ABORTED 2
# define FT_GLOB_NOMATCH 3
# define FT_GLOB_NOSYS 4
# define FT_GLOB_ABEND FT_GLOB_ABORTED

/*
** Structure describing a globbing run.
**
** typedef struct s_glob	t_glob;
** struct					s_glob
** {
** 	size_t	gl_pathc;    // Count of paths matched so far
** 	char	**gl_pathv;  // List of matched pathnames.
** 	size_t	gl_offs;     // Slots to reserve in gl_pathv.
**  int		gl_flags;    // Set to FLAGS, maybe | FT_GLOB_MAGCHAR.
**  int     gl_padding;  // Just for struct padding
** };
*/
typedef struct s_glob	t_glob;
struct					s_glob
{
	size_t	gl_pathc;
	char	**gl_pathv;
	size_t	gl_offs;
	int		gl_flags;
	int		gl_padding;
};

/*
** The ft_glob() function searches for all the pathnames matching pattern
** according to the rules used by the shell (see glob(7)).
** No tilde expansion or parameter substitution is done;
** if you want these, use wordexp(3).
*/
int						ft_glob(const char *pattern, int flags, t_glob *pglob);

/*
** The ft_globfree() function frees the dynamically allocated storage from an
** earlier call to ft_glob().
*/
void					ft_globfree(t_glob *pglob);

#endif
