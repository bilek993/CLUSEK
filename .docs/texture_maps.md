## Texture maps

Some maps can differ from game engine to game engine. That is why some map has to be designed in a specific way. This game engine tries to be very efficient, but also at the same time compatible with resources from other game engines. Bellow, you will read what should be used for best results in the CLUSEK game engine, but keep in my, that developer can choose another way for texture mapping.

### Tools

A custom texture compression tool has been designed for compressing textures in the CLUSEK game engine. Other tools like [NVIDIA Texture Tools Exporter](https://developer.nvidia.com/nvidia-texture-tools-exporter) could be used, but due to their limited capabilities, it's recommended to use internal CLUSEK tool. You can find it in Tools -> Texture compression.

<img src="compression_tool.jpg">

### Implementation

Recommended texture maps for CLUSEK have been shown bellow. These rules should be followed for best graphical and performance results. This game engine uses Block Compression. More about this topic you can read [here](https://docs.microsoft.com/en-us/windows/win32/direct3d11/texture-block-compression-in-direct3d-11) and [here](http://www.reedbeta.com/blog/understanding-bcn-texture-compression-formats/). Using Block Compression is recommended in most cases.

#### Albedo maps

Albedo maps are base color textures for models. All channels represent the colors of the object. Recommended compression for this texture is DXGI_FORMAT_BC1_UNORM_SRGB and flag TEX_COMPRESS_SRGB_IN in Compression settings should be set to TRUE.

#### Normal maps

Normal mapping is a technique used in realtime computer graphics to fake bump on materials. It helps reduce polygons and keep most of the details at the same time. Different engines are using different mappings for RGB colors to XYZ coordinates. The easiest way is to map R to X, G to Y and B to Z. That's the easiest way, but not the most efficient way. The best way is to use only two channels with compression set to DXGI_FORMAT_BC5_UNORM. That means we are using only two channels for X and Y. Z is reconstructed in real-time. That means that bellow mapping is used:

| Red | Green |
|-----|-------|
| X+  | Y+    |

#### Metalic-Smoothness maps

Physical Based Rendering (or PBR) helps reconstruct the real world like appearance of the objects. There are two workflows for PBR: specular and metalness. This game engine uses metalness approach, which utilizes only two color channels, red and green. Other channels are ignored by the game engine. Recommended compression is DXGI_FORMAT_BC5_UNORM. Mapping for this texture looks like this:

| Red       | Green      |
|-----------|------------|
| Metalness | Smoothness |

#### Occlusion maps

Occlusion maps help recreate indirect lighting without time-consuming, real-time computations. This map provides information about indirect lighting, which comes from ambient lighting and reflections. A lot of free and paid tools can bake occlusion maps and export to textures, like [Blender](https://www.blender.org/) or [xNormal](https://xnormal.net/). This game engine supports multi-channel textures or one channel (greyscale) textures. When used with multi-channel texture, it will use only red channel and other channels will be ignored. Anyway, if used with multi-channel textures, it is usually a good idea to set values for green and blue channels to the same value as red, because it is much easier to visualize indirect lighting. he recommended compression for these maps is DXGI_FORMAT_BC4_UNORM. This table describes channels for multi-channel occlusion texture:

| Red      |
|----------|
| Lighting |

#### Height maps

This map provides information about terrain height in every point of it. This game engine supports a single-channel texture for this purpose and multi-channel texture will throw a warning in the log system. It is also recommended to use 16 bit textures instead of 8 bit for better quality. Please do not use block compression on height maps, because it is not supported! This table describes the usage of channels if used in multi-channel mode:

| Red    |
|--------|
| Height |

#### Splat maps

This texture is used to control the blending of detail textures on a terrain. Each channel represents a different mask for detail textures. Only 3 channels are used (RGB) for better performance because splat map texturing is very costly to render. Recommended compression is DXGI_FORMAT_BC7_UNORM without alpha.  Mapping for this texture:

| Red | Green | Blue |
|-----|-------|------|
| Red | Green | Blue |

#### Skybox maps

Skybox is used not only in rendering the sky but also in rendering each PBR object. Skyboxes are often huge. That is why it is recommended to compress skyboxes using DXGI_FORMAT_BC6H_*. Uncompressed skyboxes can be very inefficient.