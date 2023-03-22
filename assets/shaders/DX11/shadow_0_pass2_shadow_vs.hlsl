
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

struct VSInputData
{
	float3 vPosition : POSITION;
	float4 vColor : COLOR;
	float3 vNormal : NORMAL;
	float2 vTexCoord0 : TEXCOORD0;
};

struct VSOutputData
{
	float4 fragPosition : SV_POSITION;
	float4 fragColor : COLOR;
	float2 texCoord : TEXCOORD0;
	float4 lightSpacePosition : LSPOS;
};

VSOutputData main( VSInputData pVSInput )
{
	float4 vertexPos = float4( pVSInput.vPosition, 1.0f );
	float4 worldPosition = mul( float4( vertexPos.xyz, 1.0f ), cb0ModelMatrix );
	float4 lightSpacePosition = mul( worldPosition, cbsLightSpaceMatrix );

	VSOutputData vsOutput;
	vsOutput.fragPosition = mul( mul( worldPosition, cb0ViewMatrix ), cb0ProjectionMatrix );
	vsOutput.fragColor = pVSInput.vColor;
	vsOutput.texCoord = pVSInput.vTexCoord0;
	vsOutput.lightSpacePosition = lightSpacePosition;

	return vsOutput;
}
