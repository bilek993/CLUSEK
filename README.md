CLUSEK
==================================

**CLUSEK** (C++ Language Unroad Simulator Engine Kit) is an advanced game engine written in C++ and designed for off-road game. This kit has been designed with latest technology and architecture patterns for game development. Engine is based on ECS architecture, which follows composition over inheritance pattern. This principle gives developers greater flexibility and performance. This engine also utilizes latest techniques to achieve best possible in-game experience.



## Gallery

TODO: ADD IMAGES



## Binary download

The latest official version of this game engine can always be found [here](https://github.com/bilek993/CLUSEK/releases/latest/). You can also download any release version you like. All release versions can be found [here](https://github.com/bilek993/CLUSEK/releases/).



## Coding standards
This project is using [ISO C++ 17 Standard](https://en.wikipedia.org/wiki/C%2B%2B17), so the code should use latest features designed for this standard. Also it is highly recommended to install and configure [ReSharper C++](https://www.jetbrains.com/resharper-cpp/) for overall better code quality. For consistent code it is required to use Microsoft naming conventions. This naming standard can be set from Resharper tool, but can be also followed manually. This naming standard requires to:

|   Entity kinds   |     Example    |
|:----------------:|:--------------:|
|    Namespaces    | UpperCamelCase |
|       Types      | UpperCamelCase |
|     Functions    | UpperCamelCase |
|    Properties    | UpperCamelCase |
|      Events      | UpperCamelCase |
|      Fields      | UpperCamelCase |
|    Enumerators   | UpperCamelCase |
|     Variables    | lowerCamelCase |
|    Parameters    | lowerCamelCase |
|  Other constants |    ALL_UPPER   |
| Global constands |    ALL_UPPER   |
|      Macros      |    ALL_UPPER   |



## Configuration files

This project is using two types of files for configuration. Each of them is dedicated to solving specialized problems in terms of configuration.

### CFG Files

The first of the configuration type file is a .cfg file. It was designed for this game engine for easy and fast engine configuration. This filetype was designed exclusively for CLUSEK game engine. This filetype can be described as a key-value type, where the user can define the key and a value for this key. Usage example:

```
# ---------------------------------------------------
# Example configuration file block. This block 
# comments can be used to give more information.
# ---------------------------------------------------

# Example one line comment

IntegerVariable: 17
FloatingPointVariable: 0.5
StringVariable: CLUSEK_IS_GREAT
```

### JSON Files

The second of the configuration type file is a .json file. It is a widely known open-standard data serialization format. It's easy to read for humans and implemented by most of the languages. For consistency, this project is using two spaces as indentation character. Also to keep files properly formatted it is recommended to use formatting tools, like [Curious Concept JSON Formatter](https://jsonformatter.curiousconcept.com/). Usage example:

```
{ 
  "Models":{ 
    "CivilianVehicle":"Data/Models/Example/ExampleModel.fbx"
  },
  
  "Textures":{ 
    "ExampleTexture1":"Data/Textures/ExampleTexture1.png",
    "ExampleTexture2":"Data/Textures/ExampleTexture1.png",
    "ExampleTexture3":"Data/Textures/ExampleTexture1.png",

    "Skybox":"Data/Textures/Environment/Skybox.dds"
  }
}
```



## Normal maps

Normal mapping is a technique used in realtime computer graphics to fake bump on materials. It helps reduce polygons and keep most of the details at the same time. Unfortunately, there is no one standard for normal mapping. Different engines are using different mappings for RGB colors to XYZ coordinates. This game engine is using the same mapping as a Unity game engine, which is mapped like this:

| Red | Green | Blue |
|-----|-------|------|
| X+  | Y+    | Z+   |



## Metalic-Smoothness maps

Physical Based Rendering (or PBR) helps reconstruct the real world like appearance of objects. There are two workflows for PBR: specular and metalness. This game engine uses metalness approach, which utilizes only two color channels, red and alpha. The green and blue channels are ignored by the game engine. Usually, they are filled with the same value as the red channel, because it is usually easier to visualizer metalness value of a texture. Mapping for this texture looks like this:

| Red       | Alpha      |
|-----------|------------|
| Metalness | Smoothness |



## Occlusion maps

Occlusion maps helps recreate indirect lighting without time-consuming, real-time computations. This map provides information about indirect lighting, which comes from ambient lighting and reflections. A lot of free and paid tools can bake occlusion maps and export to textures, like [Blender](https://www.blender.org/) or [xNormal](https://xnormal.net/). This game engine supports multi-channel textures or one channel (greyscale) textures. When used with multi-channel texture, it will use only red channel and other channels will be ignored. Anyway, if used with multi-channel textures, it is usually a good idea to set values for green and blue channels to the same value as red, because it is much easier to visualize indirect lighting. This table describes channels for multi-channel occlusion texture:

| Red      |
|----------|
| Lighting |



## Requirements

### Runtime requirements:

* Windows 10
* Directx 11 compatible device

### Build requirements:

* Visual Studio 2017 (v141)
* Windows SDK Version 10.0.17763.0

### Packaging requirements:

* PowerShell 5.0

## Dependencies

* [DirectX Tool Kit for DirectX 11](https://github.com/microsoft/DirectXTK/tree/94e4227a5289f6e5390bf52daf3a4d15ecdf6922)
* [EnTT](https://github.com/skypjack/entt/tree/85152bac3462c95e7be02cd4077bef8fc56de804)
* [Dear ImGui](https://github.com/ocornut/imgui/tree/9f35ba8546b075dfb5d148be4e468ef8c0f5a41c)
* [Assimp](https://github.com/assimp/assimp/tree/80799bdbf90ce626475635815ee18537718a05b1)
* [Nlohmann JSON](https://github.com/nlohmann/json/tree/ffe0e3d70f110bb1ca16f34d7d07a2f0e0974ea6)



## Source code

You can download latest version of source code using git. To do that enter below commands in your terminal:
> git lfs clone https://github.com/bilek993/CLUSEK.git

or

> git lfs clone git@github.com:bilek993/CLUSEK.git



## Redistribution

For easier, consistent and bug-free application redistribution for end-user, the special power shell script can be used. To execute this script you enter below command in your PowerShell terminal:

> Set-ExecutionPolicy Unrestricted -force

> .\Redistribute.ps1

This process is very time consuming and on slower machines can take several dozen minutes. This PowerShell script will generate a .zip file with all the required files. This .zip file can be sent to anyone that wants to use this engine without the need of full source code building.



## License

This application is licensed under [GPL-3.0](LICENSE). More information can be found [here](https://www.gnu.org/licenses/gpl-3.0.en.html).



## Authors

This application is developed by Jakub Biliński. You can find out more about me on my website: [jbilinski.pl](http:/www.jbilinski.pl).