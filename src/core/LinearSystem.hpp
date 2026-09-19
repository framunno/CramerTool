#pragma once

#include <array>
#include <cstddef>

namespace cramer
{

/// A 3x3 matrix of coefficients, stored row by row.
///
/// Plain data on purpose: the whole core is free of Qt and of any framework, so
/// it can be unit tested, reused in another front end, or compiled anywhere a
/// C++20 compiler runs.
struct Matrix3
{
    std::array<std::array<double, 3>, 3> rows{};

    double& at(std::size_t row, std::size_t column) { return rows.at(row).at(column); }
    const double& at(std::size_t row, std::size_t column) const { return rows.at(row).at(column); }
};

/// The three constants on the right-hand side, or a solution triple (x, y, z).
struct Vector3
{
    std::array<double, 3> values{};

    double& operator[](std::size_t index) { return values.at(index); }
    const double& operator[](std::size_t index) const { return values.at(index); }

    double x() const { return values[0]; }
    double y() const { return values[1]; }
    double z() const { return values[2]; }
};

/// A system of three linear equations in three unknowns:
///
///     a11 x + a12 y + a13 z = b1
///     a21 x + a22 y + a23 z = b2
///     a31 x + a32 y + a33 z = b3
struct LinearSystem
{
    Matrix3 coefficients;
    Vector3 constants;
};

/// Determinant of `matrix`, expanded along the first row (Sarrus' rule).
double determinant(const Matrix3& matrix) noexcept;

/// Copy of `matrix` with one column replaced by `column`.
/// This is the substitution at the heart of Cramer's rule.
Matrix3 withColumnReplaced(const Matrix3& matrix, std::size_t column, const Vector3& values);

/// Largest absolute value in the matrix; used to scale the tolerance with which
/// determinants are compared against zero.
double largestMagnitude(const Matrix3& matrix) noexcept;

}  // namespace cramer
