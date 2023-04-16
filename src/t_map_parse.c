/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_map_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 21:39:58 by gehebert          #+#    #+#             */
/*   Updated: 2023/04/16 17:35:11 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

t_map	*init_map(t_map *map)
{
	map->pos_x = 1;
	map->pos_y = 1;
	map->hero_side = -1;
	map->hero_x = -1;
	map->hero_y = -1;
	map->width = 0;
	map->height = 0;
	map->total_cells = 0;
	map->file = NULL;
	map->tab = NULL;
	map->flg_chk = 0;
	map->lines_to_map = 0;
	return (map);
}

static char	*spc_chk(t_map *map, int j)
{
	char	*line;
	int		i;

	line = map->tab[j];
	i = -1;
	while (++i < map->width)
		if (!ft_strchr(MAP_CHARS, line[i]))
			line[i] = ' ';
	return (line);
}

static	int	transcribe(t_map *map, int fd)
{
	char	*tmp;

	tmp = skip_file_lines(map, fd, 0);
	map->width = int_strlen(tmp);
	printf("transcribe : map width %d, tmp : %s\n", map->width, tmp);
	map->height = 0;
	while (tmp != NULL)
	{
		map->height++;
		if (int_strlen(tmp) > map->width)
			map->width = int_strlen(tmp);
		free(tmp);
		tmp = get_next_line(fd);
	}
	map->total_cells = (map->height * map->width);
	printf("DEBUG: map->h = %d:\n", map->height);
	printf("DEBUG: map->w = %d:\n", map->width);
	printf("DEBUG: map->total_cells = %d:\n", map->total_cells);
	return (map->height);
}

static t_map	*map_frame(t_map *map, int fd)
{
	char	*temp;
	int		nb;

	temp = skip_file_lines(map, fd, map->lines_to_map);
	nb = 0;
	while (temp && (nb < map->height))
	{
		map->tab[nb] = (char *)ft_calloc(sizeof(char *), (map->width + 1));
		if (map->width == int_strlen(temp))
		{
			map->tab[nb] = ft_strncpy_i(map->tab[nb], temp, map->width, 0);
			map->tab[nb] = spc_chk(map, nb);
		}
		else
			map->tab[nb] = ft_strncpy_i(map->tab[nb], temp, map->width, 0);
		free(temp);
		nb++;
		temp = get_next_line(fd);
	}
	if (temp)
		free(temp);
	map = wall_check(map);
	if (map->flg_chk == 1)
	{
		printf("DEBUG WARNING : wall chk failed \n");
		return (NULL);
	}
	return (map);
}

int	map_checker(t_cub *cub, t_map *map, char *file)
{
	int		fd;

	if (ft_strfcmp(".cub", file, 4))
		return (error("Wrong file extention.", map));
	fd = open(file, O_RDONLY);
	if (fd < 0)
		return (error("Could not open file", map));
	cub->tex_id = -1;
	if (tex_parse(cub, map, fd) < 0)
	{
//		printf("map_checker :: tex_parse failed\n");
		return (-1);
	}
	if (transcribe(map, fd) < 3)
		return (error("Map in file is too short", map));
	close(fd);
	fd = open(file, O_RDONLY);
		map->tab = (char **)ft_calloc(sizeof(char *), (map->height + 1));
	if (!map->tab || !map_frame(map, fd) || build_grid_coords_map(map) < 0
		|| build_collision_map(map) < 0)
		return (-1);
	print_collision_map(map);
	close(fd);
	return (0);
}
