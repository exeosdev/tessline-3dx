
#include "posixCommon.h"
#include <cerrno>

namespace ts3::system
{

    namespace platform
    {

        const char * posixQueryErrnoStringByCode( int pErrno )
        {
            switch( pErrno )
            {
                ts3CaseReturn( EPERM           , "Operation not permitted" );
                ts3CaseReturn( ENOENT          , "No such file or directory" );
                ts3CaseReturn( ESRCH           , "No such process" );
                ts3CaseReturn( EINTR           , "Interrupted system call" );
                ts3CaseReturn( EIO             , "Input/output error" );
                ts3CaseReturn( ENXIO           , "No such device or address" );
                ts3CaseReturn( E2BIG           , "Argument list too long" );
                ts3CaseReturn( ENOEXEC         , "Exec format error" );
                ts3CaseReturn( EBADF           , "Bad file descriptor" );
                ts3CaseReturn( ECHILD          , "No child processes" );
                ts3CaseReturn( EAGAIN          , "Resource temporarily unavailable" );
                ts3CaseReturn( ENOMEM          , "Cannot allocate memory" );
                ts3CaseReturn( EACCES          , "Permission denied" );
                ts3CaseReturn( EFAULT          , "Bad address" );
                ts3CaseReturn( ENOTBLK         , "Block device required" );
                ts3CaseReturn( EBUSY           , "Device or resource busy" );
                ts3CaseReturn( EEXIST          , "File exists" );
                ts3CaseReturn( EXDEV           , "Invalid cross-device link" );
                ts3CaseReturn( ENODEV          , "No such device" );
                ts3CaseReturn( ENOTDIR         , "Not a directory" );
                ts3CaseReturn( EISDIR          , "Is a directory" );
                ts3CaseReturn( EINVAL          , "Invalid argument" );
                ts3CaseReturn( ENFILE          , "Too many open files in system" );
                ts3CaseReturn( EMFILE          , "Too many open files" );
                ts3CaseReturn( ENOTTY          , "Inappropriate ioctl for device" );
                ts3CaseReturn( ETXTBSY         , "Text file busy" );
                ts3CaseReturn( EFBIG           , "File too large" );
                ts3CaseReturn( ENOSPC          , "No space left on device" );
                ts3CaseReturn( ESPIPE          , "Illegal seek" );
                ts3CaseReturn( EROFS           , "Read-only file system" );
                ts3CaseReturn( EMLINK          , "Too many links" );
                ts3CaseReturn( EPIPE           , "Broken pipe" );
                ts3CaseReturn( EDOM            , "Numerical argument out of domain" );
                ts3CaseReturn( ERANGE          , "Numerical result out of range" );
                ts3CaseReturn( EDEADLOCK       , "Resource deadlock avoided" );
                ts3CaseReturn( ENAMETOOLONG    , "File name too long" );
                ts3CaseReturn( ENOLCK          , "No locks available" );
                ts3CaseReturn( ENOSYS          , "Function not implemented" );
                ts3CaseReturn( ENOTEMPTY       , "Directory not empty" );
                ts3CaseReturn( ELOOP           , "Too many levels of symbolic links" );
                ts3CaseReturn( ENOMSG          , "No message of desired type" );
                ts3CaseReturn( EIDRM           , "Identifier removed" );
                ts3CaseReturn( ECHRNG          , "Channel number out of range" );
                ts3CaseReturn( EL2NSYNC        , "Level 2 not synchronized" );
                ts3CaseReturn( EL3HLT          , "Level 3 halted" );
                ts3CaseReturn( EL3RST          , "Level 3 reset" );
                ts3CaseReturn( ELNRNG          , "Link number out of range" );
                ts3CaseReturn( EUNATCH         , "Protocol driver not attached" );
                ts3CaseReturn( ENOCSI          , "No CSI structure available" );
                ts3CaseReturn( EL2HLT          , "Level 2 halted" );
                ts3CaseReturn( EBADE           , "Invalid exchange" );
                ts3CaseReturn( EBADR           , "Invalid request descriptor" );
                ts3CaseReturn( EXFULL          , "Exchange full" );
                ts3CaseReturn( ENOANO          , "No anode" );
                ts3CaseReturn( EBADRQC         , "Invalid request code" );
                ts3CaseReturn( EBADSLT         , "Invalid slot" );
                ts3CaseReturn( EBFONT          , "Bad font file format" );
                ts3CaseReturn( ENOSTR          , "Device not a stream" );
                ts3CaseReturn( ENODATA         , "No data available" );
                ts3CaseReturn( ETIME           , "Timer expired" );
                ts3CaseReturn( ENOSR           , "Out of streams resources" );
                ts3CaseReturn( ENONET          , "Machine is not on the network" );
                ts3CaseReturn( ENOPKG          , "Package not installed" );
                ts3CaseReturn( EREMOTE         , "Object is remote" );
                ts3CaseReturn( ENOLINK         , "Link has been severed" );
                ts3CaseReturn( EADV            , "Advertise error" );
                ts3CaseReturn( ESRMNT          , "Srmount error" );
                ts3CaseReturn( ECOMM           , "Communication error on send" );
                ts3CaseReturn( EPROTO          , "Protocol error" );
                ts3CaseReturn( EMULTIHOP       , "Multihop attempted" );
                ts3CaseReturn( EDOTDOT         , "RFS specific error" );
                ts3CaseReturn( EBADMSG         , "Bad message" );
                ts3CaseReturn( EOVERFLOW       , "Value too large for defined data type" );
                ts3CaseReturn( ENOTUNIQ        , "Name not unique on network" );
                ts3CaseReturn( EBADFD          , "File descriptor in bad state" );
                ts3CaseReturn( EREMCHG         , "Remote address changed" );
                ts3CaseReturn( ELIBACC         , "Can not access a needed shared library" );
                ts3CaseReturn( ELIBBAD         , "Accessing a corrupted shared library" );
                ts3CaseReturn( ELIBSCN         , ".lib section in a.out corrupted" );
                ts3CaseReturn( ELIBMAX         , "Attempting to link in too many shared libraries" );
                ts3CaseReturn( ELIBEXEC        , "Cannot exec a shared library directly" );
                ts3CaseReturn( EILSEQ          , "Invalid or incomplete multibyte or wide character" );
                ts3CaseReturn( ERESTART        , "Interrupted system call should be restarted" );
                ts3CaseReturn( ESTRPIPE        , "Streams pipe error" );
                ts3CaseReturn( EUSERS          , "Too many users" );
                ts3CaseReturn( ENOTSOCK        , "Socket operation on non-socket" );
                ts3CaseReturn( EDESTADDRREQ    , "Destination address required" );
                ts3CaseReturn( EMSGSIZE        , "Message too long" );
                ts3CaseReturn( EPROTOTYPE      , "Protocol wrong type for socket" );
                ts3CaseReturn( ENOPROTOOPT     , "Protocol not available" );
                ts3CaseReturn( EPROTONOSUPPORT , "Protocol not supported" );
                ts3CaseReturn( ESOCKTNOSUPPORT , "Socket type not supported" );
                ts3CaseReturn( ENOTSUP         , "Operation not supported" );
                ts3CaseReturn( EPFNOSUPPORT    , "Protocol family not supported" );
                ts3CaseReturn( EAFNOSUPPORT    , "Address family not supported by protocol" );
                ts3CaseReturn( EADDRINUSE      , "Address already in use" );
                ts3CaseReturn( EADDRNOTAVAIL   , "Cannot assign requested address" );
                ts3CaseReturn( ENETDOWN        , "Network is down" );
                ts3CaseReturn( ENETUNREACH     , "Network is unreachable" );
                ts3CaseReturn( ENETRESET       , "Network dropped connection on reset" );
                ts3CaseReturn( ECONNABORTED    , "Software caused connection abort" );
                ts3CaseReturn( ECONNRESET      , "Connection reset by peer" );
                ts3CaseReturn( ENOBUFS         , "No buffer space available" );
                ts3CaseReturn( EISCONN         , "Transport endpoint is already connected" );
                ts3CaseReturn( ENOTCONN        , "Transport endpoint is not connected" );
                ts3CaseReturn( ESHUTDOWN       , "Cannot send after transport endpoint shutdown" );
                ts3CaseReturn( ETOOMANYREFS    , "Too many references: cannot splice" );
                ts3CaseReturn( ETIMEDOUT       , "Connection timed out" );
                ts3CaseReturn( ECONNREFUSED    , "Connection refused" );
                ts3CaseReturn( EHOSTDOWN       , "Host is down" );
                ts3CaseReturn( EHOSTUNREACH    , "No route to host" );
                ts3CaseReturn( EALREADY        , "Operation already in progress" );
                ts3CaseReturn( EINPROGRESS     , "Operation now in progress" );
                ts3CaseReturn( ESTALE          , "Stale file handle" );
                ts3CaseReturn( EUCLEAN         , "Structure needs cleaning" );
                ts3CaseReturn( ENOTNAM         , "Not a XENIX named type file" );
                ts3CaseReturn( ENAVAIL         , "No XENIX semaphores available" );
                ts3CaseReturn( EISNAM          , "Is a named type file" );
                ts3CaseReturn( EREMOTEIO       , "Remote I/O error" );
                ts3CaseReturn( EDQUOT          , "Disk quota exceeded" );
                ts3CaseReturn( ENOMEDIUM       , "No medium found" );
                ts3CaseReturn( EMEDIUMTYPE     , "Wrong medium type" );
                ts3CaseReturn( ECANCELED       , "Operation canceled" );
                ts3CaseReturn( ENOKEY          , "Required key not available" );
                ts3CaseReturn( EKEYEXPIRED     , "Key has expired" );
                ts3CaseReturn( EKEYREVOKED     , "Key has been revoked" );
                ts3CaseReturn( EKEYREJECTED    , "Key was rejected by service" );
                ts3CaseReturn( EOWNERDEAD      , "Owner died" );
                ts3CaseReturn( ENOTRECOVERABLE , "State not recoverable" );
                ts3CaseReturn( ERFKILL         , "Operation not possible due to RF-kill" );
                ts3CaseReturn( EHWPOISON       , "Memory page has hardware error" );
            }
            return "UNKNOWN_ERROR";
        }

    }

}