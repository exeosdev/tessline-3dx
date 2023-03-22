
struct VSOutputData
{
	float4 fragPosition : SV_POSITION;
};

struct PSOutputData
{
	float fragDepth : SV_Depth;
};

PSOutputData main( VSOutputData pPSInput )
{
	PSOutputData psOut;
	psOut.fragDepth = pPSInput.fragPosition.z;

	return psOut;
}
