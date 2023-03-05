# Constexpr Ray Tracer <br> *-Compile-Time Ray Tracing in One Weekend-*

<div align="center">
<figure>
<img src="outputs/image.png" width=100%>
<figcaption>
</figcaption>
</div>

This repository provides a C++ program that renders a predefined scene at compile time using constexpr functions and just saves the rendered image at run time. Following [*Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html), simple path tracing and sphere tracing have been implemented as rendering methods, but I'm aiming to implement more rigorous physically based rendering in the future.

## Separate Rendering

Since rendering a high-resolution image at once would cause the compiler to eat up the memory, separate rendering, whereby all the pixels are split into small-sized chunks and each chunk is rendered separately, is supported. This approach is implemented on a Python script as parallel compilations. The usage of this script is as follows:

```bash
usage: main.py [-h] [--constexpr] [--image_width IMAGE_WIDTH] [--image_height IMAGE_HEIGHT] [--patch_width PATCH_WIDTH] [--patch_height PATCH_HEIGHT]
               [--max_depth MAX_DEPTH] [--num_samples NUM_SAMPLES] [--random_seed RANDOM_SEED] [--max_workers MAX_WORKERS] [--stdout_timeout STDOUT_TIMEOUT]

Separate Compilation Script

optional arguments:
  -h, --help                      show this help message and exit
  --constexpr                     whether to enable compile-time ray tracing
  --image_width IMAGE_WIDTH       width of the image
  --image_height IMAGE_HEIGHT     height of the image
  --patch_width PATCH_WIDTH       width of each patch
  --patch_height PATCH_HEIGHT     height of each patch
  --max_depth MAX_DEPTH           maximum depth for recursive ray tracing
  --num_samples NUM_SAMPLES       number of samples for SSAA (Super-Sampling Anti-Aliasing)
  --random_seed RANDOM_SEED       random seed for Monte Carlo approximation
  --max_workers MAX_WORKERS       maximum number of workers for multiprocessing
  --stdout_timeout STDOUT_TIMEOUT timeout for reading one line from the stream of each child process
```
