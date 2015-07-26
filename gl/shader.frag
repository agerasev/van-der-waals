varying vec2 vCenter;

void main(void)
{
	vec4 color = vec4(0.0,0.0,0.0,0.0);
	if(length(2.0*gl_FragCoord.xy - (vec2(1.0,1.0) + vCenter)*vec2(600.0,600.0)) < 16.0)
		color = vec4(1.0,1.0,1.0,1.0);
	gl_FragColor = color;
}
