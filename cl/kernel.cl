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

kernel void interact(global const float *src, global float *dst, const uint work_size)
{
	const int size = get_global_size(0);
	const int pos = get_global_id(0);
	int i;
	
	if(pos >= work_size)
	{
		return;
	}
	
	Particle p = loadParticle(pos,src);
	
	const float dt = 0.0001;
	const float eps = 1e3;
	const float rm = 0.5;
	const float el = 1e6;
	
	p.pos += dt*p.vel;
	
	/* We use Lennard-Jones potential */
	for(i = 0; i < work_size; ++i)
	{
		if(i == pos) continue;
		Particle p2 = loadParticle(i,src);
		float3 r = p2.pos - p.pos;
		float l2 = dot(r,r)/(rm*rm);
		float l4 = l2*l2;
		float l6 = l2*l4;
		float l8 = l4*l4;
		p.vel += dt*r*eps*12.0*(1.0 - 1.0/l6)/(l8*rm*rm);
	}
	
	/* Boundaries */
	p.vel += dt*el*convert_float3(isgreater(fabs(p.pos), (float3)(1.0)))*(fabs(p.pos) - (float3)(1.0))*sign(p.pos);
	
	storeParticle(&p,pos,dst);
}
