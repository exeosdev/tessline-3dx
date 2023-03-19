
#version 430 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec4 vColor;
layout( location = 2 ) in vec3 vNormal;
layout( location = 3 ) in vec2 vTexCoord0;

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

layout( std140, binding = 0 ) uniform CB0
{
	layout(row_major) mat4 cb0ModelMatrix;
	layout(row_major) mat4 cb0ViewMatrix;
	layout(row_major) mat4 cb0ProjectionMatrix;
};

layout( std140, binding = 7 ) uniform CBShadow
{
	vec3 cblObjectSpaceLightPos;
	vec3 cblLightDiffuseColor;
	layout(row_major) mat3 cbsNormalMatrix;
	layout(row_major) mat4 cbsShadowMatrix;
};

smooth out vec4 psColor;
smooth out vec2 psTexCoord0;
smooth out vec3 psEyeSpaceNormal;
smooth out vec3 psEyeSpacePosition;
smooth out vec4 psShadowCoords;

void main()
{
	mat4 modelViewMatrix = cb0ViewMatrix * cb0ModelMatrix;
	
	psColor = vColor;
	psTexCoord0 = vTexCoord0;

	psEyeSpacePosition = ( modelViewMatrix * vec4( vPosition, 1.0f ) ).xyz; 

	psEyeSpaceNormal = cbsNormalMatrix * vNormal;

	psShadowCoords = cbsShadowMatrix * ( cb0ModelMatrix * vec4( vPosition, 1.0f ) );

	gl_Position = cb0ProjectionMatrix * cb0ViewMatrix * cb0ModelMatrix * vec4( vPosition , 1.0 );
}
