# TinyVisualizer

A minimal OpenGL-based mesh processing and visualization library with Python bindings.

**Author:** Zherong Pan (zherong.pan.usa@gmail.com)
**Repository:** [github.com/gaoxifeng/TinyVisualizer](https://github.com/gaoxifeng/TinyVisualizer)

## Features

- **Rendering** — OpenGL 3.3+ core profile, shadow mapping with PCF soft shadows, multiple point lights, Phong materials (ambient/diffuse/specular), texture mapping, transparency and alpha blending
- **Shapes** — Sphere, box, capsule, spherical box, terrain (heightmap), arrows, Bézier/Hermite curves, voxel cells, triangle/line/point meshes, skinned meshes with skeletal animation, composite shapes
- **Cameras** — 2D orthographic and 3D perspective cameras with first-person (WASD) and trackball (arcball) manipulators
- **Physics** — Box2D integration for 2D rigid-body simulation, Bullet Physics integration for 3D simulation
- **UI** — Dear ImGui immediate-mode GUI, file open/save dialogs, keyboard and mouse input handling
- **Export** — GIF recording, MPEG2 video capture, PNG screenshots, POV-Ray scene export
- **Spatial** — Octree-based scene graph with view-frustum culling and ray intersection for object picking
- **Python** — Full Python bindings via pybind11 (`pyTinyVisualizer`)

## Dependencies

### Required

| Dependency | Purpose |
|---|---|
| [CMake](https://cmake.org/) ≥ 3.21 | Build system |
| [Eigen3](https://eigen.tuxfamily.org/) | Linear algebra |
| [GLFW](https://www.glfw.org/) | Window management and input |
| OpenGL | Graphics rendering |

### Optional

| Dependency | Purpose | CMake Option |
|---|---|---|
| [Box2D](https://box2d.org/) | 2D physics simulation | `USE_BOX2D=ON` |
| [Bullet Physics](https://pybullet.org/) | 3D physics simulation | `USE_BULLET=ON` |
| [Assimp](https://www.assimp.org/) | 3D model loading (FBX, glTF, …) | Auto-detected |
| [Python 3](https://www.python.org/) | Python bindings | `PYTHON_BINDING=Python3` |

### Bundled (in `extern/`)

[GLAD](https://glad.dav1d.de/) (OpenGL loader) · [Dear ImGui](https://github.com/ocornut/imgui) · [stb](https://github.com/nothings/stb) (image I/O) · [gif.h](https://github.com/charlietangora/gif-h) · [jo_mpeg.h](https://www.jonolick.com/) · [pybind11](https://github.com/pybind/pybind11)

## Building

### CMake Options

| Option | Default | Description |
|---|---|---|
| `CMAKE_BUILD_TYPE` | `Release` | `Debug` or `Release` |
| `USE_BOX2D` | `OFF` | Enable Box2D physics support |
| `USE_BULLET` | `OFF` | Enable Bullet Physics support |
| `USE_RTTI` | `ON` | Use native RTTI (`OFF` uses custom RTTI) |
| `USE_CXX11_ABI` | `ON` | Use C++11 ABI |
| `PYTHON_BINDING` | _(none)_ | `Python2`, `Python3`, or empty to disable |

### Linux / macOS

```bash
git clone https://github.com/gaoxifeng/TinyVisualizer.git
mkdir build && cd build
cmake ../TinyVisualizer -DPYTHON_BINDING=Python3
make install -j$(nproc)
```

### Windows (vcpkg + CMake Presets)

```powershell
git clone https://github.com/gaoxifeng/TinyVisualizer.git
cd TinyVisualizer
cmake --preset msvc-release
cmake --build build/release --target INSTALL
```

The presets expect vcpkg at `C:/vcpkg`. Adjust `CMakePresets.json` if your vcpkg path differs.

### Python Package

```bash
pip install .
```

This invokes `setup.py`, which runs CMake with `PYTHON_BINDING=Python3`, `USE_BULLET=ON`, and `USE_BOX2D=ON` automatically.

## Quick Start

### C++

```cpp
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/ShadowAndLight.h>

using namespace DRAWER;

int main(int argc, char** argv) {
  Drawer drawer(argc, argv);

  // Create a textured sphere
  auto sphere = makeSphere(8, true, 0.5f);
  sphere->setTextureDiffuse(drawChecker());
  drawer.addShape(sphere);

  // Add shadow-mapped lighting
  drawer.addLightSystem(2048, 20);
  drawer.getLight()->addLight(
    Eigen::Vector3f(2, 2, 2),  // position
    Eigen::Vector3f(1, 1, 1),  // ambient
    Eigen::Vector3f(1, 1, 1),  // diffuse
    Eigen::Vector3f(0, 0, 0)   // specular
  );

  drawer.mainLoop();
  return 0;
}
```

### Python

```python
import pyTinyVisualizer as vis
import numpy as np

drawer = vis.Drawer([])
drawer.addLightSystem(2048, 20, False)
drawer.getLight().addLight(
    np.array([2, 2, 2], dtype=np.float32),
    np.array([1, 1, 1], dtype=np.float32),
    np.array([1, 1, 1], dtype=np.float32),
    np.array([0, 0, 0], dtype=np.float32),
)

sphere = vis.makeSphere(8, True, 0.5)
sphere.setTextureDiffuse(vis.drawChecker())
drawer.addShape(sphere)

drawer.mainLoop()
```

## Examples

All examples are in the `Main/` directory. Build with CMake and run directly, or run the `.py` variants with `pyTinyVisualizer` installed.

| Example | Description |
|---|---|
| `mainLight` | Shadow mapping with multiple colored point lights |
| `mainImGui` | Dear ImGui integration with interactive arrow controls |
| `mainBox2D` | Box2D 2D physics simulation (bridge, shapes) |
| `mainBullet3D` | Bullet Physics 3D rigid-body simulation |
| `mainMesh` | Skeletal mesh animation via Assimp |
| `mainMultiview` | 2×2 multi-viewport rendering layout |
| `mainTerrain` | Heightmap terrain rendering |
| `mainVisibility` | Visibility scoring between meshes |
| `mainSelection2D` | 2D object picking via ray intersection |
| `mainSelection3D` | 3D object picking via ray intersection |
| `mainBezier` | Bézier and Hermite curve rendering |
| `mainArrow` | 3D arrow shape visualization |
| `mainCell` | Voxel cell rendering |
| `mainCellCmd` | Voxel cells via command-line parameters |
| `mainChecker` | Checker and grid texture patterns |
| `mainOctree` | Octree spatial data structure demo |
| `mainSphericalBoxAndPoints` | Spherical box geometry with point clouds |

## Project Structure

```
TinyVisualizer/
├── CMakeLists.txt              # Root build configuration
├── CMakePresets.json           # MSVC build presets (vcpkg)
├── setup.py                    # Python package build (pip install .)
├── TinyVisualizer/             # Core library
│   ├── Drawer.h/cpp            # Main application class
│   ├── MeshShape.h/cpp         # Triangle/line/point mesh
│   ├── MakeMesh.h/cpp          # Mesh factory functions
│   ├── MakeTexture.h/cpp       # Texture factory functions
│   ├── ShadowAndLight.h/cpp    # Shadow mapping + lighting
│   ├── Camera2D.h/cpp          # 2D orthographic camera
│   ├── Camera3D.h/cpp          # 3D perspective camera
│   ├── ImGuiPlugin.h/cpp       # Dear ImGui integration
│   ├── Box2DShape.h/cpp        # Box2D physics shapes
│   ├── Bullet3DShape.h/cpp     # Bullet Physics shapes
│   ├── ...                     # Additional shapes, plugins, utilities
│   └── Shader/                 # GLSL shader sources (as headers)
├── Main/                       # Example programs (.cpp and .py)
├── Python/                     # pybind11 bindings
│   ├── Python.cpp
│   ├── Constants.h
│   └── PythonImgui.h
├── cmake/                      # CMake modules (FindXxx, LoadLibs)
└── extern/                     # Bundled third-party libraries
    ├── glad/
    ├── imgui/
    ├── stb/
    └── pybind11/
```

## API Overview

### Core

| Class | Description |
|---|---|
| `Drawer` | Main application — window, scene, render loop |
| `MultiDrawer` | Multi-viewport rendering |
| `Shape` | Base class for all drawable objects |
| `SceneNode` | Octree-based spatial scene structure |
| `Plugin` | Base class for extending functionality |

### Shapes

| Class | Description |
|---|---|
| `MeshShape` | Triangle, line, and point meshes with materials |
| `CompositeShape` | Container for grouping multiple shapes |
| `ArrowShape` | 3D arrow for vectors and directions |
| `BezierCurveShape` | Bézier and Hermite curve rendering |
| `CellShape` | Voxel grid rendering |
| `TerrainShape` | Heightmap terrain |
| `Box2DShape` | Box2D physics body visualization |
| `Bullet3DShape` | Bullet Physics body visualization |
| `SkinnedMeshShape` | Skeletal animation (requires Assimp) |
| `LowDimensionalMeshShape` | Dimensionality-reduction visualization |

### Cameras

| Class | Description |
|---|---|
| `Camera2D` | 2D orthographic camera |
| `Camera3D` | 3D perspective camera |
| `FirstPersonCameraManipulator` | WASD + mouse-look controls |
| `TrackballCameraManipulator` | Arcball rotation around a target |

### Plugins

| Class | Description |
|---|---|
| `ImGuiPlugin` | Dear ImGui integration |
| `CaptureGIFPlugin` | GIF recording and screenshots |
| `CaptureMPEG2Plugin` | MPEG2 video recording |
| `CameraExportPlugin` | Camera state save/load |

### Factory Functions

```cpp
// Mesh creation (MakeMesh.h)
makeSphere(RES, fill, rad)
makeBox(RES, fill, halfExt)
makeCapsule(RES, fill, rad, height)
makeSquare(fill, halfExt, depth)
makeCircle(RES, fill, pos, rad)
makeSphericalBox(RES, fill, rad, halfExt)
makeTriMesh(fill, V, F, faceBased)
makeWires(V, E)
makeGrid(mesh, RES, fill, ctr, d0, d1)

// Texture creation (MakeTexture.h)
drawChecker(density, c0, c1)
drawGrid(density, t0, t1, c0, c1)
getWhiteTexture()
```

### Graphics Utilities

| Class | Description |
|---|---|
| `Texture` | 2D texture management |
| `FBO` / `FBOShadow` / `FBOPingPong` | Framebuffer objects |
| `VBO` | Vertex buffer objects |
| `Shader` / `Program` | GLSL shader compilation and linking |
| `ShadowLight` | Shadow mapping and lighting system |
| `Material` | Surface material properties |
| `Povray` | POV-Ray scene export |
