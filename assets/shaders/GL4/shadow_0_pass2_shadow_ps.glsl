
#version 430 core

smooth in vec4 psColor;
smooth in vec2 psTexCoord0;
smooth in vec3 psEyeSpaceNormal;
smooth in vec3 psEyeSpacePosition;
smooth in vec4 psShadowCoords;

layout( location = 0 ) out vec4 outPixelColor;

layout( binding = 0 ) uniform sampler2D uSampler0;
layout( binding = 7 ) uniform sampler2DShadow uSamplerShadow;

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

const float k0 = 1.0;
const float k1 = 0.0;
const float k2 = 0.0;

void main()
{
	mat4 modelViewMatrix = cb0ViewMatrix * cb0ModelMatrix;

	vec4 vEyeSpaceLightPosition = modelViewMatrix * vec4( cblObjectSpaceLightPos, 1.0f );

	vec3 L = ( vEyeSpaceLightPosition.xyz - psEyeSpacePosition );

	float D = length( L );

	L = normalize( L );
	
	float attenuationAmount = 1.0 / ( k0 + ( k1 * D ) + ( k2 * D * D ) );

	float diffuse = max( 0, dot( psEyeSpaceNormal, L ) ) * attenuationAmount;

	if( psShadowCoords.w > 1 ) 
	{
		float shadow = textureProj( uSamplerShadow, psShadowCoords );
		diffuse = mix( diffuse, diffuse * shadow, 0.5 ); 
	}

	if( psColor.a > 0.0f )
	{
		outPixelColor = psColor;
	}
	else
	{
		vec4 textureColor = texture( uSampler0, psTexCoord0 );
		outPixelColor = diffuse * textureColor;
	}
}
