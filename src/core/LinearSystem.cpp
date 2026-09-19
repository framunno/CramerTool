#include "core/LinearSystem.hpp"

#include <algorithm>
#include <cmath>

namespace cramer
{

double determinant(const Matrix3& m) noexcept
{
    // | a b c |
    // | d e f |  =  a(ei - fh) - b(di - fg) + c(dh - eg)
    // | g h i |
    return m.rows[0][0] * ((m.rows[1][1] * m.rows[2][2]) - (m.rows[1][2] * m.rows[2][1])) -
           m.rows[0][1] * ((m.rows[1][0] * m.rows[2][2]) - (m.rows[1][2] * m.rows[2][0])) +
           m.rows[0][2] * ((m.rows[1][0] * m.rows[2][1]) - (m.rows[1][1] * m.rows[2][0]));
}

Matrix3 withColumnReplaced(const Matrix3& matrix, std::size_t column, const Vector3& values)
{
    Matrix3 result = matrix;
    for (std::size_t row = 0; row < 3; ++row)
    {
        result.at(row, column) = values[row];
    }
    return result;
}

double largestMagnitude(const Matrix3& matrix) noexcept
{
    double largest = 0.0;
    for (const auto& row : matrix.rows)
    {
        for (const double value : row)
        {
            largest = std::max(largest, std::abs(value));
        }
    }
    return largest;
}

}  // namespace cramer
