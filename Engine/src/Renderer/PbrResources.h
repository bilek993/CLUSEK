#pragma once
#include <string>
#include <d3d11.h>
#include <wrl/client.h>

class PbrResource final
{
public:
	bool Initialize(ID3D11Device* device, const std::string& pathToBrdfLutFile);

	ID3D11ShaderResourceView** GetAddressOfBrdfLutResourceTexture();
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BrdfLutResourceTexture;

	bool LoadBrdfLutFile(ID3D11Device* device, const std::string& path);
};
