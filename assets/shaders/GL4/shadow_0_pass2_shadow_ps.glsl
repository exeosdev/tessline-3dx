
#version 430 core

in vec4 psColor;
in vec2 psTexCoord0;
in vec4 psLightSpacePosition;

layout( location = 0 ) out vec4 outPixelColor;

layout( binding = 0 ) uniform sampler2D uSampler0;
layout( binding = 7 ) uniform sampler2D uSamplerShadow;

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

float calculateShadowfactor( vec4 pLightSpacePos )
{
	vec3 projCoords = pLightSpacePos.xyz / pLightSpacePos.w;

	vec2 uvCoords;
	uvCoords.x = 0.5 * projCoords.x + 0.5;
	uvCoords.y = 0.5 * projCoords.y + 0.5;

	float zValue = 0.5 * projCoords.z + 0.5;
	float depth = texture( uSamplerShadow, uvCoords ).x;

	const float depthBias = 0.0025;

	if( depth + depthBias < zValue )
	{
		return 0.25f;
	}
	else
	{
		return 1.0f;
	}
}

void main()
{
    vec3 textureColor0;
	if( psColor.a > 0.0f )
	{
		textureColor0 = psColor.rgb;
	}
	else
	{
		textureColor0 = texture( uSampler0, psTexCoord0 ).rgb;
	}

	vec3 fixedLightColor = vec3( 1.0f );
	float shadowFactor = calculateShadowfactor( psLightSpacePosition );

	outPixelColor = vec4( shadowFactor * textureColor0 * fixedLightColor, 1.0 );
}
