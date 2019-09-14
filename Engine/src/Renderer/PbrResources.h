#pragma once
#include <string>
#include <d3d11.h>
#include <wrl/client.h>
#include "ComputeTexture.h"

class PbrResource final
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& pathToBrdfLutFile);

	ID3D11ShaderResourceView** GetAddressOfBrdfLutResourceTexture();
	ID3D11ShaderResourceView** GetAddressOfIrradianceResourceTexture();
private:
	inline static const int THREAD_COUNT = 32;
	inline static const int CUBE_SIZE = 6;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BrdfLutResourceTexture;

	ComputeTexture IrradianceTexture;

	bool LoadBrdfLutFile(ID3D11Device* device, const std::string& path);
	bool GenerateIrradiance(ID3D11Device* device, ID3D11DeviceContext* context);

	ComputeTexture CreateCubeTexture(ID3D11Device* device, int width, int height, 
		DXGI_FORMAT format, bool useMips) const;
	void CreateUnorderedAccessView(ID3D11Device* device, ComputeTexture& texture, int mipSlice = 0) const;
};
