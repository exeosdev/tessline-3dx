
//#include <ts3/math/vectorTypes.h>
//#include <ts3/math/matrixTypes.h>
//#include <ts3/math/matrixOps.h>
//#include <ts3/math/matrixUtils.h>
//#include <ts3/math/matrixUtilsSIMD.h>

#include <ts3/math/vectorOps.h>
#include <ts3/math/matrixUtils.h>

#include <ts3/gpuapi/state/inputAssemblerVertexFormat.h>
#include <ts3/gpuapi/state/graphicsPipelineStateCommon.h>

namespace math = ts3::math;

int main( int argc, char ** argv )
{
	const auto ss1 = sizeof(ts3::gpuapi::BlendConfig);
	const auto ss2 = sizeof(ts3::gpuapi::DepthStencilConfig);

	using Vec4f = math::Vector4<float>;
	using Mat2f = math::Matrix<float,2,2>;
	using Mat4f = math::Matrix4xC<float,4>;

	Mat2f m20;
	Mat4f m40;

	Vec4f vec4{ 0,1,2,3 };
	auto v4n = math::normalize( vec4 );

	Mat2f mat21{ 1.0f, 2.0f,
	             3.0f, 4.0f };
	Mat2f mat22{ 18.0f, 17.0f,
	             6.0f, 5.0f };
	Mat2f mat23{ 1.0f, 2.0f,
	             3.0f, 4.0f };
	Mat2f mat24{ 5.0f, 6.0f,
	             7.0f, 8.0f };

	Mat2f res1 = math::mul( mat21, mat22 );
	Mat2f res2 = math::mul( mat23, mat24 );

	auto sum1 = math::add( mat21, mat21 );
	auto diff1 = math::sub( sum1, mat21 );

	Mat4f mat4F{
		0, 1, 2, 3,
		10, 11, 12, 13,
		20, 21, 22, 23,
		30, 31, 32, 33
	};

	const auto ps = sizeof( Mat4f );

	math::transposeInplace( mat4F );

	return 0;
}
