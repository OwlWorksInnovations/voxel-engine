# **Voxel Engine Architecture Specification** (**THIS IS AI GENERATED**)

## **1\. Project Structure & Exporting**

The engine utilizes a **Build Toggle** strategy. The project is split into two targets:

* **Editor Mode:** Includes ImGui, scene hierarchy, and development gizmos.  
* **Runtime Mode:** A stripped-down executable that removes ImGui code and boots directly into assets/scenes/main.json.

### **Deployment Directory Structure**

```
```code
/ProjectRoot

├── /assets

│ ├── /textures \# .png, .jpg files

│ ├── /shaders \# .vs, .fs (Shader.h)

│ ├── /scripts \# .lua user scripts

│ └── /scenes \# .json (Entities) & .bin (Voxel Data)

├── Game.exe \# The compiled Runtime binary

└── glfw3.dll \# Required dynamic libraries
```

## **2\. Scene Serialization**

The scene system manages the state of the Registry (ECS). Because voxels are high-density, we use a hybrid approach:

### **A. Entity Data (JSON)**

Used for high-level entity metadata and component properties.

* **Library:** nlohmann/json  
* **Storage:** transform, name, id, and script\_path.

### **B. Voxel Data (Binary)**

Used for ChunkManager data to ensure fast I/O.

* **Format:** Raw buffer dumps of the chunk arrays or RLE (Run-Length Encoding) for compression.

## **3\. Scripting Integration (Lua)**

The engine supports custom user logic through an embedded **Lua VM** (via the sol2 wrapper).

### **Implementation Workflow**

1. **ScriptComponent:** An ECS component containing a string path to a .lua file.  
2. **Bindings:** The C++ Engine exposes the Transform and Input classes to Lua.  
3. **Lifecycle Hooks:**  
   * OnStart(): Initialization logic.  
   * OnUpdate(dt): Logic executed every frame within the main loop.

## **4\. Systems Architecture Map**

| Layer | Responsibility | Technology |
| :---- | :---- | :---- |
| **Platform** | Windowing & Input | GLFW / GLAD |
| **ECS** | Entity Management | Custom Registry |
| **Rendering** | Voxel & Mesh Drawing | OpenGL 3.3 Core |
| **Logic** | Scripting VM | Lua (sol2) |
| **UI** | Editor Tooling | ImGui |
| **I/O** | Scene Persistence | JSON / Binary Streams |

## **5\. Implementation Roadmap**

* \[ \] **SceneSerializer:** Create a class to iterate through entities and registry to produce JSON.  
* \[ \] **Binary Voxel I/O:** Add SaveToFile and LoadFromFile to ChunkManager.  
* \[ \] **ScriptSystem:** Integrate sol2 and call Lua functions in the main loop.  
* \[ \] **Build Pipeline:** Setup a CMake or Visual Studio configuration for "Runtime" vs "Editor".
