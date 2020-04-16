#include "FileDialog.h"
#include "Logger.h"
#include <algorithm>

std::string FileDialog::OpenFile(const std::string& title, const LPCSTR filters)
{
	std::string filename(MAX_PATH, '\0');

	OPENFILENAME openFileName {};
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.lpstrFilter = filters;
	openFileName.lpstrFile = const_cast<char*>(filename.c_str());
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrTitle = title.c_str();
	openFileName.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetOpenFileNameA(&openFileName))
	{
		Logger::Warning("File dialog finished with problems!");
		return "";
	}

	filename.erase(std::remove_if(filename.begin(), filename.end(), IsNullCharacter), filename.end());
	return filename;
}

std::string FileDialog::SaveFile(const std::string& title, const LPCSTR filters)
{
	std::string filename(MAX_PATH, '\0');

	OPENFILENAME openFileName{};
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.lpstrFilter = filters;
	openFileName.lpstrFile = const_cast<char*>(filename.c_str());
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrTitle = title.c_str();
	openFileName.Flags = OFN_DONTADDTORECENT;

	if (!GetSaveFileNameA(&openFileName))
	{
		Logger::Warning("File dialog finished with problems!");
		return "";
	}

	filename.erase(std::remove_if(filename.begin(), filename.end(), IsNullCharacter), filename.end());
	return filename;
}

bool FileDialog::IsNullCharacter(const char character)
{
	return character == '\0';
}
