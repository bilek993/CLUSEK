## Texture maps

Some maps can differ from game engine to game engine. That is why some map has to be designed in a specify way. This game engine tries to be as much as possible compatible with Unity maps.

### Normal maps

Normal mapping is a technique used in realtime computer graphics to fake bump on materials. It helps reduce polygons and keep most of the details at the same time. Unfortunately, there is no one standard for normal mapping. Different engines are using different mappings for RGB colors to XYZ coordinates. This game engine is using the same mapping as a Unity game engine, which is mapped like this:

| Red | Green | Blue |
|-----|-------|------|
| X+  | Y+    | Z+   |

### Metalic-Smoothness maps

Physical Based Rendering (or PBR) helps reconstruct the real world like appearance of objects. There are two workflows for PBR: specular and metalness. This game engine uses metalness approach, which utilizes only two color channels, red and alpha. The green and blue channels are ignored by the game engine. Usually, they are filled with the same value as the red channel, because it is usually easier to visualizer metalness value of a texture. Mapping for this texture looks like this:

| Red       | Alpha      |
|-----------|------------|
| Metalness | Smoothness |

### Occlusion maps

Occlusion maps helps recreate indirect lighting without time-consuming, real-time computations. This map provides information about indirect lighting, which comes from ambient lighting and reflections. A lot of free and paid tools can bake occlusion maps and export to textures, like [Blender](https://www.blender.org/) or [xNormal](https://xnormal.net/). This game engine supports multi-channel textures or one channel (greyscale) textures. When used with multi-channel texture, it will use only red channel and other channels will be ignored. Anyway, if used with multi-channel textures, it is usually a good idea to set values for green and blue channels to the same value as red, because it is much easier to visualize indirect lighting. This table describes channels for multi-channel occlusion texture:

| Red      |
|----------|
| Lighting |

### Height maps

This map provides information about terrain height in every point of it. This game engine supports a single-channel texture for this purpose and multi-channel texture will throw a warning in the log system. It is also recommended to use 16 bit textures instead of 8 bit for better quality. This table describes the usage of channels if used in multi-channel mode:

| Red    |
|--------|
| Height |