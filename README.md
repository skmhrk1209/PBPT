# Constexpr Ray Tracer <br> *-Compile-Time Ray Tracing in One Weekend-*

<div align="center">
<figure>
<img src="outputs/image.png" width=100%>
<figcaption>
</figcaption>
</div>

This repository provides a C++ program that renders a predefined scene at compile time using constexpr functions and just saves the rendered image at run time. Following [*Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html), simple path tracing and sphere tracing have been implemented as rendering methods, but I'm aiming to implement more rigorous physically based rendering in the future.

## Usage

### Build

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release ..
cmake --build build
```

### Render

```bash
./pbpt -W ${IMAGE_WIDTH} -H ${IMAGE_HEIGHT} -N ${NUM_SAMPLES}
```
