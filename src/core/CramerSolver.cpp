#include "core/CramerSolver.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace cramer
{

namespace
{

// Relative tolerance used to decide whether a computed value is zero. Doubles
// carry about 16 significant digits; 1e-12 leaves a wide margin for the
// rounding introduced by a handful of multiplications, while still rejecting
// values that are genuinely non-zero.
constexpr double kRelativeTolerance = 1e-12;

// A 3x3 determinant is a sum of products of three entries, so its magnitude -
// and the rounding error that comes with it - grows with the cube of the
// largest entry. The threshold has to follow the same law, otherwise a system
// written with tiny coefficients (1e-6) would look singular and one written
// with huge ones (1e6) would never look singular.
double determinantTolerance(double magnitude) noexcept
{
    return kRelativeTolerance * magnitude * magnitude * magnitude;
}

// Gaussian elimination compares single entries, which grow linearly.
double pivotTolerance(double magnitude) noexcept
{
    return kRelativeTolerance * magnitude;
}

// Gaussian elimination with partial pivoting. `rows` holds `columns` values per
// row; returns how many independent rows are left after elimination.
int rankOf(std::vector<std::vector<double>> rows, std::size_t columns, double tolerance)
{
    int rank = 0;
    for (std::size_t column = 0; column < columns && rank < static_cast<int>(rows.size()); ++column)
    {
        // Pivot on the largest remaining entry of this column: the standard way
        // to keep the elimination numerically stable.
        std::size_t pivot = static_cast<std::size_t>(rank);
        for (std::size_t row = pivot + 1; row < rows.size(); ++row)
        {
            if (std::abs(rows[row][column]) > std::abs(rows[pivot][column]))
            {
                pivot = row;
            }
        }
        if (std::abs(rows[pivot][column]) <= tolerance)
        {
            continue;  // this column brings nothing new
        }
        std::swap(rows[static_cast<std::size_t>(rank)], rows[pivot]);

        // Clear the column below the pivot.
        for (std::size_t row = static_cast<std::size_t>(rank) + 1; row < rows.size(); ++row)
        {
            const double factor = rows[row][column] / rows[static_cast<std::size_t>(rank)][column];
            for (std::size_t c = column; c < rows[row].size(); ++c)
            {
                rows[row][c] -= factor * rows[static_cast<std::size_t>(rank)][c];
            }
        }
        ++rank;
    }
    return rank;
}

std::vector<std::vector<double>> toRows(const Matrix3& matrix, const Vector3* constants)
{
    std::vector<std::vector<double>> rows;
    rows.reserve(3);
    for (std::size_t row = 0; row < 3; ++row)
    {
        std::vector<double> values(matrix.rows[row].begin(), matrix.rows[row].end());
        if (constants != nullptr)
        {
            values.push_back((*constants)[row]);
        }
        rows.push_back(std::move(values));
    }
    return rows;
}

}  // namespace

int rank(const Matrix3& matrix)
{
    return rankOf(toRows(matrix, nullptr), 3, pivotTolerance(largestMagnitude(matrix)));
}

int rank(const LinearSystem& system)
{
    double magnitude = largestMagnitude(system.coefficients);
    for (const double constant : system.constants.values)
    {
        magnitude = std::max(magnitude, std::abs(constant));
    }
    return rankOf(toRows(system.coefficients, &system.constants), 4, pivotTolerance(magnitude));
}

Solution solve(const LinearSystem& system)
{
    Solution solution;
    solution.determinant = determinant(system.coefficients);
    for (std::size_t column = 0; column < 3; ++column)
    {
        solution.numerators[column] = determinant(withColumnReplaced(system.coefficients, column, system.constants));
    }

    const double tolerance = determinantTolerance(largestMagnitude(system.coefficients));
    if (std::abs(solution.determinant) > tolerance)
    {
        // Cramer's rule proper.
        solution.type = SolutionType::Unique;
        for (std::size_t i = 0; i < 3; ++i)
        {
            solution.values[i] = solution.numerators[i] / solution.determinant;
        }
        return solution;
    }

    // Singular matrix: Rouché-Capelli decides between no solution and infinitely
    // many. Equal ranks mean the constants are consistent with the equations.
    solution.type = rank(system.coefficients) == rank(system) ? SolutionType::Infinite : SolutionType::Inconsistent;
    return solution;
}

}  // namespace cramer
