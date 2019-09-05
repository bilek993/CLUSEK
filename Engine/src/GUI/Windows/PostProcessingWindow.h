#pragma once
#include "BaseWindow.h"

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
	};
};
