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

**I'm trying to compile source code with a newer Visual Studio compiler, but it fails. How can I solve this?**

It's hard to tell. Other compilers are highly not recommended but might work. The best way is to navigating to 'Error List' and solving errors one by one. A known problem is missing memory include. To solve this add ``#include <memory>`` at the top of the files when compiler tells it's needed for smart pointers.

**I would like to add new component. Should I do something special?**

Yes, besides adding a new structure in `ECS/Components` folder and filter, you should do one more thing. In `EntityViewerWindow` class in `DrawList` method, you should also define your component. It is needed for GUI Editor. Below is the example of declaring a new component.

```
DrawComponent<NewComponent>(entity, "New Component Name");
```