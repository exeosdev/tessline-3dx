
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <ts3/core/utility/gdsTypeSupportInternal.h>
#include <ts3/core/utility/gdsTypeSupportStd.h>
#include <ts3/core/utility/gdsCore.h>
#include <ts3/stdext/pathNameIterator.h>
#include <ts3/engine/utility/scfIndexBuilder.h>
#include <ts3/engine/utility/scfIOSupport.h>
#include <ts3/system/sysContextNative.h>

using namespace ts3;

inline const std::string & toString( ESCFEntryType pEntryType )
{
    static const std::unordered_map<ESCFEntryType, std::string> sMap = {
        { ESCFEntryType::Resource, "Resource" },
        { ESCFEntryType::VirtualFolder, "VirtualFolder" }
    };
    return sMap.at( pEntryType );
}

void test1_scf();
void test2_gds();

int main( int, const char ** )
{
    test1_scf();
	return 0;
}

void test1_scf()
{
    system::SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    auto sysContext = system::createSysContext( sysContextCreateInfo );
    auto fileManager = sysContext->createFileManager();

    SCFIOProxy scfIO{ fileManager };

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

        scfIB.addResource( tex_raw, "tex_raw01.raw", SCFInputDataSource::fromFile( fileManager, "smptex.txt" ), "TXR01" );
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
    gds::serialize( binaryBuffer, driverName1 );

    std::wstring driverName2;
    gds::deserialize( binaryBuffer, driverName2 );

    std::vector<int64> vector1{ 4, 5, 6, 7, 8 };
    gds::serialize( binaryBuffer, vector1 );

    std::vector<int64> vector2;
    gds::deserialize( binaryBuffer, vector2 );

    std::string smString = "SampleX1";
    std::vector<std::string> strVec1;
    std::vector<std::string> strVec2;

    for(uint32 i=0; i < 6; ++i){strVec1.push_back( smString );}
    strVec2.resize( strVec1.size() );

    auto strVec1AV = bindArrayView( strVec1.data(), strVec1.size() );
    auto strVec2AV = bindArrayView( strVec2.data(), strVec2.size() );

    gds::serialize( binaryBuffer, strVec1AV );
    gds::deserialize( binaryBuffer, strVec2AV );

    const auto binSize1 = gds::evalByteSize( strVec1AV );
    const auto binSize2 = gds::evalByteSize( strVec2AV );

    std::vector<uint16> u16Vec;
    u16Vec.resize( 32 );

    const auto u16VecS0 = gds::evalByteSize( u16Vec );
    ts3DebugAssert( u16VecS0 == ( u16Vec.size() * sizeof( uint16 ) + sizeof( uint64 ) ) );

    const auto u16VecS1 = gds::evalByteSize( gds::typeCast<uint64>( u16Vec ) );
    ts3DebugAssert( u16VecS1 == ( u16Vec.size() * sizeof( uint64 ) + sizeof( uint64 ) ) );

    std::unordered_map<std::string, std::string> M0;
    M0["Mateusz"] = "Grzejek";
    M0["Magdalena0"] = "Grzejek";
    M0["Magdalena1"] = "Mazur";

    std::unordered_map<std::string, std::string> M1;
    gds::serialize( binaryBuffer, M0 );
    gds::deserialize( binaryBuffer, M1 );
}
