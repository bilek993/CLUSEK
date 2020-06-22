## FAQ

**Should I build CLUSEK from source code or download binary version?**

If you want to redesign how the engine works, then you download source code. If you are interested in adding own models and redefining scene, then you can download binary version and when you exceed limits of this version, you can always download source code and use it that way.

**When I try to run this, it freezes at the loading screen. How can I solve this?**

It looks like the TDR mechanism kicks in and disconnect your adapter. This mechanism is provided by GPU manufacturer drivers to the OS. More about it you can read [here](https://docs.nvidia.com/gameworks/content/developertools/desktop/timeout_detection_recovery.htm). The simplest way to fix it is to change `IrradianceSampleDelta` in configuration file from the default value to `0.0125` to `0.05` or even higher.

**I have weird dark visual artifacts. Should all objects be very dark?**

They shouldn't unless you use dark environment texture. This situation mainly occurs when the graphic card can't handle converting LatLong image to Cubemap. To fix this change `ConvertLatLongToCubeMap` from `YES` to `COMPATIBLE` in resources file. Below you can see an example of resources file with correct parameters for compatible mode.

```
"Skybox": {
  "Path": "Data/Textures/Environment/Skybox.dds",
  "ConvertLatLongToCubeMap": "COMPATIBLE"
}
```

**My sRGB texture is rendered incorrectly. Does CLUSEK support sRGB textures?**

Yes, the CLUSEK game engine fully supports sRGB textures and automatically imports them correctly. To determine if texture file is sRGB it uses metadata as follow:

* For PNG, it is indicated by `/sRGB/RenderingIntent` set to 1;
* For JPG, it is indicated by `/app1/ifd/exif/{ushort=40961}` set to 1;
* For TIFF is is indicated by `/ifd/exif/{ushort=40961}` set to 1.

For more informatio you can follow [Microsoft documenation for WIC Texture Loader](https://github.com/Microsoft/DirectXTK/wiki/WICTextureLoader). 

If you would like to use sRGB format without declaring it in the texture file, you can force loading it in sRGB mode by modifying the resources file. Set parameter `SRGBMode` to `FORCED`. It will always load this texture in sRGB mode, no matter what is set in metadata of the texture file. Below you can see an example of importing texture with `FORCED` mode.

```
"ExampleAlbedo": {
  "Path": "Data/Textures/Example/ExampleAlbedo.png",
  "SRGBMode": "FORCED"
}
```

**How can I enable mipmaps generation?**

Mipmaps generation is disabled by default, but can be enabled by setting `SKIP` value for the `MipMaps` parameter in the resources file. In the example below, you can see how it works for example texture.

```
"ExampleNormalTexture": {
  "Path": "Data/Textures/Example/ExampleNormalTexture.png",
  "MipMaps": "GENERATE"
}
```

**I'm trying to compile source code with a newer Visual Studio compiler, but it fails. How can I solve this?**

It's hard to tell. Other compilers are highly not recommended but might work. The best way is to navigating to 'Error List' and solving errors one by one. A known problem is missing memory include. To solve this add ``#include <memory>`` at the top of the files when compiler tells it's needed for smart pointers.

**I would like to add new component. Should I do something special?**

Detailed information about adding a new component you can find in [the ECS section](./ecs.md).