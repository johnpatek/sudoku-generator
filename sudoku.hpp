/*
 * Copyright 2024 John R Patek Sr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _SUDOKU_HPP_
#define _SUDOKU_HPP_

#include <algorithm>
#include <array>
#include <execution>
#include <functional>
#include <list>
#include <random>
#include <set>
#include <unordered_map>
#include <string>

namespace sudoku
{
    template <class ExecutionPolicy, class IteratorType, class RandomEngineType>
    IteratorType generate_solution(ExecutionPolicy &&policy, IteratorType first, IteratorType last, RandomEngineType &&random_engine)
    {
        static const auto set_value = [](const std::set<int> &set, int index)
        {
            std::set<int>::const_iterator it = set.cbegin();
            while (std::distance(it, set.cend()) < index)
            {
                it++;
            }
            return *it;
        };

        static const auto initialize_grid = [](const std::array<int, 81> &grid, RandomEngineType &&random_engine)
        {
            
        };

        static const auto initialize = [&](std::array<int, 81> &array, RandomEngineType &&random_engine)
        {
            const std::array<int, 9> fill_values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            std::vector<std::pair<int, std::set<int>>> states;
            return std::move(std::list<std::pair<int, std::set<int>>>(states.begin(),states.end()));
        };

        static const auto compare_states = [](const std::pair<int, std::set<int>> &lhs, const std::pair<int, std::set<int>> &rhs)
        {
            return lhs.second.size() < rhs.second.size();
        };
        std::array<int, 81> grid;
        std::list<std::pair<int, std::set<int>>> states;
        std::uniform_int_distribution<int> random_int;

        // states.reserve(81);

        do
        {
            states = initialize(grid, random_engine);
            bool filling(true);
            while (filling)
            {
                filling = false;
                auto min_state = std::min_element(policy, states.begin(), states.end(), compare_states);
                if (min_state != states.end())
                {
                    const int target_row_index = min_state->first / 9;
                    const int target_column_index = min_state->first % 9;
                    const int target_box_index = (3 * (min_state->first / 27)) + ((min_state->first / 3) % 3);
                    if (!min_state->second.empty())
                    {
                        filling = true;
                        const int value = set_value(min_state->second, random_int(random_engine) % min_state->second.size());
                        grid[min_state->first] = value;
                        states.erase(min_state);
                        for (auto &state : states)
                        {
                            const int row_index = state.first / 9;
                            const int column_index = state.first % 9;
                            const int box_index = (3 * (state.first / 27)) + ((state.first / 3) % 3);
                            if (row_index == target_row_index || column_index == target_column_index || box_index == target_box_index)
                            {
                                state.second.erase(value);
                            }
                        }
                    }
                }
            }
        } while (std::find(policy, grid.cbegin(), grid.cend(), 0) != grid.cend());

        return std::copy(policy, grid.cbegin(), grid.cend(), first);
    }

    template <class IteratorType, class RandomEngineType>
    IteratorType generate_solution(IteratorType first, IteratorType last, RandomEngineType &&random_engine)
    {
        return generate_solution(std::execution::seq, first, last, random_engine);
    }

    template <int N, int... Rest>
    struct array_impl
    {
        static constexpr auto &value = array_impl<N - 1, N, Rest...>::value;
    };

    template <int... Rest>
    struct array_impl<0, Rest...>
    {
        static constexpr int value[] = {0, Rest...};
    };

    template <int... Rest>
    constexpr int array_impl<0, Rest...>::value[];

    template <int N>
    struct array_initializer
    {
        static constexpr auto &value = array_impl<N>::value;

        array_initializer() = delete;
        array_initializer(const array_initializer &) = delete;
        array_initializer(array_initializer &&) = delete;
    };

    template <class ExecutionPolicy, class SolutionIteratorType, class PuzzleIteratorType, class RandomEngineType>
    PuzzleIteratorType generate_puzzle(ExecutionPolicy &&policy, SolutionIteratorType first, SolutionIteratorType last, PuzzleIteratorType dest, size_t unknowns, RandomEngineType &&random_engine)
    {
        static constexpr int indices[81] = array_initializer<80>::value;
        std::array<int, 81> grid;
        std::array<int, 81> masked;

        std::copy_n(policy, first, 81, grid.begin());
        auto masked_end = std::sample(std::begin(indices), std::end(indices), masked.begin(), unknowns, random_engine);
        std::for_each(
            masked.begin(),
            masked_end,
            [&](int index)
            {
                grid[index] = 0;
            });

        return std::copy(policy, grid.begin(), grid.end(), dest);
    }

    template <class SolutionIteratorType, class PuzzleIteratorType, class RandomEngineType>
    PuzzleIteratorType generate_puzzle(SolutionIteratorType first, SolutionIteratorType last, PuzzleIteratorType dest, size_t unknowns, RandomEngineType &&random_engine)
    {
        return generate_puzzle(std::execution::seq, first, last, dest, unknowns, random_engine);
    }
}

#endif