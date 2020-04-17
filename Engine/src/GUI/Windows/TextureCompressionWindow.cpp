#include "TextureCompressionWindow.h"
#include <imgui.h>
#include "../../Utils/FileDialog.h"
#include "../../Utils/TextureBlockCompression.h"
#include "../../Utils/StringUtil.h"

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
	DrawInputSettingsTreeNode();
	DrawMipMapsSettingsTreeNode();
	DrawCompressionSettingsTreeNode();
	DrawOutputSettingsTreeNode();
	DrawMainActionButton();
}

void TextureCompressionWindow::DrawInputSettingsTreeNode() const
{
	if (ImGui::TreeNode("Input settings"))
	{
		ImGui::TreePop();
	}
}

void TextureCompressionWindow::DrawMipMapsSettingsTreeNode() const
{
	if (ImGui::TreeNode("MipMaps settings"))
	{
		ImGui::TreePop();
	}
}

void TextureCompressionWindow::DrawCompressionSettingsTreeNode() const
{
	if (ImGui::TreeNode("Compression settings"))
	{
		ImGui::TreePop();
	}
}

void TextureCompressionWindow::DrawOutputSettingsTreeNode() const
{
	if (ImGui::TreeNode("Output settings"))
	{
		ImGui::TreePop();
	}
}

void TextureCompressionWindow::DrawMainActionButton()
{
	if (ImGui::Button("Convert"))
	{
		FutureCompression = std::async(std::launch::async, TextureBlockCompression::Compress,	&LoadingInProgress,
																										&InputFilePath,
																										&OutputFilePath);
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

		if (StringUtil::FindExtension(OutputFilePath) != "DDS")
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
