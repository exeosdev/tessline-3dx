
Texture2D tx0Default : register( t0 );
SamplerState smDefault0 : register( s0 );

struct VSOutputData
{
	float4 psPosition : SV_POSITION;
	float2 psTexCoord0 : TEXCOORD0;
};

float4 main( VSOutputData pPSInput ) : SV_TARGET
{
	return tx0Default.Sample( smDefault0, float2( pPSInput.psTexCoord0.x, 1.0f - pPSInput.psTexCoord0.y ) );
}
