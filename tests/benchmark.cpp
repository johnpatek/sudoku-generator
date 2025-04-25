#include <sudoku.hpp>

#include <chrono>
#include <iostream>
#include <thread>

static std::atomic<int> index;
static std::array<std::array<int, 81>, 1000> array;
static void fill_array()
{
    int current_index = index.fetch_add(1);
    while (current_index < array.size())
    {

        current_index = index.fetch_add(1);
    }
}

static void print_grid(const std::array<int, 81> &grid)
{
    for (size_t index = 0; index < grid.size(); index++)
    {
        std::cout << grid[index];
        if ((index + 1) % 9 == 0)
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << " ";
        }
    }
}

int main(int argc, char **argv)
{
    const int thread_count = std::thread::hardware_concurrency();
    const unsigned int seed = std::random_device()();

    std::atomic<int> offset;
    std::array<std::array<int, 81>, 1000> grids;
    std::array<std::array<int, 81>, 1000> puzzles;
    std::vector<std::thread> threads;
    std::default_random_engine random_engine(seed);

    std::cerr << "benchmarking single grid generation" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    sudoku::generate_solution(std::execution::par_unseq, grids[0].begin(), grids[0].end(), random_engine);
    auto end = std::chrono::high_resolution_clock::now();
    auto usduration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cerr << "generated grid in " << usduration.count() << "us" << std::endl
              << std::endl;

    std::cerr << "benchmarking single puzzle generation" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    sudoku::generate_puzzle(std::execution::par_unseq, grids[0].begin(), grids[0].end(), puzzles[0].begin(), 64, random_engine);
    end = std::chrono::high_resolution_clock::now();
    auto nsduration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cerr << "generated puzzle in " << nsduration.count() << "ns" << std::endl
              << std::endl;

    std::cerr << "benchmarking 1000 grids on " << thread_count << " thread(s)" << std::endl;
    threads.reserve(thread_count);
    start = std::chrono::high_resolution_clock::now();
    while (threads.size() < threads.capacity())
    {
        threads.push_back(std::thread(
            [&]()
            {
                int current_offset = offset.fetch_add(1);
                while (current_offset < 1000)
                {
                    std::array<int, 81> &grid = grids[current_offset];
                    sudoku::generate_solution(std::execution::par_unseq, grid.begin(), grid.end(), random_engine);
                    current_offset = offset.fetch_add(1);
                }
            }));
    }
    for (std::thread &thread : threads)
    {
        thread.join();
    }
    end = std::chrono::high_resolution_clock::now();
    auto msduration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cerr << "generated 1000 grids in " << msduration.count() << "ms" << std::endl
              << std::endl;

    std::cerr << "benchmarking 1000 puzzles on " << thread_count << " thread(s)" << std::endl;
    threads.clear();
    start = std::chrono::high_resolution_clock::now();
    while (threads.size() < threads.capacity())
    {
        threads.push_back(std::thread(
            [&]()
            {
                int current_offset = offset.fetch_add(1);
                while (current_offset < 1000)
                {
                    std::array<int, 81> &grid = grids[current_offset];
                    std::array<int, 81> &puzzle = puzzles[current_offset];
                    sudoku::generate_puzzle(std::execution::par_unseq, grid.begin(), grid.end(), puzzle.begin(), 64, random_engine);
                    current_offset = offset.fetch_add(1);
                }
            }));
    }
    for (std::thread &thread : threads)
    {
        thread.join();
    }
    end = std::chrono::high_resolution_clock::now();
    usduration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cerr << "generated 1000 puzzles in " << usduration.count() << "us" << std::endl
              << std::endl;

    return 0;
}