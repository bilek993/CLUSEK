## Models 3D

Using 3d models is one of the most important things in game engines. That is why this game engine supports a wide variety of 3d formats, thanks to [Assimp](http://www.assimp.org/) library. Almost 99.9999% of creation tools can export 3d models into a format, that CLUSEK supports. 

### Blender and .FBX

The best way to work with this game engine is to use free software named [Blender](https://www.blender.org/) in version 2.8 or later. This 3d modeling software offers the easiest way of exporting models into the CLUSEK game engine. Units in blender should be set to meters and unit scale should be set to 1.0. Exporting format should be set to FBX (.fbx) and the model scale should be set to 0.01. When using these settings, developers and artists can use consistent units to describe the 3d world (e.g. for physics). For other 3d software tools these settings might be different, but this table describes all required settings:

| Parameter    | Value  |
|--------------|--------|
| Units        | Meters |
| Scale        | 0.01   |
| Forward Axis | Z-     |
| Up Axis      | Y+     |