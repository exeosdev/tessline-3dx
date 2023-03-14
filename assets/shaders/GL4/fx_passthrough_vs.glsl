
#version 430 core

layout( std140, binding = 0 ) uniform CB0
{
	vec4 scolor;
	layout(row_major) mat4 cb0ModelMatrix;
	layout(row_major) mat4 cb0ViewMatrix;
	layout(row_major) mat4 cb0ProjectionMatrix;
};

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 vTexCoord0;

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out smooth vec2 psTexCoord0;

void main()
{
	psTexCoord0 = vTexCoord0;
	gl_Position = cb0ProjectionMatrix * cb0ViewMatrix * cb0ModelMatrix * vec4( vPosition , 1.0 );
}
