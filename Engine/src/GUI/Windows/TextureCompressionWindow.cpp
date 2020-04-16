#include "TextureCompressionWindow.h"
#include <imgui.h>
#include "../../Utils/FileDialog.h"

void TextureCompressionWindow::Draw()
{
	ImGui::Begin("Texture compression", &IsEnabled);

	if (!LoadingInProgress)
		DrawMain();
	else
		DrawLoading();

	ImGui::End();
}

void TextureCompressionWindow::DrawLoading() const
{
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 31);
	ImGui::SetCursorPosY((ImGui::GetWindowSize().y / 2));
	ImGui::Text("LOADING %c", "|/-\\"[static_cast<int>(ImGui::GetTime() / 0.15f) & 3]);
}

void TextureCompressionWindow::DrawMain()
{
	
	DrawFileSelection();

	if (ImGui::TreeNode("Aaaaa"))
	{
		ImGui::TreePop();
	}

	if (ImGui::Button("Convert"))
	{
	}
}

void TextureCompressionWindow::DrawFileSelection()
{
	const auto callbackText = [](ImGuiInputTextCallbackData *data)
	{
		const auto textPointer = static_cast<std::string*>(data->UserData);
		textPointer->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(textPointer->c_str());
		return 0;
	};

	if (ImGui::Button("SELECT##Input_File"))
		InputFilePath = FileDialog::OpenFile("Select input file...", "Image Files (*.bmp;*.png;*.tiff;*.jpg;*.jpeg)\0*.bmp;*.png;*.tiff;*.jpg;*.jpeg\0");

	ImGui::SameLine();

	ImGui::InputText(	"Input file",
						const_cast<char*>(InputFilePath.c_str()),
						InputFilePath.capacity() + 1,
						ImGuiInputTextFlags_CallbackResize,
						callbackText,
						&InputFilePath);

	if (ImGui::Button("SELECT##Output_File"))
	{
		OutputFilePath = FileDialog::SaveFile("Select output file...", "DirectDraw Surface (*.dds)\0*.dds");

		if (OutputFilePath.substr(OutputFilePath.size() - 4) != ".dds")
		{
			Logger::Debug("Adding missing .dds extenstion...");
			OutputFilePath += ".dds";
		}
	}

	ImGui::SameLine();

	ImGui::InputText(	"Output file",
						const_cast<char*>(OutputFilePath.c_str()),
						OutputFilePath.capacity() + 1,
						ImGuiInputTextFlags_CallbackResize,
						callbackText,
						&OutputFilePath);
}
