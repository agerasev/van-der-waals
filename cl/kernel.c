#ifndef __OPENCL_VERSION__
#include "opencl.h"
#endif // __OPENCL_VERSION__

typedef struct
{
	float3 pos;
	float3 vel;
} 
Particle;

Particle loadParticle(int pos, global const float *ptr)
{
	Particle p;
	p.pos = vload3(2*pos, ptr);
	p.vel = vload3(2*pos+1, ptr);
	return p;
}

void storeParticle(const Particle *p, int pos, global float *ptr)
{
	vstore3(p->pos, 2*pos, ptr);
	vstore3(p->vel, 2*pos + 1, ptr);
}

kernel void interact(global float *one, global float *two, const uint work_size)
{
	const int size = get_global_size(0);
	const int pos = get_global_id(0);
	int i, s;
	
	const float dt = 0.00001; // time step
	const float in = 1e1;     // interaction magnitude
	const float rm = 0.2;     // equillibrium distance
	const float el = 1e6;     // boundaries elasticity
	const float em = 1e-4;    // emission coefficient
	const float ms = 100.0;    // maximal speed
	
	global float *src = one;
	global float *dst = two;
	global float *tmp;
	
	Particle p;
	if(pos < work_size)
		p = loadParticle(pos,src);
	
	for(s = 0; s < 200; ++s)
	{
		barrier(CLK_GLOBAL_MEM_FENCE);
		
		if(pos >= work_size)
			continue;
		
		float3 acc = (float3)(0.0);
		float3 frc;
		/* We use Lennard-Jones potential */
		for(i = 0; i < work_size; ++i)
		{
			if(i == pos) 
				continue;
			Particle p2 = loadParticle(i, src);
			float3 r = p2.pos - p.pos;
			float l2 = dot(r,r)/(rm*rm);
			if(l2 < 0.6)
				continue;
			float l4 = l2*l2;
			float l6 = l2*l4;
			float l8 = l4*l4;
			frc = r*in*12.0*(1.0 - 1.0/l6)/(l8*rm*rm);
			acc += frc*(1.0 - em*dot(frc, p.vel - p2.vel));
		}
		
		/* Boundaries */
		frc = el*convert_float3(isgreater(fabs(p.pos), (float3)(1.0)))*(fabs(p.pos) - (float3)(1.0))*sign(p.pos);
		acc += frc*(1.0 - em*dot(frc, p.vel));
		
		p.vel += dt*acc;
		float spd = length(p.vel);
		if(spd > ms)
			p.vel *= ms/spd;
		p.pos += dt*p.vel;
		
		storeParticle(&p,pos,dst);
		
		tmp = src;
		src = dst;
		dst = tmp;
	}
}
