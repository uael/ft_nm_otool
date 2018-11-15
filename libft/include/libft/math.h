/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft/math.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/15 18:54:24 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_MATH_H
# define LIBFT_MATH_H

# include "tys.h"

typedef t_bool	(t_eqfn)(void *a, void *b);

extern t_bool	ft_ispow2(ssize_t n);

extern int8_t	ft_i8max(int8_t a, int8_t b);
extern int16_t	ft_i16max(int16_t a, int16_t b);
extern int32_t	ft_i32max(int32_t a, int32_t b);
extern int64_t	ft_i64max(int64_t a, int64_t b);
extern ssize_t	ft_imax(ssize_t a, ssize_t b);

extern uint8_t	ft_u8max(uint8_t a, uint8_t b);
extern uint16_t	ft_u16max(uint16_t a, uint16_t b);
extern uint32_t	ft_u32max(uint32_t a, uint32_t b);
extern uint64_t	ft_u64max(uint64_t a, uint64_t b);
extern size_t	ft_umax(size_t a, size_t b);

extern int8_t	ft_i8min(int8_t a, int8_t b);
extern int16_t	ft_i16min(int16_t a, int16_t b);
extern int32_t	ft_i32min(int32_t a, int32_t b);
extern int64_t	ft_i64min(int64_t a, int64_t b);
extern ssize_t	ft_imin(ssize_t a, ssize_t b);

extern uint8_t	ft_u8min(uint8_t a, uint8_t b);
extern uint16_t	ft_u16min(uint16_t a, uint16_t b);
extern uint32_t	ft_u32min(uint32_t a, uint32_t b);
extern uint64_t	ft_u64min(uint64_t a, uint64_t b);
extern size_t	ft_umin(size_t a, size_t b);

extern uint64_t	ft_pow(int64_t n, int16_t p);
extern uint8_t	pow2_next8(uint8_t n);
extern uint16_t	pow2_next16(uint16_t n);
extern uint32_t	pow2_next32(uint32_t n);
extern uint64_t	pow2_next64(uint64_t n);
extern size_t	pow2_next(size_t n);

extern t_bool	ft_ieq(int a, int b);
extern t_bool	ft_ueq(unsigned int a, unsigned int b);
extern t_bool	ft_leq(long a, long b);
extern t_bool	ft_uleq(unsigned long a, unsigned long b);
extern t_bool	ft_lleq(long long int a, long long int b);
extern t_bool	ft_ulleq(unsigned long long int a, unsigned long long int b);
extern t_bool	ft_szeq(ssize_t a, ssize_t b);
extern t_bool	ft_uszeq(size_t a, size_t b);
extern t_bool	ft_streq(char const *a, char const *b);

#endif
