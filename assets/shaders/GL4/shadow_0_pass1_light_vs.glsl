
#version 430 core

layout( location = 0 ) in vec3 vPosition;

layout( std140, binding = 0 ) uniform CB0
{
	layout(row_major) mat4 cb0ModelMatrix;
	layout(row_major) mat4 cb0ViewMatrix;
	layout(row_major) mat4 cb0ProjectionMatrix;
};

layout( std140, binding = 7 ) uniform CBShadow
{
	layout(row_major) mat4 cbsLightSpaceMatrix;
	vec2 cbsShadowMapSize;
};

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

void main()
{
	gl_Position = cb0ProjectionMatrix * cb0ViewMatrix * cb0ModelMatrix * vec4( vPosition , 1.0 );
}
