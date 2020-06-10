#include "ImGuiExtensions.h"
#include <string>

int ImGuiExtensions::InputTextStringCallback(ImGuiInputTextCallbackData* data)
{
	const auto textPointer = static_cast<std::string*>(data->UserData);
	textPointer->resize(data->BufTextLen);
	data->Buf = const_cast<char*>(textPointer->c_str());
	return 0;
}
