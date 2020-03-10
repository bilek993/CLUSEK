#include "ProfilerAnnotations.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"

ProfilerAnnotations::ProfilerAnnotations(ID3D11DeviceContext* context)
{
	const auto hr = context->QueryInterface(__uuidof(UserDefineAnnotationHandler), reinterpret_cast<void**>(UserDefineAnnotationHandler.GetAddressOf()));
	if (FAILED(hr))
		Logger::Error("Failed to create 'UserDefineAnnotationHandler'!");
}

void ProfilerAnnotations::BeginEvent(const std::string& name) const
{
	UserDefineAnnotationHandler->BeginEvent(StringUtil::StringToWide(name).c_str());
}

void ProfilerAnnotations::EndEvent() const
{
	UserDefineAnnotationHandler->EndEvent();
}
