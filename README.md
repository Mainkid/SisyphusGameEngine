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
Physx
1) GameEngine\vendor\PhysX\physx\generate_projects.bat - start and choose installation preset (checked on x64 VC17 (MSVS 2022), so pick preset #2)
2) GameEngine\vendor\PhysX\physx\compiler\vc17win64\PhysxSDK.sln - open with MSVS, RMB->Build on CMakePredefinedTargets\ALL_BUILD in Solution Explorer
3) Just build GameEngine.sln project, in MSVS log check that all necessary .dll and .lib files has been distributed properly.
   
## Screenshots
![alt text](GameEngine/info/engineGUI.png)
![alt text](GameEngine/info/particles.gif)

## Upcoming features
### Shadows:
- Spotlight, point light shadow mapping
- PCF
### Rendering:
- Skybox
- Materials
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
