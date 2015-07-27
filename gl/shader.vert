attribute vec3 aVertex;

varying float vRadius;
varying vec2 vCenter;

void main(void)
{
	vCenter = aVertex.xy;
	vRadius = 8.0/(1.0 + (aVertex.z + 1.0));
	gl_Position = vec4(vCenter,0.0,1.0);
}
