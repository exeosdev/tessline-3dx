
#version 430 core

layout( location = 0 ) in vec3 vPosition;

layout( std140, binding = 7 ) uniform CBLightPass
{
	vec3 cblObjectSpaceLightPos;
	vec3 cblLightDiffuseColor;
	layout(row_major) mat4 cblModelMatrix;
	layout(row_major) mat4 cblSpaceMatrix;
};

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

void main()
{
	gl_Position = cblSpaceMatrix * cblModelMatrix * vec4( vPosition , 1.0 );
}
