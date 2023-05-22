/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_draw_floor.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gehebert <gehebert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/26 17:27:04 by iamongeo          #+#    #+#             */
/*   Updated: 2023/05/22 19:28:02 by gehebert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	__render_floor_init(t_cub *cub, uint32_t **b, float **p)
{
	*p = cub->renderer.floor_factors - 1;
	b[0] = (uint32_t *)cub->renderer.bg_layer->pixels
		+ (SCN_WIDTH * (cub->scn_midy + 1)) - 1;
	b[1] = (uint32_t *)cub->renderer.walls_layer->pixels
		+ (SCN_WIDTH * (cub->scn_midy + 1)) - 1;
}

static void	__render_ceiling_init(t_cub *cub, uint32_t **b, float **p)
{
	__render_floor_init(cub, b, p);
	b[2] = (uint32_t *)cub->renderer.bg_layer->pixels
		+ (SCN_WIDTH * (cub->scn_midy - 2)) - 1;
}

static void	__render_floor_incr(float **ps, uint32_t **bs)
{
//	static int	i;

//	printf("i : %d ", i++);
	++(*ps);
//	++rays[0];
//	++rays[1];
	++(bs[0]);
	++(bs[1]);
}

static void	__render_flr_cil_incr(float **ps, uint32_t **bs)
{
	++(*ps);
//	++rays[0];
//	++rays[1];
	++(bs[0]);
	++(bs[1]);
	++(bs[2]);
	++(bs[3]);
}

void	__render_sky(t_cub *cub, t_rdata *rd)
{
	int			texture_xoffsets[SCN_WIDTH];
	uint32_t	*pxls;
	int			*tofs[2];
	int			x;
	int			y;

	(void)rd;
	tofs[0] = texture_xoffsets - 1;
	x = -1;
	while (++x < SCN_WIDTH)
		*(++tofs[0]) = (int)((x - cub->scn_midx) * cub->inv_sw
				* cub->renderer.sky_fov_to_tex
				+ cub->renderer.sky_ori_offset) % cub->tex.skymap->width;
	pxls = (uint32_t *)cub->renderer.sky_layer->pixels;
	tofs[1] = cub->renderer.sky_yoffsets - 1;
//	printf("skymap ptr : %p\n", cub->tex.skymap);
//	printf("sky w, h : (%d, %d), fov to tex : %f, sky ori off : %d\n", cub->tex.skymap->width, cub->tex.skymap->height, 
//		cub->renderer.sky_fov_to_tex, cub->renderer.sky_ori_offset);
	y = -1;
	while (++y < cub->scn_midy)
	{
		tofs[0] = texture_xoffsets - 1;
		++tofs[1];
		x = -1;
		while (++x < SCN_WIDTH)
			*(++pxls) = ((uint32_t *)cub->tex.skymap->pixels)[*(++tofs[0])
				+ (*tofs[1]) * cub->tex.skymap->width];
	}
}

static void	__render_floor_sky(t_cub *cub, t_rdata *rdata)
{
//	const float		flr_ratios[2] = {};//{cub->renderer.flrw_to_cw, cub->renderer.flrh_to_cw};
//	const float		*rays[2] = {rd[0].rx, rd[0].ry};
	t_rdata			*rd;
	float			*params;
	float			param_scalar;
	uint32_t		*buffs[3];
	int				incr[2];
	int				cx;
	int				cy;
	float			mx;
	float			my;
	float			x;
	float			y;
	mlx_texture_t	**tex_arr;

	__render_floor_init(cub, buffs, &params);
//	params = cub->renderer.floor_factors - 1;
	incr[1] = 0;
	while (++incr[1] < cub->scn_midy)
	{
		rd = rdata - 1;
		incr[0] = -1;
		while (++incr[0] < SCN_WIDTH)
		{
			++rd;
			__render_floor_incr(&params, buffs);
//			printf("*rays[0] %f, ", *rays[0]);
//			printf("*params %f\n", *params);
			param_scalar = *params;

			
			x = *rd->rx * param_scalar + cub->hero.ply_obj->px;
			y = *rd->ry * param_scalar + cub->hero.ply_obj->py;
	//		printf("x, y : %f, %f\n", x, y);
//			printf("cx, cy : %d, %d\n", cx, cy);
			if (*buffs[1] || x < 0.0f || y < 0.0f)
//				|| x >= cub->map.width || y >= cub->map.height)
				continue ;
			cx = (int)(x * cub->inv_cw);//fmodf(x, CELL_WIDTH);
			cy = (int)(y * cub->inv_cw);//fmodf(y, CELL_WIDTH);
			if (cx >= cub->map.width || cy >= cub->map.height)
				continue ;
			mx = x - (cx * CELL_WIDTH);//fmodf(x, CELL_WIDTH);//x - cx;
			my = y - (cy * CELL_WIDTH);//fmodf(y, CELL_WIDTH);//y - cy;
			tex_arr = cub->map.mx[cy][cx]->xwalls;
//			printf("tex_arr : %p, mx %f, my %f\n", tex_arr, mx, my);
			if (!tex_arr)// || tex_arr[1] != NULL)
				continue ;
	//		printf("drawing on mx %f, my %f\n", mx, my);
			*buffs[0] = get_tex_pixel(tex_arr[0], mx * tex_arr[0]->width * cub->inv_cw,//* flr_ratios[0],
				my * tex_arr[0]->height * cub->inv_cw);//flr_ratios[1]);
//			*buffs[0] = floor_get_pixel(cub->floor_tex,
//					(int)(fmodf(*rays[0] * (*params) + cub->hero.px, CELL_WIDTH)
//						* flr_ratios[0]),
//					(int)(fmodf(*rays[1] * (*params) + cub->hero.py, CELL_WIDTH)
//						* flr_ratios[1]));
		}
//		rays[0] = rd[0].rx;
//		rays[1] = rd[0].ry;
	}
}

static void	__render_floor_ceiling(t_cub *cub, t_rdata *rdata)
{
	//const float		*rays[2] = {rd[0].rx, rd[0].ry};
	t_rdata			*rd;
	float			*params;
//	float			param_scalar;
	uint32_t		*buffs[4];
	int				incr[2];
//	mlx_texture_t	*texs[2];
	int				cx;
	int				cy;
	float			mx;
	float			my;
	float			x;
	float			y;
	mlx_texture_t	**tex_arr;

//	__render_floor_init(cub, buffs, &params);
//	printf("renderering floor and ceiling \n");
	__render_ceiling_init(cub, buffs, &params);
	incr[1] = 0;
	while (++incr[1] < cub->scn_midy)
	{
		rd = rdata - 1;
		incr[0] = -1;
		while (++incr[0] < SCN_WIDTH)
		{
			++rd;
			__render_flr_cil_incr(&params, buffs);

//			printf("*params vs param_scalar vs rd->dist : %f vs %f vs %f\n",
//				*params, param_scalar, rd->dist);

//			param_scalar = *params;
//			if (param_scalar >= rd->dist)
//				param_scalar = rd->dist - 0.01f;
			x = *rd->rx * (*params) + cub->hero.ply_obj->px;
			y = *rd->ry * (*params) + cub->hero.ply_obj->py;
	//		printf("x, y : %f, %f\n", x, y);
//			printf("cx, cy : %d, %d\n", cx, cy);
			if (*buffs[1] || x < 0.0f || y < 0.0f)
//				|| x >= cub->map.width || y >= cub->map.height)
				continue ;
			cx = (int)(x * cub->inv_cw);//fmodf(x, CELL_WIDTH);
			cy = (int)(y * cub->inv_cw);//fmodf(y, CELL_WIDTH);
			if (cx >= cub->map.width || cy >= cub->map.height)
				continue ;			
			mx = x - (cx * CELL_WIDTH);//fmodf(x, CELL_WIDTH);//x - cx;
			my = y - (cy * CELL_WIDTH);//fmodf(y, CELL_WIDTH);//y - cy;

			if (is_wall(&cub->map, cx, cy))
			{
//				printf("is wall. cell before : (%d, %d)\n", cx, cy);
//				if (rd->side % 2)
				if (rd->side == N_SIDE || rd->side == S_SIDE)
					cy = rd->cy - rd->cincr_y;
				else
					cx = rd->cx - rd->cincr_x;
//				printf("is wall. cell after : (%d, %d)\n", cx, cy);
			}

			tex_arr = cub->map.mx[cy][cx]->xwalls;
		//	printf("tex_arr : %p, mx %f, my %f\n", tex_arr, mx, my);
			if (!tex_arr)// || tex_arr[2] != NULL)
				continue ;

		//	printf("drawing on mx %f, my %f\n", mx, my);
			*buffs[0] = get_tex_pixel(tex_arr[0],
				mx * tex_arr[0]->width * cub->inv_cw,//* flr_ratios[0],
				my * tex_arr[0]->height * cub->inv_cw);//flr_ratios[1]);
			*buffs[2] = get_tex_pixel(tex_arr[1],
				mx * tex_arr[1]->width * cub->inv_cw,//* flr_ratios[0],
				my * tex_arr[1]->height * cub->inv_cw);//flr_ratios[1]);
//			*buffs[0] = floor_get_pixel(cub->floor_tex,
//					(int)(fmodf(*rays[0] * (*params) + cub->hero.px, CELL_WIDTH)
//						* flr_ratios[0]),
//					(int)(fmodf(*rays[1] * (*params) + cub->hero.py, CELL_WIDTH)
//						* flr_ratios[1]));
		}
//		rays[0] = rd[0].rx;
//		rays[1] = rd[0].ry;
		buffs[2] -= 2 * SCN_WIDTH;
	}
}

void	render_floor_sky(t_cub *cub)//, t_rdata *rd)
{
	__render_sky(cub, cub->hero.rcast.rdata);
	if (0 || cub->tex.open_sky)
		__render_floor_sky(cub, cub->hero.rcast.rdata);// rd);
	else
		__render_floor_ceiling(cub, cub->hero.rcast.rdata);//;rd);
}
