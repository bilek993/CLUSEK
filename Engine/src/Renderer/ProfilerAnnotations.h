#pragma once
#include <string>
#include "Shaders/ComputeShader.h"
#include <d3d11_1.h>

class ProfilerAnnotations final
{
public:
	explicit ProfilerAnnotations(ID3D11DeviceContext* context);

	void BeginEvent(const std::string& name) const;
	void EndEvent() const;

private:
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> UserDefineAnnotationHandler;
};
