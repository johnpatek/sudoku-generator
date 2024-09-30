#ifndef _SUDOKU_HPP_
#define _SUDOKU_HPP_

#include <algorithm>
#include <array>
#include <execution>
#include <functional>
#include <random>
#include <set>
#include <unordered_map>
#include <string>

#include <iostream>

#define DEBUG(T) std::cerr << T << std::endl

namespace sudoku
{
    template <class ExecutionPolicy, class IteratorType, class RandomEngineType>
    IteratorType generate_solution(ExecutionPolicy &&policy, IteratorType first, IteratorType last, RandomEngineType &&random_engine)
    {
        static const std::array<int, 9> fill_values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        static const auto set_value = [](const std::set<int> &set, int index)
        {
            std::set<int>::const_iterator it = set.cbegin();
            while (std::distance(it, set.cend()) < index)
            {
                it++;
            }
            return *it;
        };
        static const auto apply_constraints = [&](std::set<int> &row, std::set<int> &column, std::set<int> &box)
        {
            std::array<int, 18> row_column_union;
            std::array<int, 27> row_column_box_union;
            std::array<int, 9> constraints;
            std::array<int, 9> possible_values;
            std::array<int, 18>::iterator row_column_union_last = std::set_union(row.begin(), row.end(), column.begin(), column.end(), row_column_union.begin());
            std::array<int, 27>::iterator row_column_box_union_last = std::set_union(row_column_union.begin(), row_column_union_last, box.begin(), box.end(), row_column_box_union.begin());
            std::array<int, 9>::iterator constraints_last = std::unique_copy(row_column_box_union.begin(), row_column_box_union_last, constraints.begin());
            std::array<int, 9>::iterator possible_values_last = std::set_difference(fill_values.begin(), fill_values.end(), constraints.begin(), constraints_last, possible_values.begin());
            return std::set<int>(possible_values.begin(), possible_values_last);
        };
        static const auto compare_states = [](const std::pair<int, std::set<int>> &lhs, const std::pair<int, std::set<int>> &rhs)
        {
            return lhs.second.size() < rhs.second.size();
        };
        std::array<int, 81> grid;
        std::vector<std::pair<int, std::set<int>>> states;
        std::array<std::set<int>, 9> row_sets;
        std::array<std::set<int>, 9> column_sets;
        std::array<std::set<int>, 9> box_sets;
        std::uniform_int_distribution<int> random_int;
        states.reserve(81);

        do
        {
            grid.fill(0);
            states.clear();
            while (states.size() < states.capacity())
            {
                int state_index = static_cast<int>(states.size());
                states.push_back(std::make_pair(state_index++, std::set<int>(fill_values.cbegin(), fill_values.cend())));
            }
            std::fill(policy, row_sets.begin(), row_sets.end(), std::set<int>());
            std::fill(policy, column_sets.begin(), column_sets.end(), std::set<int>());
            std::fill(policy, box_sets.begin(), box_sets.end(), std::set<int>());
            std::shuffle(states.begin(), states.end(), random_engine);
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
                    std::set<int> &target_row = row_sets[target_row_index];
                    std::set<int> &target_column = column_sets[target_column_index];
                    std::set<int> &target_box = box_sets[target_box_index];
                    if (!min_state->second.empty())
                    {
                        filling = true;
                        const int value = set_value(min_state->second, random_int(random_engine) % min_state->second.size());
                        target_row.emplace(value);
                        target_column.emplace(value);
                        target_box.emplace(value);
                        grid[min_state->first] = value;
                        states.erase(min_state);
                        for (auto &state : states)
                        {
                            const int row_index = state.first / 9;
                            const int column_index = state.first % 9;
                            const int box_index = (3 * (state.first / 27)) + ((state.first / 3) % 3);
                            if (row_index == target_row_index || column_index == target_column_index || box_index == target_box_index)
                            {
                                state.second = apply_constraints(row_sets[row_index], column_sets[column_index], box_sets[box_index]);
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
}

#endif