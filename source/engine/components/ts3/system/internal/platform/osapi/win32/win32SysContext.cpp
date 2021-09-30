
#include <ts3/system/sysContextNative.h>
#include <process.h>
#include <tlhelp32.h>
#include <ts3/system/fileCommon.h>
#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    void nativeSysContextInternalInitialize( SysContext & pSysContext, const SysContextCreateInfo & pCreateInfo )
    {
        auto & nativeData = pSysContext.mInternal->nativeDataPriv;
        nativeData.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    }

    void nativeSysContextInternalRelease( SysContext & pSysContext )
    {
        auto & nativeData = pSysContext.mInternal->nativeDataPriv;
        nativeData.appExecModuleHandle = nullptr;
    }


    std::string SysContext::queryCurrentProcessExecutableFilePath()
    {
        std::string executableFilePath;

        DWORD validProcessID = 0;
        DWORD currentProcessID = getpid();
        HANDLE moduleSnapshotHandle = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, currentProcessID );

        MODULEENTRY32 moduleEntryInfo;
        moduleEntryInfo.dwSize = sizeof( MODULEENTRY32 );

        if( Module32First( moduleSnapshotHandle, &moduleEntryInfo ) )
        {
            do
            {
                if( moduleEntryInfo.th32ProcessID == currentProcessID )
                {
                    validProcessID = currentProcessID;
                    break;
                }
            }
            while( Module32Next( moduleSnapshotHandle, &moduleEntryInfo ) );
        }

        CloseHandle( moduleSnapshotHandle );

        if( validProcessID != 0 )
        {
            executableFilePath = moduleEntryInfo.szExePath;
        }

        return executableFilePath;
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
