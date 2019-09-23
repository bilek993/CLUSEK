#pragma once
#include <string>
#include <d3d11.h>
#include <wrl/client.h>
#include "ComputeTexture.h"
#include "../Loaders/ConfigData.h"

class PbrResource final
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config,
		ID3D11ShaderResourceView* const* skyResourceView);

	ID3D11ShaderResourceView** GetAddressOfBrdfLutResourceTexture();
	ID3D11ShaderResourceView** GetAddressOfIrradianceResourceTexture();
	ID3D11ShaderResourceView** GetAddressOfRadianceResourceTexture();
private:
	inline static const int THREAD_COUNT = 32;
	inline static const int CUBE_SIZE = 6;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	ComputeTexture BrdfLutTexture;
	ComputeTexture IrradianceTexture;
	ComputeTexture RadianceTexture;

	bool InitializeSamplerState(ID3D11Device* device);
	bool LoadBrdfLutFile(ID3D11Device* device, ID3D11DeviceContext* context,
		int textureSize);
	bool GenerateIrradiance(ID3D11Device* device, ID3D11DeviceContext* context, 
		ID3D11ShaderResourceView* const* skyResourceView, int textureSize);
	bool GenerateRadiance(ID3D11Device* device, ID3D11DeviceContext* context,
		ID3D11ShaderResourceView* const* skyResourceView, int textureSize);
	void CleanUp(ID3D11DeviceContext* context);
};
