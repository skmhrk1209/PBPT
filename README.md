# PBPT: Physically Based Path Tracer

<div align="center">
<figure>
<img src="outputs/image.png" width=100%>
<figcaption>
</figcaption>
</div>

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
