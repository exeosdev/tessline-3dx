
#version 430 core

layout( location = 0 ) in vec3 vPosition;

layout( std140, binding = 7 ) uniform CBShadowPass1
{
	layout(row_major) mat4 cbs1ModelMatrix;
	layout(row_major) mat4 cbs1LightSpaceMatrix;
};

out gl_PerVertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

void main()
{
	gl_Position = cbs1LightSpaceMatrix * cbs1ModelMatrix * vec4( vPosition , 1.0 );
}
