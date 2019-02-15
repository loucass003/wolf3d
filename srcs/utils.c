/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llelievr <llelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/13 23:40:27 by llelievr          #+#    #+#             */
/*   Updated: 2019/02/16 00:30:55 by llelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf.h"

double	ticks_to_ms(clock_t ticks)
{
	return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	unsigned int	index;

	int bpp = surface->format->BytesPerPixel;
	index = y * surface->pitch + x * bpp;
	Uint8 *p = (Uint8 *)surface->pixels + index;
	return ((p[0] << 16) | (p[1] << 8) | p[2]);
}

void	draw_line(t_wolf *wolf, t_pixel p0, t_pixel p1)
{
	t_pixel			d;
	t_pixel			s;
	int				e[2];
	unsigned int 	index;

	d = (t_pixel){ abs(p1.x - p0.x), abs(p1.y - p0.y) };
	s = (t_pixel){ (p0.x < p1.x ? 1 : -1), (p0.y < p1.y ? 1 : -1) };
	e[0] = (d.x > d.y ? d.x : -d.y) / 2;
	while (p0.x != p1.x || p0.y != p1.y)
	{
		index = p0.y * (int)S_WIDTH + p0.x;
		if (index > IMG_MAX_I)
			continue ;
		((unsigned int *)wolf->pixels)[index] = p0.color;
		e[1] = e[0];
		if (e[1] > -d.x)
		{
			e[0] -= d.y;
			p0.x += s.x;
		}
		if (e[1] < d.y)
		{
			e[0] += d.x;
			p0.y += s.y;
		}
	}
}
