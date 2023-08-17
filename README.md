# GameEngine
My very own game engine.


## Features
- ECS architectural pattern.
- Phong shading.
- Deferred rendering.
- Directional, Spot, Point lights.
- Cascaded Shadow Mapping for Directional lights.
- Basic GUI functionality (Hierarchy Window, Content Browser, Viewport, etc.)
- Mouse picking and interactive guizmo tools.
- GPU-particle system implementation (with Bitonic sort algorithm, HL-vectors based shading).

## Installation
```bash
git clone --recurse-submodules https://github.com/Mainkid/GameEngine.git
mkdir GameEngine/bin
cd GameEngine/bin
cmake ../GameEngine
```

## Upcoming features
### Shadows:
- Spotlight, point light shadow mapping
- PCF
### Rendering:
- Skybox
- PBR
- Transparency support
- Global illumination
- Antialiasing
- Gamma correction :)
### GPU:
- Fexible particle system editor
### Engine Functionality:
- Playmode
- Native scripting
- Data serialization
