# PBPT: Physically-Based Path Tracer

PBPT is a physically-based path tracer with support for Constructive Solid Geometry (CSG) written in C++20.

The scene from [*Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html) was rendered by PBPT as follows.

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

## License

PBPT is released under the MIT license.

## References

- [Real-Time Rendering](https://www.realtimerendering.com/)
- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [Ray-Traced Constructive Solid Geometry](https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S19/final_projects/audrey_lorelei.pdf)
- [Realistic Image Synthesis Using Photon Mapping](http://www.graphics.stanford.edu/papers/jensen_book/)
- [Physically Based Rendering: From Theory to Implementation](https://pbr-book.org/)
