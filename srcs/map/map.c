/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llelievr <llelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/19 12:29:48 by llelievr          #+#    #+#             */
/*   Updated: 2019/02/19 21:59:04 by llelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf.h"

t_block_state		*create_block_state(t_world *w, int block_id)
{
	t_block_state	*state;

	if (block_id >= w->blocks_count || block_id == -1)
	{
		if (block_id >= w->blocks_count)
		{
			ft_putstr("Block non declared used: id -> ");
			ft_putnbr(block_id + 1);
			ft_putchar('\n');
		}
		return (NULL);
	}
	if (!(state = (t_block_state *)malloc(sizeof(t_block_state))))
		return (NULL);
	state->type = B_NORMAL;
	state->block = w->blocks[block_id];
	return (state);
}

t_block_state		***load_map_data(t_world *w, t_json_value *val)
{
	t_json_array	*arr;
	t_json_element	*row_elem;
	t_json_element	*col_elem;
	t_block_state	***states;
	int				i;
	int				j;
	
	if (!val || val->type != JSON_ARRAY)
		return (NULL);
	arr = (t_json_array *)val;
	if (arr->elems_count != w->size.height)
		return (NULL);
	if (!(states = (t_block_state ***)malloc(sizeof(t_block_state **) * w->size.height)))
		return (NULL);
	row_elem = arr->elements;
	i = 0;
	while (row_elem)
	{
		if (row_elem->value->type != JSON_ARRAY 
			|| ((t_json_array *)row_elem->value)->elems_count != w->size.width)
			return (NULL);
		col_elem = ((t_json_array *)row_elem->value)->elements;
		if (!(states[i] = (t_block_state **)malloc(sizeof(t_block_side *) * w->size.width)))
			return (NULL);
		j = 0;
		while (col_elem)
		{
			if (col_elem->value->type != JSON_NUMBER)
				return (NULL);
			if (!(states[i][j++] = create_block_state(w, (int)((t_json_number *)col_elem->value)->value - 1)) && (int)((t_json_number *)col_elem->value)->value > 0)
				return (NULL);
			col_elem = col_elem->next;
		}
		i++;
		row_elem = row_elem->next;
	}
	return (states);
}