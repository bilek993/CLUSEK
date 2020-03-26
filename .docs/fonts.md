## Fonts

This game engine supports fonts but requires converting fonts into special formal. To convert this you have to use a special tool from Microsoft - MakeSpriteFont.

You can compile this tool on your own from the [source code](https://github.com/microsoft/DirectXTK/wiki/MakeSpriteFont). Anyway, this tool has been added to this project in the executable form, so you can use it without compiling anything.

To use it open your favorite terminal and navigate to the correct path (`...\Dependencies\MakeSpriteFont`). Next, enter the following command:

```
MakeSpriteFont "Comic Sans MS" myfile.spritefont /FontSize:16
```

This command will generate a new file named `myfile.spritefont` from your Comic Sans MS font from your operating system. You can customize generating of the sprite font to suit your needs.