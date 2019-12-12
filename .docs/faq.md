## FAQ

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