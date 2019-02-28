/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llelievr <llelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/14 12:04:24 by llelievr          #+#    #+#             */
/*   Updated: 2019/02/28 02:33:14 by llelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf.h"

static void		compute_dir(t_wolf *wolf, t_ray *ray)
{
	if (ray->dir.x < 0)
		ray->side_dist.x = (wolf->player.pos.x - (float)ray->hit_pos.x) * ray->delta_dist.x;
	else
		ray->side_dist.x = ((float)ray->hit_pos.x + 1.0 - wolf->player.pos.x) * ray->delta_dist.x;
	if (ray->dir.y < 0)
		ray->side_dist.y = (wolf->player.pos.y - (float)ray->hit_pos.y) * ray->delta_dist.y;
	else
		ray->side_dist.y = ((float)ray->hit_pos.y + 1.0 - wolf->player.pos.y) * ray->delta_dist.y;
}

static void	compute_face(t_ray *ray)
{
	if (ray->side == 0 && ray->dir.x < 0)
		ray->face = F_NORTH;
	if (ray->side == 0 && ray->dir.x > 0)
		ray->face = F_SOUTH;
	if (ray->side == 1 && ray->dir.y > 0)
		ray->face = F_WEST;
	if (ray->side == 1 && ray->dir.y < 0)
		ray->face = F_EAST;
}

void			dist_round_block(t_wolf *wolf, t_ray *ray)
{
	t_block_round *block = (t_block_round *)ray->hit->block;
	double p = ray->hit_pos.x + 0.5;
	double q = ray->hit_pos.y + 0.5;

	double r = block->radius;

	/*if (ray->hit->block->height == 2)
		return ;*/ // MEH / 20 ? (On devrai le fix !)
	if (ray->dir.x == 0)
	{
		ray->dist -= 1 - (r * 2);
		return;
	}

	double m = ray->dir.y / ray->dir.x;
	double c = wolf->player.pos.y - wolf->player.pos.x * m; 

	double A = (m * m + 1);
	double B = 2 * (m * c - m * q - p);
	double C = (q * q - r * r + p * p - 2 * c * q + c * c);

	double delta = B * B - 4 * A * C;

	if (delta > 0)
	{
		float s_delta = sqrt(delta);
		float x1 = (-B - s_delta) / (2 * A);
		float y1 = m * x1 + c;

		float dist1;
		if (ray->side == 0)
			dist1 = (x1 - wolf->player.pos.x) / ray->dir.x;
		else
			dist1 = (y1 - wolf->player.pos.y) / ray->dir.y;
		float x2 = (-B + s_delta) / (2 * A);
		float y2 = m * x2 + c;

		float dist2;
		if (ray->side == 0)
			dist2 = (x2 - wolf->player.pos.x) / ray->dir.x;
		else
			dist2 = (y2 - wolf->player.pos.y) / ray->dir.y;
		ray->circle_last_hit_x = (dist1 < dist2 ? x1 : x2) - p;
		ray->circle_last_hit_y = (dist1 < dist2 ? y1 : y2) - q;
		ray->circle_last_out_dist = dist1 < dist2 ? dist2 : dist1;

		ray->dist = dist1 < dist2 ? dist1 : dist2;
	} else
		ray->hit = NULL;
}

static void	compute_dist(t_wolf *wolf, t_ray *ray)
{
	if (ray->hit)
	{
		if (ray->hit->block->type == B_ROUND)
			return (dist_round_block(wolf, ray));
	}
	if (ray->side == 0)
		ray->dist = (ray->hit_pos.x - wolf->player.pos.x + (1 - ray->step.x) / 2.0) / ray->dir.x;
	else
		ray->dist = (ray->hit_pos.y - wolf->player.pos.y + (1 - ray->step.y) / 2.0) / ray->dir.y;

	if (ray->dist <= 0.1)
		ray->dist = 0.1;
}

t_ray			create_ray(t_wolf *wolf, int x)
{
	t_ray	ray;

	ray.x = x;
	ray.dir = ft_mat2_mulv(wolf->player.matrix, ft_vec2_add(
		(t_vec2){0, wolf->dist_to_plane },
		(t_vec2){ PLANE * (2.0f * (float)x / S_WIDTH - 1.0f) * S_RATIO, 0}));
	ray.step = (t_pixel) { ray.dir.x > 0 ? 1 : -1 , ray.dir.y > 0 ? 1 : -1, 0 };
	ray.side_dist = (t_vec2){0, 0};
	ray.delta_dist = (t_vec2){fabs(1.0f / ray.dir.x), fabs(1.0f / ray.dir.y)};
	ray.hit_pos = (t_pixel){.x = wolf->player.pos.x, .y = wolf->player.pos.y};
	ray.hit = wolf->world.data[ray.hit_pos.y][ray.hit_pos.x];
	ray.fhit = ray.hit;
	ray.side = ray.side_dist.x < ray.side_dist.y;
	compute_face(&ray);
	compute_dist(wolf, &ray);
	return (ray);
}

t_bool			next_ray(t_wolf *wolf, t_ray *ray)
{
	compute_dir(wolf, ray);
	if (ray->side_dist.x < ray->side_dist.y)
	{
		ray->side_dist.x += ray->delta_dist.x;
		ray->hit_pos.x += ray->step.x;
		ray->side = 0;
	}
	else
	{
		ray->side_dist.y += ray->delta_dist.y;
		ray->hit_pos.y += ray->step.y;
		ray->side = 1;
	}
	ray->hit = NULL;
	ray->fhit = NULL;
	if (ray->hit_pos.x < 0 || ray->hit_pos.x >= wolf->world.size.x || ray->hit_pos.y < 0 || ray->hit_pos.y >= wolf->world.size.y)
		return (FALSE);
	ray->hit = wolf->world.data[ray->hit_pos.y][ray->hit_pos.x];
	ray->fhit = ray->hit;
	compute_face(ray);
	compute_dist(wolf, ray);
	return (TRUE);
}


t_bool			prev_ray(t_wolf *wolf, t_ray *ray)
{
	if (ray->side == 0)
	{
		ray->side_dist.x -= ray->delta_dist.x;
		ray->hit_pos.x -= ray->step.x;
		ray->side = 1;
	}
	else
	{
		ray->side_dist.y -= ray->delta_dist.y;
		ray->hit_pos.y -= ray->step.y;
		ray->side = 0;
	}
	ray->hit = NULL;
	ray->fhit = NULL;
	if (ray->hit_pos.x < 0 || ray->hit_pos.x >= wolf->world.size.x || ray->hit_pos.y < 0 || ray->hit_pos.y >= wolf->world.size.y)
		return (FALSE);
	ray->hit = wolf->world.data[ray->hit_pos.y][ray->hit_pos.x];
	ray->fhit = ray->hit;
	compute_face(ray);
	compute_dist(wolf, ray);
	return (TRUE);
}