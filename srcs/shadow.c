/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvivaudo <fvivaudo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/31 15:57:51 by fvivaudo          #+#    #+#             */
/*   Updated: 2017/03/16 10:34:14 by dmoureu-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rtv1.h>

//I consider an object like a series of wall instead of a single entity, it might not be the best way to calculate
// transparency, refraction
double		computeshadow(t_env *e, t_ray *r, double light, double dist)
{
	t_obj			*cursor;
	cursor = e->obj;
//	double t = dist; // distance between point and light
	//printf("type == %d\n", cursor->type);
	//if (cursor->type == TYPE_SPHERE)
	//	printf("Transparency computeshadow == %g\n",cursor->material.transparency);
	while (cursor)
	{
		//if (cursor->type == TYPE_SPHERE)
		//	printf("type == %d\n", cursor->type);
		//if (cursor->type == TYPE_SPHERE)
		//	printf("Transparency computeshadow 1 == %g\n",cursor->material.transparency);
		if ((cursor->type == TYPE_SPHERE && iraysphere(r, cursor, &dist)) ||
			(cursor->type == TYPE_PLANE && irayplane(r, cursor, &dist)) ||
			(cursor->type == TYPE_CYLINDER && iraycylinder(r, cursor, &dist)) ||
			(cursor->type == TYPE_CONE && iraycone(r, cursor, &dist)) ||
			(cursor->type == TYPE_QUADRIC && irayquadric(r, cursor, &dist)) ||
			(cursor->type == TYPE_TORUS && iraytorus(r, cursor, &dist)) ||
			(cursor->type == TYPE_TRIANGLE && iraytriangle(r, cursor, &dist, 1)))
		{
		//	e->t = t;
			//printf("e->cmat.transparency = %g\n", e->cmat.transparency);
			light *= cursor->material.transparency; // is it accurate?
		}
		cursor = cursor->nextitem;
	}
	//if (cursor->type == TYPE_SPHERE)
	//	printf("Transparency computeshadow 1 == %g\n", e->cmat.transparency);
	//	printf("Transparency computeshadow 1 == %g\n", e->cmat.transparency);
	//	ft_putendl("Transparency computeshadow 1 == %g\n",cursor->material.transparency);
	return (light);
	// if an intersection was found
}

void	perlinmap (t_env *e)
{
	t_vec output;
	double noiseCoef = 0;

	for (int level = 1; level < 10; level++)
	{
		noiseCoef += (1.0 / level) * fabs(noise(level * e->cmat.procedural_scale * e->newstart.x, level * e->cmat.procedural_scale * e->newstart.y, level * e->cmat.procedural_scale * e->newstart.z));
	};
	//noiseCoef = 0.5 * sin((e->newstart.x + e->newstart.y) * e->cmat.procedural_scale + noiseCoef) + 0.5;
	output = vectoradd(
	vectorscale(noiseCoef, *(t_vec*)&e->cmat.diffuse)
	,vectorscale((1.0 - noiseCoef), *(t_vec*)&e->cmat.diffuse2));

	e->cmat.diffuse.red = output.x;
	e->cmat.diffuse.green = output.y;
	e->cmat.diffuse.blue = output.z;
}

void	marble (t_env *e)
{
	t_vec output;
	double noiseCoef = 0;

	for (int level = 1; level < 10; level++)
	{
		noiseCoef += (1.0 / level) * fabs(noise(level * e->cmat.procedural_scale * e->newstart.x, level * e->cmat.procedural_scale * e->newstart.y, level * e->cmat.procedural_scale * e->newstart.z));
	};
	noiseCoef = 0.5 * sin((e->newstart.x + e->newstart.y) * e->cmat.procedural_scale + noiseCoef) + 0.5;
	output = vectoradd(
	vectorscale(noiseCoef, *(t_vec*)&e->cmat.diffuse)
	,vectorscale((1.0 - noiseCoef), *(t_vec*)&e->cmat.diffuse2));

	e->cmat.diffuse.red = output.x;
	e->cmat.diffuse.green = output.y;
	e->cmat.diffuse.blue = output.z;
}


void	checker (t_env *e)
{
	if (((int)floor(e->newstart.x / (1.0 / e->cmat.procedural_scale))
		+ (int)floor(e->newstart.y / (1.0 / e->cmat.procedural_scale))
		+ (int)floor(e->newstart.z / (1.0 / e->cmat.procedural_scale))) % 2 == 0)
	{
		e->cmat.diffuse = e->cmat.diffuse2;
	}
}

/*for each light in the scene
**{
**	if the light is not in shadow of another object
**	{
**		add this light contribution to computed color;
**	}
**}*/
int		deal_shadow(t_env *e)
{
	t_ray			lightray;
	t_light			*tmplight;
	double			tmpdot;
	double			distancetolight;
//	t_obj			*cursor;

	tmplight = e->lights;
//	cursor = e->obj;

if (e->cmat.type == TEXTURE_MARBLE)
	marble(e);
else if (e->cmat.type == TEXTURE_CHECKER)
	checker(e);
else if (e->cmat.type == TEXTURE_PERLIN)
	perlinmap(e);
else
	e->col = e->cmat.diffuse;

// printf("e->cmat.diffuse.red = %g, e->cmat.diffuse.green = %g, e->cmat.diffuse.blue= %g\n", e->cmat.diffuse.red, e->cmat.diffuse.green, e->cmat.diffuse.blue);

	while (tmplight)
	{
		if (e->cmat.bump)
		{
			e->n = bump_mapping(e);
		}
		//if (cursor->type == TYPE_SPHERE)
		//	printf("Transparency computeshadow 2 == %g\n",cursor->material.transparency);
		e->clight = *tmplight;
		tmplight = tmplight->next;
		//distance between intersection point and light
		e->dist = vectorsub(e->clight.pos, e->newstart);
		distancetolight = vectormagnitude(e->dist);
//		printf("Checkpoint0\n");
/*		printf("e->newstart.x = %g, e->newstart.y = %g, e->newstart.z = %g\n", e->newstart.x, e->newstart.y, e->newstart.z);
		printf("e->clight.pos.x = %g, e->clight.pos.y = %g, e->clight.pos.z = %g\n", e->clight.pos.x, e->clight.pos.y, e->clight.pos.z);
		printf("e->n.x = %g, e->n.y = %g, e->n.z = %g\n", e->n.x, e->n.y, e->n.z);
		printf("e->dist.x = %g, e->dist.y = %g, e->dist.z = %g\n", e->dist.x, e->dist.y, e->dist.z);
		printf("e->t = %g\n", e->t);*/


		//	printf("Transparency == %g\n",e->transcoef);

		//if the vector dot is negative, then both vectors are going in opposite directions
		vectornormalize(&e->dist);
		//printf("type == %d\n", cursor->type);
		//if (e->transcoef == 0.0)
	//	{



			if ((tmpdot = vectordot(e->n, e->dist)) <= 0.0f)
			{
				continue;
			}
	//	}
	//	while (cursor)
		//{/
		//	if (cursor->type == TYPE_SPHERE)
		//		printf("Transparency computeshadow 1 == %g\n",cursor->material.transparency);
		//	cursor = cursor->nextitem;
		//}
		if (e->transcoef == 0.0)
		{
			e->t = vectormagnitude(e->dist);
			if (e->t <= 0.0f)
				continue; // delete later, eventually
		}
		//Check if there is something between point and light
		//limits light range, or does it?
	//	lightray.dir = vectorscale((1 / e->t), e->dist); //alternative to normalization?
		lightray.dir = e->dist;
		lightray.start = vectoradd(e->newstart, e->dist);
		//lightray.start = vectoradd(e->newstart, lightray.dir);
		// the object risks a colision with itself, which is quite problematic
		// 2 solutions :
		// -forbid colision detection with itself
		// -launch the ray a small distance away from the actual intersection point (CURRENT)
		//printf("intersection\n");
	//	printf("intersection\n");
/*		if (intersection(e, &lightray, -1))
		{
			continue;
		}*/

		double light = 1;
		//temporary removal
		if ((light = computeshadow(e, &lightray, light, distancetolight)) == 0.0) // 1 is light, is it ok?
		{
			continue;
		}


		if (e->cmat.type == TEXTURE_MARBLE)
			marble(e);
		else if (e->cmat.type == TEXTURE_CHECKER)
			checker(e);
		else if (e->cmat.type == TEXTURE_PERLIN)
			perlinmap(e);
	//	printf("light = %g\n", light);
	//	printf("intersection2\n");

	//	lambert diffusion
		//if (cursor->material.transparency == 0)
	//	if (e->transcoef == 0.0)
			e->lambert = tmpdot * e->coef;
	//	else
	//		e->lambert = 1;
		e->col.red += e->lambert * e->clight.intensity.red * e->cmat.diffuse.red * light;
		e->col.green += e->lambert * e->clight.intensity.green * e->cmat.diffuse.green * light;
		e->col.blue += e->lambert * e->clight.intensity.blue * e->cmat.diffuse.blue * light;

		//check collision with objects between object and light

		// Blinn Phong model
		blinn_phong(e, lightray.dir);
	}
	if (e->col.red == 0 && e->col.green == 0 && e->col.red == 0)
		return (FALSE);
	return (TRUE);
	//Ambient lighting

	//	printf("e->red = %g, e->green = %g, e->blue = %g\n", e->red, e->green, e->blue);
}
