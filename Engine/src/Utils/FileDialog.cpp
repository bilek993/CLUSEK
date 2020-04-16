#include "FileDialog.h"
#include <algorithm>
#include "Logger.h"

std::string FileDialog::OpenFile(const std::string& title, const std::string& filters)
{
	std::string filename(MAX_PATH, '\0');

	OPENFILENAME openFileName {};
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.lpstrFilter = filters.c_str();
	openFileName.lpstrFile = const_cast<char *>(filename.c_str());
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrTitle = title.c_str();
	openFileName.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetOpenFileNameA(&openFileName))
	{
		Logger::Warning("File dialog finished with problems!");
		return "";
	}

	return filename;
}

std::string FileDialog::SaveFile(const std::string& title, const std::string& filters)
{
	std::string filename(MAX_PATH, '\0');

	OPENFILENAME openFileName{};
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.lpstrFilter = filters.c_str();
	openFileName.lpstrFile = const_cast<char *>(filename.c_str());
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrTitle = title.c_str();
	openFileName.Flags = OFN_DONTADDTORECENT;

	if (!GetSaveFileNameA(&openFileName))
	{
		Logger::Warning("File dialog finished with problems!");
		return "";
	}

	return filename;
}
