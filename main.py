import os
import json
import atexit
import random
import asyncio
import argparse
import textwrap
import itertools
import subprocess
import skimage.io
import numpy as np


def main(args):

    processes = set()

    @atexit.register
    def killall():
        for process in processes:
            if process.returncode is None:
                try:
                    process.kill()
                # NOTE: why...?
                except ProcessLookupError:
                    pass
        else:
            processes.clear()

    def create_coroutine(program):

        async def coroutine():

            semaphore = asyncio.Semaphore(args.max_workers)

            async def subcoroutine(patch_coord_x, patch_coord_y):

                async with semaphore:

                    print(
                        f"\n-------------------------------- Patch ({patch_coord_x}, {patch_coord_y}) --------------------------------")
                    print(">>> Launching...")

                    process = await asyncio.create_subprocess_shell(
                        program(patch_coord_x, patch_coord_y),
                        stdout=subprocess.PIPE,
                        stderr=subprocess.STDOUT,
                    )
                    processes.add(process)

                    print(
                        f"\n-------------------------------- Patch ({patch_coord_x}, {patch_coord_y}) --------------------------------")
                    print(">>> Launched!")

                    while not process.stdout.at_eof():

                        try:
                            line = await asyncio.wait_for(process.stdout.readline(), args.stdout_timeout)

                        except asyncio.TimeoutError:
                            pass

                        else:
                            print(
                                f"\n-------------------------------- Patch ({patch_coord_x}, {patch_coord_y}) --------------------------------")
                            print(line.decode())

                    await process.wait()

                return (patch_coord_x, patch_coord_y), process

            patch_coords_y, patch_coords_x = zip(
                *itertools.product(range(args.image_height // args.patch_height), range(args.image_width // args.patch_width)))
            subtasks = list(map(asyncio.create_task, map(subcoroutine, patch_coords_x, patch_coords_y)))

            for subtask in asyncio.as_completed(subtasks):

                (patch_coord_x, patch_coord_y), process = await subtask

                print(
                    f"\n-------------------------------- Patch ({patch_coord_x}, {patch_coord_y}) --------------------------------")
                print(">>> Failed..." if process.returncode else ">>> Succeeded!")

                if process.returncode:
                    break

            else:
                return True

            killall()

            for subtask in subtasks:
                if not subtask.done():
                    subtask.cancel()

            return False

        return coroutine

    print(f"\n================================ Parameters ================================")

    print(json.dumps(vars(args), indent=4))

    print(f"\n================================ CMake ================================")

    if (asyncio.run(create_coroutine(lambda patch_coord_x, patch_coord_y: textwrap.dedent(f"""\
        srun cmake \\
            -D CMAKE_BUILD_TYPE=Release \\
            -D CONSTEXPR={"ON" if args.constexpr else "OFF"} \\
            -D IMAGE_WIDTH={args.image_width} \\
            -D IMAGE_HEIGHT={args.image_height} \\
            -D PATCH_WIDTH={args.patch_width} \\
            -D PATCH_HEIGHT={args.patch_height} \\
            -D PATCH_COORD_X={patch_coord_x} \\
            -D PATCH_COORD_Y={patch_coord_y} \\
            -D MAX_DEPTH={args.max_depth} \\
            -D NUM_SAMPLES={args.num_samples} \\
            -D RANDOM_SEED={args.random_seed} \\
            -S {os.path.dirname(os.path.abspath(__file__))} \\
            -B build/patch_{patch_coord_x}_{patch_coord_y}
    """))())):

        print(f"\n================================ CMake ================================")
        print(">>> Succeeded!")

        print(f"\n================================ Make ================================")

        if (asyncio.run(create_coroutine(lambda patch_coord_x, patch_coord_y: textwrap.dedent(f"""\
            srun cmake --build build/patch_{patch_coord_x}_{patch_coord_y}
        """))())):

            print(f"\n================================ Make ================================")
            print(">>> Succeeded!")

            print(f"\n================================ App ================================")

            if (asyncio.run(create_coroutine(lambda patch_coord_x, patch_coord_y: textwrap.dedent(f"""\
                srun build/patch_{patch_coord_x}_{patch_coord_y}/ray_tracing
            """))())):

                print(f"\n================================ App ================================")
                print(">>> Succeeded!")

                image = np.concatenate([
                    np.concatenate([
                        skimage.io.imread(f"outputs/patch_{patch_coord_x}_{patch_coord_y}.ppm")
                        for patch_coord_x in range(args.image_width // args.patch_width)
                    ], axis=1)
                    for patch_coord_y in range(args.image_height // args.patch_height)
                ], axis=0)

                skimage.io.imsave(f"outputs/image.png", image)

            else:

                print(f"\n================================ App ================================")
                print(">>> Failed...")

        else:

            print(f"\n================================ Make ================================")
            print(">>> Failed...")

    else:

        print(f"\n================================ CMake ================================")
        print(">>> Failed...")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Separate Compilation Script")
    parser.add_argument("--constexpr", action="store_true", help="whether to enable compile-time ray tracing")
    parser.add_argument("--image_width", type=int, default=600, help="width of the image")
    parser.add_argument("--image_height", type=int, default=400, help="height of the image")
    parser.add_argument("--patch_width", type=int, default=10, help="width of each patch")
    parser.add_argument("--patch_height", type=int, default=10, help="height of each patch")
    parser.add_argument("--max_depth", type=int, default=50, help="maximum depth for recursive ray tracing")
    parser.add_argument("--num_samples", type=int, default=10,
                        help="number of samples for SSAA (Super-Sampling Anti-Aliasing)")
    parser.add_argument("--random_seed", type=int, default=random.randrange(1 << 32),
                        help="random seed for Monte Carlo approximation")
    parser.add_argument("--max_workers", type=int, default=8, help="maximum number of workers for multiprocessing")
    parser.add_argument("--stdout_timeout", type=float, default=1.0,
                        help="timeout for reading one line from the stream of each child process")

    main(parser.parse_args())
