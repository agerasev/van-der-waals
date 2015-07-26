attribute vec3 aVertex;
varying vec2 vCenter;

void main(void)
{
	vCenter = aVertex.xy;
	gl_Position = vec4(vCenter,0.0,1.0);
}
