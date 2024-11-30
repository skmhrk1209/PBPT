#include <omp.h>

#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <execution>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <string>
#include <thread>

#include "image.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "scene/weekend.hpp"
#include "tensor.hpp"

int main(int argc, char** argv) {
    boost::mpi::environment environment(argc, argv);
    boost::mpi::communicator communicator;

    boost::program_options::options_description options_description("Monte-Carlo Path Tracer");
    options_description.add_options()("image_width,W", boost::program_options::value<int>()->default_value(1000), "Image width")(
        "image_height,H", boost::program_options::value<int>()->default_value(1000), "Image height")(
        "num_samples,N", boost::program_options::value<int>()->default_value(1000), "Number of samples per pixel for Monte-Carlo")(
        "bernoulli_p,P", boost::program_options::value<float>()->default_value(0.99), "Continuation probability for Russian roulette")(
        "random_seed,S", boost::program_options::value<int>()->default_value(__LINE__), "Random seed for Monte-Carlo sampling")(
        "num_threads,T", boost::program_options::value<int>()->default_value(1), "Number of threads for OpenMP")("help,h", "Shows help");

    boost::program_options::variables_map variables_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options_description), variables_map);
    boost::program_options::notify(variables_map);

    if (variables_map.count("help") && !communicator.rank()) {
        std::cout << options_description << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    auto image_width = variables_map["image_width"].as<int>();
    auto image_height = variables_map["image_height"].as<int>();
    auto num_samples = variables_map["num_samples"].as<int>();
    auto bernoulli_p = variables_map["bernoulli_p"].as<float>();
    auto random_seed = variables_map["random_seed"].as<int>();
    auto num_threads = variables_map["num_threads"].as<int>();

    omp_set_num_threads(num_threads);

    if (!communicator.rank()) {
        std::cout << "\n================ MPI ================" << std::endl;
        std::cout << "World Size: " << communicator.size() << std::endl;
    }

    communicator.barrier();

    for (auto rank = 0; rank < communicator.size(); ++rank) {
        communicator.barrier();
        if (rank == communicator.rank()) {
            std::cout << "Rank " << communicator.rank() << ": Initialized" << std::endl;
        }
    }

    communicator.barrier();

    if (!communicator.rank()) {
        std::cout << "\n================ OpenMP ================" << std::endl;
        std::cout << "Number of threads: " << num_threads << std::endl;
    }

    communicator.barrier();

    auto num_total_pixels = image_width * image_height;
    auto num_remaining_pixels = num_total_pixels % communicator.size();

    auto num_split_pixels = num_total_pixels / communicator.size();
    if (!communicator.rank()) num_split_pixels += num_remaining_pixels;

    auto start_index = num_split_pixels * communicator.rank();
    if (communicator.rank()) start_index += num_remaining_pixels;

    auto stop_index = start_index + num_split_pixels;

    auto image = [&]() {
        std::vector<std::array<Scalar, 3>> colors(num_split_pixels);
        std::vector<std::size_t> rendered_samples(num_split_pixels);

        auto image_writer = [&](auto global_index, const auto& color) constexpr {
            auto local_index = global_index - start_index;
            auto prev_color = pbpt::tensor::elemwise(pbpt::math::square<Scalar>, pbpt::tensor::Vector<Scalar, 3>(colors[local_index]));
            auto next_color = pbpt::tensor::elemwise(pbpt::math::sqrt<Scalar>,
                                                     (prev_color * rendered_samples[local_index] + color) / (rendered_samples[local_index] + 1));
            colors[local_index] = next_color;
            ++rendered_samples[local_index];
        };

        if (communicator.rank()) std::cout.setstate(std::ios_base::badbit);

        boost::progress_timer progress_timer;
        boost::progress_display progress_display(num_samples);

        for (auto sample_index = 0; sample_index < num_samples; ++sample_index) {
            pbpt::renderer::path_tracer<Scalar, pbpt::tensor::Vector>(pbpt::scene::weekend::object, pbpt::scene::weekend::camera,
                                                                      pbpt::scene::weekend::background, image_width, image_height, start_index,
                                                                      stop_index, bernoulli_p, random_seed + sample_index, image_writer);

            communicator.barrier();

            std::vector<decltype(colors)> gathered_colors;
            if (!communicator.rank()) gathered_colors.resize(communicator.size());

            boost::mpi::gather(communicator, colors, gathered_colors, 0);

            if (!communicator.rank()) {
                auto image = gathered_colors | std::views::join;
                using namespace std::literals::string_literals;
                std::filesystem::path filename = "outputs/"s + std::to_string(sample_index) + ".ppm"s;
                std::filesystem::create_directories(filename.parent_path());
                pbpt::image::write_ppm(filename, image, image_width, image_height);
            }

            ++progress_display;
        }

        return colors;
    }();

    std::filesystem::path filename = "outputs/image.ppm";
    std::filesystem::create_directories(filename.parent_path());
    pbpt::image::write_ppm(filename, image, image_width, image_height);
}
