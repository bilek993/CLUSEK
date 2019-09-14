#pragma once
#include <string>
#include <d3d11.h>
#include <wrl/client.h>
#include "ComputeTexture.h"

class PbrResource final
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& pathToBrdfLutFile,
		ID3D11ShaderResourceView* const* skyResourceView);

	ID3D11ShaderResourceView** GetAddressOfBrdfLutResourceTexture();
	ID3D11ShaderResourceView** GetAddressOfIrradianceResourceTexture();
	ID3D11ShaderResourceView** GetAddressOfRadianceResourceTexture();
private:
	inline static const int THREAD_COUNT = 32;
	inline static const int CUBE_SIZE = 6;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BrdfLutResourceTexture;
	ComputeTexture IrradianceTexture;
	ComputeTexture RadianceTexture;

	bool InitializeSamplerState(ID3D11Device* device);
	bool LoadBrdfLutFile(ID3D11Device* device, const std::string& path);
	bool GenerateIrradiance(ID3D11Device* device, ID3D11DeviceContext* context, 
		ID3D11ShaderResourceView* const* skyResourceView);
	bool GenerateRadiance(ID3D11Device* device, ID3D11DeviceContext* context,
		ID3D11ShaderResourceView* const* skyResourceView);
	void CleanUp(ID3D11DeviceContext* context) const;

	ComputeTexture CreateCubeTexture(ID3D11Device* device, int width, int height, 
		DXGI_FORMAT format, bool useMips) const;
	void CreateUnorderedAccessView(ID3D11Device* device, ComputeTexture& texture, int mipSlice = 0) const;
};
