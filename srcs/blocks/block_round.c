#include "wolf.h"

t_block		*load_round_block(t_world *w, t_json_object *obj)
{
	t_block_round	*block;
	t_json_value	*val;
	double			*texture_i;

	if (!(block = (t_block_round *)malloc(sizeof(t_block_round))))
		return (NULL);
	block->super.type = B_ROUND;
	if (!ft_json_color(json_object_get(obj, "minimap_color"), &block->minimap_color))
		return (json_free_ret(block));
	val = json_object_get(obj, "height");
	block->super.height = (!val || val->type != JSON_NUMBER ? 1 : ((t_json_number *)val)->value);
	val = json_object_get(obj, "radius");
	block->radius = (!val || val->type != JSON_NUMBER ? 0.5 : ((t_json_number *)val)->value);
	if (!(texture_i = json_get_number(obj, "texture")))
		return (NULL);
	block->texture = w->textures[(int)*texture_i];
	return ((t_block *)block);
}

int			round_block_minimap(t_wolf *wolf, t_block_state *state, t_vec2 map)
{
	t_block_round	*block;

	block = (t_block_round *)state->block;
	(void)wolf;
	float x = map.x - (int)map.x - 0.5;
	float y = map.y - (int)map.y - 0.5;
	if (x * x + y * y < block->radius * block->radius)
		return (ft_color_i(block->minimap_color));
	else
		return (-1);
}