
Texture2D txTexture0 : register( t0 );

SamplerState smSampler0 : register( s0 );

struct VSOutputData
{
	float4 psPosition : SV_POSITION;
	float2 psTexCoord0 : TEXCOORD0;
};

float4 main( VSOutputData pPSInput ) : SV_TARGET
{
	return txTexture0.Sample( smSampler0, pPSInput.psTexCoord0 );
}
