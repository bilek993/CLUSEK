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