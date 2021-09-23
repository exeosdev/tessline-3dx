
#ifndef __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
#define __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__

#include "fileManager.h"
#include <list>
#include <map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidFileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32FileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11FileManager.h"
#endif

namespace ts3::system
{

    struct FileDeleter
    {
        void operator()( File * pFile ) const
        {
            pFile->mFileManager->_releaseFile( *pFile );
        }
    };

    struct File::ObjectInternalData
    {
        FileNativeData nativeDataPriv;
        file_str_t name;
        file_str_t fullPath;
    };

    struct FileInstance
    {
        File::ObjectInternalData privateData;

        File fileObject;

        std::list<FileInstance>::iterator fileListRef;

        FileInstance( FileManager * pFileManager )
        : fileObject( pFileManager, &privateData )
        {}
    };

    struct FileManager::ObjectInternalData
    {
        using FileList = std::list<FileInstance>;
        using FileMap = std::map<File *, FileInstance *>;

        struct FileRef
        {
            FileInstance * fileInstance = nullptr;
            FileList::iterator listIter;
            FileMap::iterator mapIter;

            explicit operator bool() const
            {
                return fileInstance != nullptr;
            }
        };

        FileList fileList;
        FileMap fileMap;

        FileRef findFile( File * pFile )
        {
            FileRef fileRef;

            auto fileMapIter = fileMap.find( pFile );
            if( fileMapIter != fileMap.end() )
            {
                fileRef.mapIter = fileMapIter;
                fileRef.fileInstance = fileMapIter->second;
                fileRef.listIter = fileMapIter->second->fileListRef;
            }

            return fileRef;
        }
    };

}

#endif //__TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
