
#pragma once

#ifndef __TS3_ENGINE_SCF_INDEX_H__
#define __TS3_ENGINE_SCF_INDEX_H__

#include "scfEntry.h"

namespace ts3
{

	class SCFIndex : public SCFVirtualFolder
	{
		friend class SCFIndexBuilder;

	public:
		SCFIndex() = default;
		~SCFIndex() = default;

	};

}

#endif // __TS3_ENGINE_SCF_INDEX_H__
