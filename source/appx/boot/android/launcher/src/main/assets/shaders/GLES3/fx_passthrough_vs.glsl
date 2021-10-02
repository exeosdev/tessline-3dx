#version 310 es

layout( std140, binding = 0 ) uniform CB0
{
	vec4 scolor;
	layout(row_major) mat4 cb0ModelMatrix;
	layout(row_major) mat4 cb0ViewMatrix;
	layout(row_major) mat4 cb0ProjectionMatrix;
};

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 vTexCoord0;
layout( location = 2 ) in vec4 vStaticColor;

out smooth vec2 psTexCoord0;

void main()
{
	psTexCoord0 = vTexCoord0;
	gl_Position = cb0ProjectionMatrix * cb0ViewMatrix * cb0ModelMatrix * vec4( vPosition , 1.0 );
}