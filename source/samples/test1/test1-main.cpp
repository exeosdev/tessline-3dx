
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <ts3/core/utility/gdsCore.h>
#include <ts3/core/reflection/enumTypeInfo.h>
#include <ts3/gpuapi/prerequisites.h>
#include <ts3/stdext/pathNameIterator.h>
#include <ts3/engine/utility/scfIndexBuilder.h>
#include <ts3/engine/utility/scfIOSupport.h>
#include <ts3/engine/utility/scfXMLReader.h>
#include <ts3/system/sysContextNative.h>

using namespace ts3;

void test1_scf( system::FileManagerHandle pSysFileManager );
void test2_gds();

int main( int, const char ** )
{
	system::SysContextCreateInfo sysContextCreateInfo;
	sysContextCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
	auto sysContext = system::createSysContext( sysContextCreateInfo );
	auto fileManager = sysContext->createFileManager();

	auto & tpi = gpuapi::queryEnumTypeInfo<gpuapi::EVertexAttribFormat>();
	const auto & cname = tpi.getConstantName( gpuapi::EVertexAttribFormat::VEC4_SBYTE_NORM );

	SCFXMLReader xmlResReader{ fileManager };
	auto rootNode = xmlResReader.readFile( "C:\\Repo\\Exeos\\tessline-3dx\\Resources.xml" );
	auto nodes = rootNode.getNodeList( true );

	const auto & resnodes = rootNode.subFolder( "fonts" )->getResourceNodes();
	const auto & res = resnodes[0];

	const auto type = res.attribute( "type" );
	const auto infonode = res.dataNode( "info" );
	const auto texdimnode = infonode.firstSubNode( "textureDimensions" );

	const auto texdimvalx = texdimnode.firstSubNode("width").value();
	const auto texdimvaly = texdimnode.firstSubNode("height").value();

	for( auto * node : nodes )
	{
		std::cout << node->nodeName() << " (value = '" << node->nodeTextValue() << "')\n";
		std::flush( std::cout );
	}

//	for( auto * R = xmlResReader.findNextResource(); R != nullptr; )
//	{
//		auto * N = R->first_node("info")->first_node("fontID");
//		std::cout << std::string( N->value(), N->value_size() ) << "\n";
//		std::flush( std::cout );
//		R = xmlResReader.findNextResource();
//	}

	test1_scf( fileManager );

	return 0;
}

void test1_scf( system::FileManagerHandle pSysFileManager )
{

	SCFIOProxy scfIO{ pSysFileManager };

    SCFIndexBuilder scfIB;
    {
        scfIB.addResource( nullptr, "x_common01.res", SCFInputDataSource::asPlaceholder( 64 ), "XRES1" );
        scfIB.addResource( nullptr, "x_common02.res", SCFInputDataSource::asPlaceholder( 64 ), "XRES2" );

        auto * tex = scfIB.addFolder( nullptr, "tex" );

        scfIB.addResource( tex, "tex_common01.dds", SCFInputDataSource::asPlaceholder( 64 ), "TXC01" );
        scfIB.addResource( tex, "tex_common02.dds", SCFInputDataSource::asPlaceholder( 64 ), "TXC02" );
        scfIB.addResource( tex, "tex_common03.dds", SCFInputDataSource::asPlaceholder( 64 ), "TXC03" );
        scfIB.addResource( tex, "tex_common04.dds", SCFInputDataSource::asPlaceholder( 64 ), "TXC04" );

        auto * tex_raw = scfIB.addFolder( tex, "raw" );

        scfIB.addResource( tex_raw, "tex_raw01.raw", SCFInputDataSource::fromFile( pSysFileManager, "smptex.txt" ), "TXR01" );
        scfIB.addResource( tex_raw, "tex_raw02.raw", SCFInputDataSource::asPlaceholder( 64 ), "TXR02" );

        scfIO.saveIndex( "scf_sample_index.scf", scfIB );
    }

    SCFIndex scfIndex;
    {
        scfIO.loadIndex( "scf_sample_index.scf", scfIndex );
    }

    const auto L = scfIndex.enumerateEntries();

    std::string tabs;
    for( const auto & E : L )
    {
        tabs.assign( E->mInfo->treeSubLevel - 1, '\t'  );
        std::cout << tabs << E->mInfo->path << " [" << toString( E->mInfo->entryType ) << "]\n";
    }
    std::flush( std::cout );

    auto & texture = scfIndex.getEntryByPath( "/tex/raw/tex_raw01.raw" );
    auto & texResource = texture.asResource();

    std::string textureData{};
    texResource.readData( textureData );

    return;
}

// MOESIF

void test2_gds()
{
    const std::wstring driverName1 = L"GL4";
    byte binaryBuffer[512];
    GdsCore::serialize( binaryBuffer, driverName1 );

    std::wstring driverName2;
    GdsCore::deserialize( binaryBuffer, driverName2 );

    std::vector<int64> vector1{ 4, 5, 6, 7, 8 };
    GdsCore::serialize( binaryBuffer, vector1 );

    std::vector<int64> vector2;
    GdsCore::deserialize( binaryBuffer, vector2 );

    std::string smString = "SampleX1";
    std::vector<std::string> strVec1;
    std::vector<std::string> strVec2;

    for(uint32 i=0; i < 6; ++i){strVec1.push_back( smString );}
    strVec2.resize( strVec1.size() );

    auto strVec1AV = bindArrayView( strVec1.data(), strVec1.size() );
    auto strVec2AV = bindArrayView( strVec2.data(), strVec2.size() );

    GdsCore::serialize( binaryBuffer, strVec1AV );
    GdsCore::deserialize( binaryBuffer, strVec2AV );

    const auto binSize1 = GdsCore::evalByteSize( strVec1AV );
    const auto binSize2 = GdsCore::evalByteSize( strVec2AV );

    std::vector<uint16> u16Vec;
    u16Vec.resize( 32 );

    //const auto u16VecS0 = GdsCore::evalByteSize( u16Vec );
    //ts3DebugAssert( u16VecS0 == ( u16Vec.size() * sizeof( uint16 ) + sizeof( uint64 ) ) );

    //const auto u16VecS1 = GdsCore::evalByteSize( gds::typeCast<uint64>( u16Vec ) );
    //ts3DebugAssert( u16VecS1 == ( u16Vec.size() * sizeof( uint64 ) + sizeof( uint64 ) ) );

    std::unordered_map<std::string, std::string> M0;
    M0["Mateusz"] = "Grzejek";
    M0["Magdalena0"] = "Grzejek";
    M0["Magdalena1"] = "Mazur";

    std::unordered_map<std::string, std::string> M1;
    GdsCore::serialize( binaryBuffer, M0 );
    GdsCore::deserialize( binaryBuffer, M1 );
}
