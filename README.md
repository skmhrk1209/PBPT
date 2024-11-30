# PBPT: Physically-Based Path Tracer

A physically-based version of [*Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html) with support for Constructive Solid Geometry (CSG).

<div align="center">
<figure>
<img src="outputs/image.png" width=100%>
<figcaption>
</figcaption>
</div>

## Usage

### Build

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

### Render

Parallel rendering with MPI and OpenMP is supported.

```bash
mpiexec -np ${NUM_WORKERS} \
    build/pbpt -W ${IMAGE_WIDTH} -H ${IMAGE_HEIGHT} -N ${NUM_SAMPLES} \
```
