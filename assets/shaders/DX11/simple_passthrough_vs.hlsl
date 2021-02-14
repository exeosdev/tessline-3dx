
cbuffer CB0 : register( b0 )
{
	float4 scolor;
	float4x4 cb0ModelMatrix;
	float4x4 cb0ViewMatrix;
	float4x4 cb0ProjectionMatrix;
};

struct VSInputData
{
	float4 vPosition : POSITION;
	float2 vTexCoord0 : TEXCOORD0;
};

struct VSOutputData
{
	float4 psPosition : SV_POSITION;
	float2 psTexCoord0 : TEXCOORD0;
};

VSOutputData main( VSInputData pVSInput )
{
	float4 vertexPos = float4( pVSInput.vPosition );
	vertexPos = mul( vertexPos, cb0ModelMatrix );
	vertexPos = mul( vertexPos, cb0ViewMatrix );
	vertexPos = mul( vertexPos, cb0ProjectionMatrix );
	
	VSOutputData vsOutput;
	vsOutput.psPosition = vertexPos;
	vsOutput.psTexCoord0 = pVSInput.vTexCoord0;
	
	return vsOutput;
}
