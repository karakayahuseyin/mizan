# Mizan

Mizan is a lightweight 3D geometric modeling kernel built from scratch with modern C++20. It provides a clean, handle-based API for boundary representation (B-Rep) modeling with support for both analytic geometries and NURBS curves/surfaces. Designed with minimal dependencies, Mizan aims to serve as a foundation for CAD applications, 3D editors, and similar tools.

---

## Build

```bash
cmake -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build
```

## Install

```bash
cmake --install build --prefix build/install
```