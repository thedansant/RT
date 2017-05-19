/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvivaudo <fvivaudo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/31 15:59:09 by fvivaudo          #+#    #+#             */
/*   Updated: 2017/03/17 15:40:03 by dmoureu-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rtv1.h>

void			reset(t_env *e, int x, int y)
{
	t_vec viewplanepoint;

	viewplanepoint = vectoradd(
	vectoradd(e->cam.viewplanebottomleftpoint, vectorscale(x, e->cam.xincvector)),
	vectorscale(y, e->cam.yincvector));
	e->r.dir = vectorsub(viewplanepoint, e->cam.eyepoint);
	vectornormalize(&e->r.dir);
	e->id = -1;
	e->col.red = 0;
	e->col.green = 0;
	e->col.blue = 0;
	e->level = 0;
	e->coef = 1.0;
	e->r.initialstart = e->cam.eyepoint;
	e->r.start = e->cam.eyepoint;
	e->vdir = e->r.dir;
	e->crefraction = 1.0;
	e->transcoef = 0;
	e->reflecoef = 0;
//	e->r.start.x = e->eyepoint.x + x;
//	e->r.start.y = e->eyepoint.y + y;
}


/*t_color	marble(t_env *e)
{

}*/

//color perturbation, to implement later, maybe.
/*		t_vec output;
		double noiseCoef = 0;
		e->cmat.diffuse2 = colorinit(1, 1, 0);

		for (int level = 1; level < 10; level++)
		{
			noiseCoef += (1.0 / level) * fabs(noise(level * 0.05 * e->newstart.x, level * 0.05 * e->newstart.y, level * 0.05 * e->newstart.z));
		};
		noiseCoef = 0.5 * sin((e->newstart.x + e->newstart.y) * 0.05 + noiseCoef) + 0.5;
		output = vectoradd(
		vectorscale(noiseCoef, *(t_vec*)&e->cmat.diffuse)
		,vectorscale((1.0 - noiseCoef), *(t_vec*)&e->cmat.diffuse2));

		e->cmat.diffuse.red = output.x;
		e->cmat.diffuse.green = output.y;
		e->cmat.diffuse.blue = output.z;*/

void		refract(t_env *e)
{
	double	ViewProjection = vectordot(e->r.dir, e->n);
	double coef = 1;
	double fCosThetaT;
	double 	fdensity1 = e->crefraction;
	double 	fdensity2 = e->cmat.refraction;
	//printf("fdensity1  ==  %g  ||  fdensity2 == %g\n", fdensity1, fdensity2);
	double reflectance;
	double	fCosThetaI = fabs(ViewProjection);
	double fSinThetaI = sqrtf(1 - fCosThetaI * fCosThetaI);
	double fSinThetaT = (fdensity1 / fdensity2) * fSinThetaI;
	fSinThetaI = sqrt(1 - fCosThetaI * fCosThetaI);
	fSinThetaT = (fdensity1 / fdensity2) * fSinThetaI; 
	//printf("%f\n", fSinThetaI);
	if (fSinThetaT * fSinThetaT > 0.9999)
	{
 		reflectance = 1.0; // pure reflectance at grazing angle
		fCosThetaT = 0.0;
	}
	else
	{
	 	fCosThetaT = sqrtf(1 - fSinThetaT * fSinThetaT);
	 	double fReflectanceOrtho = (fdensity2 * fCosThetaT - fdensity1 * fCosThetaI ) / (fdensity2 * fCosThetaT + fdensity1 * fCosThetaI);
	 	fReflectanceOrtho = fReflectanceOrtho * fReflectanceOrtho;
	 
		double fReflectanceParal =(fdensity1 * fCosThetaT - fdensity2 * fCosThetaI ) / (fdensity1 * fCosThetaT + fdensity2 * fCosThetaI);

		fReflectanceParal = fReflectanceParal * fReflectanceParal;
		reflectance = 0.5 * (fReflectanceOrtho + fReflectanceParal);
	}

	double fRoulette = (1.0 / RAND_MAX) * rand();
	//printf("roulette  ==  %g            |||          reflectance == %g\n",fRoulette, reflectance);
	if (fRoulette < reflectance)
	{
	//	printf("lol\n");
	 // rays are either reflected, ..
	 	coef *= e->cmat.reflection;
	 	e->r.start = vectoradd(e->newstart, e->r.dir);
	 	e->r.dir = vectoradd(e->r.dir, vectorscale(e->cmat.reflection, e->n));
		vectornormalize(&e->r.dir);
	}
	else
	{
	  //..transmitted..
	//	printf("mdr\n");


		coef *= e->cmat.refraction;
		double fOldRefractionCoef = e->crefraction;
//		e->obj->reversen = FALSE;
//		if (e->obj->t[1] != DOESNOTEXIST)
//		{
//			e->crefraction = 1.0;
//			e->obj->reversen = TRUE;						
//		}
//		else
//		{
//			e->crefraction = e->cmat.refraction;
//		}

		e->r.start = vectoradd(e->newstart, e->r.dir);
		e->r.dir = vectoradd(e->r.dir,vectorscale(fCosThetaI, e->n));
		e->r.dir = vectorscale((fOldRefractionCoef / e->crefraction), e->r.dir);
		e->r.dir = vectoradd(e->r.dir, vectorscale(-fCosThetaT , e->n));
		vectornormalize(&e->r.dir);
	}
}

double  **singleton_depth_map() {
		static char init = 0;
		static double **depth_map;
		int i;

		if (!init)
		{
			printf("singleton init\n");
			depth_map = malloc(sizeof(double *) * (HEIGHT + 1));
			i = -1;
			printf("init 1\n");

			while(++i < HEIGHT + 1)
			{
			//	printf("init 2\n");

				depth_map[i] = malloc(sizeof(double) * (1 + WIDTH));
				ft_memset(depth_map[i], 0, 1 + WIDTH);
			}
			init = 1;
			printf("init dones\n");
		}
	//	printf("singleton called\n");

		return depth_map;
}


t_color			reflect_and_refract(t_env e)
{
	t_color			reflecolor;
	t_color			refracolor;
	t_color			res;
	t_vec			tmp;
	t_obj			*collide;
//	double		**effect;

	e.t = MAX_RANGE;
//	if (e.level == 0)
//	{
		//printf("%f\n", e.t);
//	  effect = singleton_depth_map();
//		effect[e.y][e.x] = e.t;
	 // printf("x %d, y %d e.t %f\n ",e.x, e.y, e.t);

//	}
		collide = computeray(&e);
	//ft_putendl("ok99");
	if (!collide)
	{
		res = colorinit(0, 0, 0);
		return (res);
	}
	e.cmat = collide->material;


	//if (e.id == 2)
	//e.cmat.transparency = 1;
	//e.blue += 1;

	//e.cmat.reflection = 1.0;

	double originalcoef;
	double tmpcoef1;
	double tmpcoef2;
	double angle = cos(vectordot(e.r.dir, vectorscale(1.0, e.n)));
//	printf("angle == %g\n", angle);

	//ft_putendl("ok0");
	double R0 = pow((e.crefraction - e.cmat.refraction) / (e.crefraction + e.cmat.refraction), 2);

//	printf("r0 == %g  \\ transparency == %g\n", R0, e.cmat.transparency);
	//ft_putendl("ok1");
	//shlick's approximation

	//double kr = R0 + (1.0 - R0) * pow((1.0 - angle), 5);

	//printf("angle  ==   %g ||| R0  ==    %g    |||   e.reflecoef  ==   %g\n", angle, R0, e.reflecoef);

	e.reflecoef = R0 + (1.0 - R0) * pow((1.0 - angle), 5) * (1.0 - e.cmat.transparency);
	e.transcoef = 1.0 - e.reflecoef; // how much light is reflected, computed by Fresnel equation
	//e.transcoef = 0.7;
	//e.reflecoef = 0.3;
	//ft_putendl("ok2");
//	e.reflecoef *= e.cmat.reflection; //is it only reflection or diffusion too?
//	e.transcoef *= e.cmat.transparency;
	//if 
//	printf("e.transcoef == %g\n", e.transcoef);
//	printf(" e.reflecoef == %g\n", e.reflecoef);
	//printf(" sum == %g\n", e.reflecoef + e.transcoef);
	//if (e.transcoef)
	//printf(" e.transcoef == %g\n", e.transcoef);

	//ft_putendl("ok3");
	originalcoef = e.coef /*  (1.0 - e.cmat.transparency)*/;
	tmpcoef1 = e.coef * e.reflecoef;
	tmpcoef2 = e.coef * e.transcoef;

	refracolor = colorinit(0, 0, 0);
	reflecolor = colorinit(0, 0, 0);
	//ft_putendl("ok4");

	++e.level;
	if (e.level < MAX_DEPTH_LEVEL && e.cmat.reflection > 0)
	{//only reflection
//	if (e.level != 0)
//	{
//		if (e.reflecoef)
//		if (e.transcoef)
//		printf("e.level = %d\n", e.level);
//	}
		//reflected ray = dir−2(dir⋅n )n
		e.coef = tmpcoef1;
		tmp = vectorscale(2 * vectordot(e.r.dir, e.n), e.n);
		e.r.dir = vectorsub(e.r.dir, tmp); // use temporary direction
		e.r.start = vectoradd(e.newstart, e.r.dir);
		vectornormalize(&e.r.dir);
		reflecolor = reflect_and_refract(e);
	}
	if (e.level < MAX_DEPTH_LEVEL && e.transcoef > 0/* && e.cmat.transparency > 0*/)
	{//only refraction

		e.coef = tmpcoef2;
		//e.cmat.refraction = 1.2;
		//printf(" cmat.refracoef ==  %g\n", e.cmat.refraction);
	//ft_putendl("ok6");
	//	printf("e.r.dir1.x = %g, e.r.dir1.y = %g, e.r.dir1.z = %g\n", e.r.dir.x, e.r.dir.y, e.r.dir.z);
		refract(&e);
	//	printf("e.r.dir2.x = %g, e.r.dir2.y = %g, e.r.dir2.z = %g\n", e.r.dir.x, e.r.dir.y, e.r.dir.z);
		//ft_putendl("ok7");
		refracolor = reflect_and_refract(e);
	//	printf("e.crefraction = %g\n", e.crefraction);
	//	printf("refracolor.red = %g, refracolor.green = %g, refracolor.blue = %g\n", refracolor.red, refracolor.green, refracolor.blue);

	//ft_putendl("ok8");
	//	e.coef = tmpcoef2;
	}
//	if (e.id == 2)
//	{
	//	printf("e.level = %d\n", e.level);
	//	printf("refracolor.red = %g, refracolor.green = %g, refracolor.blue = %g\n", refracolor.red, refracolor.green, refracolor.blue);
	//	printf("reflecolor.red = %g, reflecolor.green = %g, reflecolor.blue = %g\n", reflecolor.red, reflecolor.green, reflecolor.blue);
//	}
	if (e.cmat.transparency < 1.0 && e.cmat.reflection < 1.0) // transparency and reflection could be color specific, may implement later
	{
//		if (e.id == 2)
//			printf("ok\n");
		deal_shadow(&e);
	//	e.obj = NULL;
	}
/*	if (e.transcoef != 0.0)
	{
		res.red =  e.col.red;
		res.green =  e.col.green ;
		res.blue = e.col.blue;
	}	reflectionColor * kr + refractionColor * (1 - kr)
	else
	{*/
		res.red = reflecolor.red * e.reflecoef + refracolor.red * (1 - e.reflecoef) + e.col.red;
		res.green = reflecolor.green * e.reflecoef + refracolor.green * (1 - e.reflecoef) + e.col.green;
		res.blue = reflecolor.blue * e.reflecoef + refracolor.blue * (1 - e.reflecoef) + e.col.blue;
//	}

	//printf("res.blue = %g\n", res.blue);
//	if (e.id == 2)
//		printf("res.red = %g, res.green = %g, res.blue = %g\n", res.red, res.green, res.blue);
	//there is no shadow on the reflected surface, need to fix later

	/*if ((e.coef > 0) && (e.level < MAX_DEPTH_LEVEL))
	{
		++e.level;
		//reflected ray = dir−2(dir⋅n )n
		tmp = vectorscale(2 * vectordot(e.r.dir, e.n), e.n);
		e.r.dir = vectorsub(e.r.dir, tmp);
		e.r.start = vectoradd(e.newstart, e.r.dir);
		vectornormalize(&e.r.dir);
		reflect_and_refract(e);
	}

	else
	{

	}*/
	return (res);
}

unsigned char	*update_img(t_env *e, int x, int y)
{
	static unsigned char	img[3 * WIDTH * HEIGHT];

	if (e)
	{
		img[(x + y * WIDTH) * 3 + 0] =
		(unsigned char)((e->col.red * C) < C ? e->col.red * C : C);
		img[(x + y * WIDTH) * 3 + 1] =
		(unsigned char)((e->col.green * C) < C ? e->col.green * C : C);
		img[(x + y * WIDTH) * 3 + 2] =
		(unsigned char)((e->col.blue * C) < C ? e->col.blue * C : C);
	}
	return (img);
}

void			get_img_pos(int *x, int *y, int inter)
{
	*x += inter;
	while (*x > WIDTH)
	{
		*x -= WIDTH;
		++(*y);
		if (*y >= HEIGHT)
			*x = WIDTH;
	}
}

t_obj	*copyallobj(t_obj *obj)
{
	t_obj *copy;

	copy = (t_obj*)malloc(sizeof(t_obj));
	copy->id = obj->id;
	//copy->id = obj->id;
	//copy->parent = obj->parent;
	copy->type = obj->type;
	copy->material = obj->material;
	copy->pos = obj->pos;
	copy->v0 = obj->v0;
	copy->v1 = obj->v1;
	copy->v2 = obj->v2;
	copy->dir = obj->dir;
	copy->rad = obj->rad;
	copy->rad2 = obj->rad2;
	copy->height = obj->height;
	copy->alpha = obj->alpha;
	copy->quad = obj->quad;
	copy->isneg = obj->isneg;
	copy->rotation = obj->rotation;
	copy->normobj = NULL;
	if (obj->nextneg)
	{
		copy->nextneg = copyallobj(obj->nextneg);
	}
	else
	{
		copy->nextneg = NULL;
	}
	if (obj->nextslice)
	{
		copy->nextslice = copyallobj(obj->nextslice);
	}
	else
	{
		copy->nextslice = NULL;
	}
	if (obj->nextitem)
	{
		copy->nextitem = copyallobj(obj->nextitem);
	}
	else
	{
		copy->nextitem = NULL;
	}
	return (copy);
}

/*t_cam copycam(t_cam cam)
{
	t_cam camcopy;

	camcopy.xincvector = cam.xincvector;
	camcopy.yincvector = cam.yincvector;
	camcopy.eyepoint = cam.eyepoint;
	camcopy.lookat = cam.lookat;
	camcopy.viewplanebottomleftpoint = cam.viewplanebottomleftpoint;
	camcopy.vdir = cam.vdir;

	return (camcopy);
}*/

// void		effect(t_env *new, t_env *e)
// {
// 	double	tmpred;
// 	double	tmpgreen;
// 	double	tmpblue;
// 	printf("biss     === = =e->effect = %g, e->ered = %g, e->egreen = %g, e->eblue= %g\n", e->effect, e->ered, e->egreen, e->eblue);
//
//
// 	tmpred = new->col.red;
// 	tmpgreen = new->col.green;
// 	tmpblue = new->col.blue;
// 	new->col.red = (tmpred + tmpgreen + tmpblue) * e->ered / 100;
// 	new->col.green = (tmpred + tmpgreen + tmpblue) * e->egreen / 100;
// 	new->col.blue = (tmpred + tmpgreen + tmpblue) * e->eblue / 100;
// }


t_light	*copyalllights(t_light *light)
{
	t_light *copy;

	copy = (t_light*)malloc(sizeof(t_light));

	copy->pos = light->pos;
	copy->intensity = light->intensity;

	if (light->next)
	{
		copy->next = copyalllights(light->next);
	}
	return (copy);
}

void			*cast_ray_thread(void *e)
{
	t_env 	new;
	t_env 	*env;
	int 	interval;

	interval = ((t_thread_task*)e)->i;
	env = ((t_thread_task*)e)->arg;
	//printf("interval = %d\n", interval);

	new.obj = env->obj;
	new.cam = env->cam;
	new.lights = env->lights;
	new.x = 0;
	new.y = 0;
	get_img_pos(&new.x, &new.y, interval);
	//printf("effect %g, red %g , green %g ,  blue %g\n", env->col.effect, env->col.ered, env->col.egreen, env->col.eblue);

//	new.obj = copyallobj(*env->obj);
//	new.cam = env->cam;
//	new.lights = copyalllights(env->lights); // maybe copy a malloced version for each thread?

	while (1)
	{
		reset(&new, new.x, new.y);
		new.col = reflect_and_refract(new);

	//	printf("env->effect = %g, env->ered = %g, env->egreen = %g, env->eblue= %g\n", env->effect, env->ered, env->egreen, env->eblue);
		// if (new.id != -1) //Ambient shading has to take place after every reflection took place
		// {

			//exposure/ saturation
			new.col.red = 1.0 - exp(new.col.red * EXPOSURE);
			new.col.blue = 1.0 - exp(new.col.blue * EXPOSURE);
			new.col.green = 1.0 - exp(new.col.green * EXPOSURE);
		// }


		update_img(&new, new.x, new.y);
		if (new.x >= WIDTH && new.y >= HEIGHT)
		{
		//	pthread_cond_broadcast(&e->cond, &e->mutex);
			break;
		}
		get_img_pos(&new.x, &new.y, interval);
	}
	return (NULL);
}

void env_stereo(t_env *e) {
	e->cam.viewplanebottomleftpoint.x += 20;
}
void env_rev_stereo(t_env *e) {
	e->cam.viewplanebottomleftpoint.x -= 20;
}
void env_anim(t_env *e) {
	e->cam.eyepoint.z += 100;
	e->cam.viewplanebottomleftpoint.z += 100;
}
#include <stdio.h>
#include <stdlib.h>




int				main(int ac, char **av)
{

	t_env			*original;
	t_env			*copy;
	int				fd;
	pthread_t		pth[MAX_THREAD];
	t_thread_task	arg;
	int				i;
	int 			ste;
	printf("enter name\n");



	i = 0;
	if (ac != 2 || (fd = open(av[1], O_RDONLY)) <= -1 || !(original = readConfig(fd)))
	{
		return (0);
	}
	ste = 0;
if (original->effect.stereo)
	ste = 1;
while (original->effect.anim--)
{
	while (1)
	{
		arg.i = 1;
		arg.arg = (void*)original;
		cast_ray_thread((void *)(&arg));
		print_img(&original->effect, update_img(NULL, WIDTH - 1, HEIGHT - 1), original->effect.anim);
			if (!original->effect.stereo)
				break;
			original->effect.stereo = 0;
		env_stereo(original);
	}
	if (ste)
	{
		original->effect.stereo = 1;
		env_rev_stereo(original);
	}
	env_anim(original);
}
	return (0);
}
