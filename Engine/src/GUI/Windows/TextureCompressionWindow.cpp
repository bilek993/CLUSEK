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

void TextureCompressionWindow::DrawInputSettingsTreeNode()
{
	if (ImGui::TreeNode("Input settings"))
	{
		ImGui::Checkbox("WIC_FLAGS_NONE", &CurrentCompressionSettings.WIC_FLAGS_NONE);
		ImGui::Checkbox("WIC_FLAGS_FORCE_RGB", &CurrentCompressionSettings.WIC_FLAGS_FORCE_RGB);
		ImGui::Checkbox("WIC_FLAGS_NO_X2_BIAS", &CurrentCompressionSettings.WIC_FLAGS_NO_X2_BIAS);
		ImGui::Checkbox("WIC_FLAGS_NO_16BPP", &CurrentCompressionSettings.WIC_FLAGS_NO_16BPP);
		ImGui::Checkbox("WIC_FLAGS_ALLOW_MONO", &CurrentCompressionSettings.WIC_FLAGS_ALLOW_MONO);
		ImGui::Checkbox("WIC_FLAGS_ALL_FRAMES", &CurrentCompressionSettings.WIC_FLAGS_ALL_FRAMES);
		ImGui::Checkbox("WIC_FLAGS_IGNORE_SRGB", &CurrentCompressionSettings.WIC_FLAGS_IGNORE_SRGB);
		ImGui::Checkbox("WIC_FLAGS_FORCE_SRGB", &CurrentCompressionSettings.WIC_FLAGS_FORCE_SRGB);
		ImGui::Checkbox("WIC_FLAGS_FORCE_LINEAR", &CurrentCompressionSettings.WIC_FLAGS_FORCE_LINEAR);
		ImGui::Checkbox("WIC_FLAGS_DITHER", &CurrentCompressionSettings.WIC_FLAGS_DITHER);
		ImGui::Checkbox("WIC_FLAGS_DITHER_DIFFUSION", &CurrentCompressionSettings.WIC_FLAGS_DITHER_DIFFUSION);
		ImGui::Checkbox("WIC_FLAGS_FILTER_POINT", &CurrentCompressionSettings.WIC_FLAGS_FILTER_POINT);
		ImGui::Checkbox("WIC_FLAGS_FILTER_LINEAR", &CurrentCompressionSettings.WIC_FLAGS_FILTER_LINEAR);
		ImGui::Checkbox("WIC_FLAGS_FILTER_CUBIC", &CurrentCompressionSettings.WIC_FLAGS_FILTER_CUBIC);
		ImGui::Checkbox("WIC_FLAGS_FILTER_FANT", &CurrentCompressionSettings.WIC_FLAGS_FILTER_FANT);
		ImGui::TreePop();
	}
}

void TextureCompressionWindow::DrawMipMapsSettingsTreeNode()
{
	if (ImGui::TreeNode("MipMaps settings"))
	{
		ImGui::Checkbox("TEX_FILTER_DEFAULT", &CurrentCompressionSettings.TEX_FILTER_DEFAULT);
		ImGui::Checkbox("TEX_FILTER_POINT", &CurrentCompressionSettings.TEX_FILTER_POINT);
		ImGui::Checkbox("TEX_FILTER_LINEAR", &CurrentCompressionSettings.TEX_FILTER_LINEAR);
		ImGui::Checkbox("TEX_FILTER_CUBIC", &CurrentCompressionSettings.TEX_FILTER_CUBIC);
		ImGui::Checkbox("TEX_FILTER_FANT", &CurrentCompressionSettings.TEX_FILTER_FANT);
		ImGui::Checkbox("TEX_FILTER_BOX", &CurrentCompressionSettings.TEX_FILTER_BOX);
		ImGui::Checkbox("TEX_FILTER_TRIANGLE", &CurrentCompressionSettings.TEX_FILTER_TRIANGLE);
		ImGui::Checkbox("TEX_FILTER_DITHER", &CurrentCompressionSettings.TEX_FILTER_DITHER);
		ImGui::Checkbox("TEX_FILTER_DITHER_DIFFUSION", &CurrentCompressionSettings.TEX_FILTER_DITHER_DIFFUSION);
		ImGui::Checkbox("TEX_FILTER_WRAP_U", &CurrentCompressionSettings.TEX_FILTER_WRAP_U);
		ImGui::Checkbox("TEX_FILTER_WRAP_V", &CurrentCompressionSettings.TEX_FILTER_WRAP_V);
		ImGui::Checkbox("TEX_FILTER_WRAP_W", &CurrentCompressionSettings.TEX_FILTER_WRAP_W);
		ImGui::Checkbox("TEX_FILTER_MIRROR_U", &CurrentCompressionSettings.TEX_FILTER_MIRROR_U);
		ImGui::Checkbox("TEX_FILTER_MIRROR_V", &CurrentCompressionSettings.TEX_FILTER_MIRROR_V);
		ImGui::Checkbox("TEX_FILTER_MIRROR_W", &CurrentCompressionSettings.TEX_FILTER_MIRROR_W);
		ImGui::Checkbox("TEX_FILTER_SRGB_IN", &CurrentCompressionSettings.TEX_FILTER_SRGB_IN);
		ImGui::Checkbox("TEX_FILTER_SRGB_OUT", &CurrentCompressionSettings.TEX_FILTER_SRGB_OUT);
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
