# Voxel Engine (OpenGL, C++)

## Examples

## Compile & Run (All in one)
```
mkdir build/ && cd ./build && cmake --build . && cd ../dist && ./voxel-engine
```

### Keybinds
- Movement: WASD
- Look Around: Mouse
- Toggle Wireframe: ;

## How did I learn?
I followed the getting started tutorial on [LearnOpenGL](https://learnopengl.com/Getting-started/OpenGL). I highly recommend doing the exercises; they seem stupid but you learn so much from them.

## Is this project finished?
No, if you would like to see a progress report look at the TODO.md in this repo.

## Should you learn OpenGL?
It depends if you want to make a game more than a game engine. I personally enjoy knowing how everything works because if I want to implement something I don't need to google "how to do x in Godot". A downside of this is time, because now I need to implement whatever I need from scratch but then again this way I will always be able to know exactly how to use everything. Another reason might be if you want to have flexibility and have tight performance constraints, like making a voxel engine because in Godot implementing all the optimization methods don't nearly match the performance gain compared to scratch in OpenGL.
