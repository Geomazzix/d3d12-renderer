#include "Utility/D3DErrorHandler.h"
#include <cstdio>

namespace mtd3d
{
	D3DErrorHandler::D3DErrorHandler()
	{
#if defined (DEBUG) || defined (_DEBUG)
		D3D_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugInterface)));
		m_DebugInterface->EnableDebugLayer();

		//For "Device Removed Extended Data" - debug logging.
		D3D_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DredSettings)));
		m_DredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		m_DredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
#endif
	}

	void D3DErrorHandler::CheckD3DError(long hr, const char* method, const std::string& file, int lineNumber)
	{
		if (FAILED(hr))
		{
			printf("D3D12 error in %s on line number %i in '%s'.\n", file.c_str(), lineNumber, method);
			__debugbreak();
		}
	}
}