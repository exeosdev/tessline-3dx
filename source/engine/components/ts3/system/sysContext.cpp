
#include <ts3/system/sysContext.h>

namespace ts3::system
{

	SysContext::SysContext() = default;

	SysContext::~SysContext() noexcept = default;

	std::string SysContext::queryCurrentProcessExecutableDirectory() const
	{
		auto executableFileDirectory = queryCurrentProcessExecutableFilePath();

		if( !executableFileDirectory.empty() )
		{
			std::replace( executableFileDirectory.begin(),
			              executableFileDirectory.end(),
			              TS3_PCL_ENV_NON_STANDARD_PATH_DELIMITER,
			              TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );

			const auto lastStringPosition = executableFileDirectory.length() - 1;
			const auto lastSeparatorPos = executableFileDirectory.find_last_of( TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );

			if( ( lastSeparatorPos > 0 ) && ( lastSeparatorPos < lastStringPosition ) )
			{
				executableFileDirectory.resize( lastSeparatorPos );
			}
		}

		return executableFileDirectory;
	}

} // namespace ts3::system
