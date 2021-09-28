
#include <exf/system/session-context.h>
#include <exf/system/native-defs.h>

extern int app_main( int, char **, void * );

int main( int argc, char ** argv )
{

    exf::system::SystemContextCreateInfo sessionCtxCreateInfo {};
    auto systemContext = exf::system::creCreateSystemContext( sessionCtxCreateInfo );

	return app_main( argc, argv, systemContext );
}
