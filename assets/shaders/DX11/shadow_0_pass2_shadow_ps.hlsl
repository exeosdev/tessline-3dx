
Texture2D txTexture0 : register( t0 );
Texture2D txTextureShadow : register( t7 );

SamplerState smSampler0 : register( s0 );
SamplerState smSamplerShadow : register( s7 );

cbuffer CB0 : register( b0 )
{
	float4x4 cb0ModelMatrix;
	float4x4 cb0ViewMatrix;
	float4x4 cb0ProjectionMatrix;
};

cbuffer CBShadow : register( b7 )
{
	float4x4 cbsLightSpaceMatrix;
	float4 cbsShadowProperties;
};

struct VSOutputData
{
	float4 fragPosition : SV_POSITION;
	float4 fragColor : COLOR;
	float2 texCoord : TEXCOORD0;
	float4 lightSpacePosition : LSPOS;
};

float calculateShadowfactor( float4 pLightSpacePos )
{
	float3 projCoords = pLightSpacePos.xyz / pLightSpacePos.w;

	float2 uvCoords;
	uvCoords.x = 0.5 * projCoords.x + 0.5;
	uvCoords.y = -0.5 * projCoords.y + 0.5;

	float zValue = projCoords.z;
	float depth = txTextureShadow.Sample( smSamplerShadow, uvCoords ).r;

	const float depthBias = 0.00015;

	if( depth + depthBias < zValue )
	{
		return 0.5f;
	}
	else
	{
		return 1.0f;
	}
}

float4 main( VSOutputData pPSInput ) : SV_TARGET
{
	float3 textureColor0;
	if( pPSInput.fragColor.a > 0.0f )
	{
		textureColor0 = pPSInput.fragColor.rgb;
	}
	else
	{
		textureColor0 = txTexture0.Sample( smSampler0, pPSInput.texCoord ).rgb;
	}

	float3 fixedLightColor = float3( 1.0f, 1.0f, 1.0f );
	float shadowFactor = calculateShadowfactor( pPSInput.lightSpacePosition );

	return float4( shadowFactor * textureColor0 * fixedLightColor, 1.0 );
}
