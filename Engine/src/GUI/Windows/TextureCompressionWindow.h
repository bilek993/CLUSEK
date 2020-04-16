#pragma once
#include "BaseWindow.h"

class TextureCompressionWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawLoading() const;
	void DrawMain() const;

	bool LoadingInProgress = false;
};
