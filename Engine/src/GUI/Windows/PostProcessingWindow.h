#pragma once
#include "BaseWindow.h"

// This function is designed for easier mapping creation of new post processing effect from gui
#define MAP_POST_PROCESSING_EFFECT(NAME, VERTEX_FILENAME, PIXEL_FILENAME) CurrentPostProcessingSettings->List.emplace_back(std::make_unique<ReusablePostProcessing>(renderSystem->GetPointerToDeviceContext(), renderSystem->GetPointerToDevice(), windowWidth, windowHeight, CurrentPostProcessingSettings->Format, NAME, PIXEL_FILENAME, VERTEX_FILENAME));

class PostProcessingWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawStack() const;
	void DrawCreator();
	void AddNewPostProcessingEffect() const;

	int SelectedItemForCreation = 0;

	inline static const std::string PAYLOAD_CELL_NAME = "PP_CELL";

	inline static const char* const AVAILABLE_POST_PROCESSING_EFFECTS[] = 
	{ 
		"Gamma Correction", 
		"Reinhard Tone Mapper",
		"Simple ACES Tone Mapper",
		"Complex ACES Tone Mapper",
	};
};
