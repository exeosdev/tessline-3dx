
cbuffer CB0 : register( b0 )
{
	float4x4 cb0ModelMatrix;
	float4x4 cb0ViewMatrix;
	float4x4 cb0ProjectionMatrix;
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
};

VSOutputData main( VSInputData pVSInput )
{
	float4 vertexPos = float4( pVSInput.vPosition, 1.0f );
	vertexPos = mul( vertexPos, cb0ModelMatrix );
	vertexPos = mul( vertexPos, cb0ViewMatrix );
	vertexPos = mul( vertexPos, cb0ProjectionMatrix );

	VSOutputData vsOutput;
	vsOutput.fragPosition = vertexPos;

	return vsOutput;
}
