
# 🪐 3D/2D Graphics Engine

Welcome to the **3D/2D Graphics Engine** – a modern, modular, and plugin-based rendering library designed for high-performance 3D & 2D graphics applications.

This repository is the foundation for a **next-generation graphics stack** inspired by the likes of **BGFX**, **Filament**, and **Flutter**, but designed for:

✅ **Ease of use** (drag-and-drop for beginners)
✅ **Full power** (advanced users can inject custom shaders/pipelines without touching Vulkan/OpenGL directly)
✅ **Backend abstraction** (Vulkan today, OpenGL/Metal/DX12 tomorrow)
✅ **Unified 3D & 2D layers** (game worlds and UI co-exist seamlessly).

---

## 🚀 Features

* 🔌 **Plugin-based rendering backends** (Vulkan, OpenGL stubs, Metal planned)
* 🎨 **High-level declarative API** for pipelines, materials, and framegraphs
* 🧮 SIMD-accelerated **math library** (vec, mat, quaternions)
* 🗂 **Scene graph & render graph** system for 3D/2D objects
* 🖥 **Widget toolkit** for cross-platform UIs (Flutter-like Flex, Stack, Grid layouts)
* ⚡ **Job system** and custom memory allocators for performance-critical tasks
* 📦 Hot-reload of assets & shaders for fast iteration

---

## 🏗 Repository Layout

```
repo-root/
├─ CMakeLists.txt            # Root CMake config; includes per-module subdirs
├─ docs/                     # Design docs, architecture ADRs, mdBook site
├─ extern/                   # Third-party dependencies (GLM, stb, etc.)
├─ tools/                    # Asset cooker, reflection-gen, CI/build scripts
│
├─ src/
│  ├─ core/                  # Engine-agnostic utilities
│  │   ├─ math/              # SIMD vectors, matrices, quaternions
│  │   ├─ memory/            # Arena, pool, frame allocators
│  │   ├─ jobs/              # Thread-pool & task graph
│  │   ├─ util/              # Logger, Time, FileSystem, Config
│  │   └─ core.hpp           # Umbrella include
│  │
│  ├─ graphics/              # High-level API & scene/render graphs
│  │   ├─ api/               # Mesh, Material, Pipeline DSL, Framebuffer DSL
│  │   ├─ scene/             # Scene graph: Node, Transform, Camera, Light
│  │   ├─ render/            # RenderGraph, RenderPass, FrameGraph builder
│  │   ├─ resources/         # MeshLoader, TextureLoader, hot-reload watcher
│  │   └─ graphics.hpp
│  │
│  ├─ ui/                    # Retained-mode UI toolkit (Flutter-like)
│  │   ├─ layout/            # Flex, Stack, Grid, Sliver
│  │   ├─ widgets/           # Button, Slider, Text, Canvas, etc.
│  │   └─ ui.hpp
│  │
│  ├─ backend/               # Pure ABI – no GPU code
│  │   └─ include/IRenderBackend.h
│  │
│  └─ app/                   # Sample apps & sandboxes
│      ├─ Sandbox3D/
│      ├─ SpriteViewer/
│      └─ main.cpp
│
└─ plugins/
   ├─ vulkan/                # Vulkan backend plugin
   ├─ opengl/                # OpenGL stub backend
   └─ null/                  # Null backend (logs calls for testing)
```

---

## 💻 Requirements

* **C++20 or later**
* **CMake 3.20+**
* **Vulkan SDK** (for Vulkan backend) – [Install from LunarG](https://vulkan.lunarg.com/)
* **GLFW / SDL2** (for window/input – optional)
* A **GPU** supporting Vulkan 1.2+

---

## ⚙️ Building

### 1️⃣ Clone the repository

```bash
git clone https://github.com/yourusername/3d2d_engine.git
cd 3d2d_engine
```

### 2️⃣ Install dependencies

* Install Vulkan SDK
* Make sure CMake and a C++20 compiler are available

### 3️⃣ Build the project

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 🎮 Running

Launch the sandbox app:

```bash
./app/Sandbox3D/Sandbox3D
```

This opens a test window and renders a rotating textured cube (once Sprint 2 lands).

---

## 📚 Documentation

* **Design Philosophy** → [docs/3d\_2d\_library.md](docs/3d_2d_library.md)
* **API Reference** → Coming soon (Doxygen + mdBook site)
* **Tutorials & Samples** → After core MVP

---

## 🗺 Roadmap

| Stage         | Goal                                  |
| ------------- | ------------------------------------- |
| ✅ Sprint 0    | Repo reshuffle; build clears screen   |
| 🚧 Sprint 1   | Core foundations (math, memory, jobs) |
| ⏳ Sprint 2    | Vulkan backend parity; textured cube  |
| 🛣 Phases 3–8 | 3D features, 2D renderer, UI toolkit  |
| ✨ Future      | Editor layer, Metal/WebGPU plugins    |

---

## 🤝 Contributing

We welcome contributions! Start by reading [CONTRIBUTING.md](CONTRIBUTING.md) (coming soon) and check the issue tracker for open tasks.

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## ✉ Contact

For questions or feedback, reach out to: `your.email@example.com`

---

