#pragma once
#include "BaseWindow.h"

class VehicleDetailsWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	int SelectedId = -1;
};
