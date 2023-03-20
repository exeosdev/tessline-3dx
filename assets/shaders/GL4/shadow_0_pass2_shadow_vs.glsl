
#version 430 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec4 vColor;
layout( location = 2 ) in vec3 vNormal;
layout( location = 3 ) in vec2 vTexCoord0;

layout( std140, binding = 0 ) uniform CB0
{
	layout(row_major) mat4 cb0ModelMatrix;
	layout(row_major) mat4 cb0ViewMatrix;
	layout(row_major) mat4 cb0ProjectionMatrix;
};

layout( std140, binding = 7 ) uniform CBShadow
{
	layout(row_major) mat4 cbsLightSpaceMatrix;
};

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out vec4 psColor;
out vec2 psTexCoord0;
out vec4 psLightSpacePosition;

void main()
{
    vec4 worldPosition = cb0ModelMatrix * vec4( vPosition, 1.0 );
	vec4 lightSpacePosition = cbsLightSpaceMatrix * vec4( worldPosition.xyz, 1.0f );

	psColor = vColor;
	psTexCoord0 = vTexCoord0;
	psLightSpacePosition = lightSpacePosition;

	gl_Position = cb0ProjectionMatrix * cb0ViewMatrix  * worldPosition;
}