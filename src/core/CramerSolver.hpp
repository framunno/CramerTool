#pragma once

#include "core/LinearSystem.hpp"

#include <cstdint>

namespace cramer
{

/// What a system of three equations can look like.
enum class SolutionType : std::uint8_t
{
    Unique,        ///< The three planes meet in exactly one point (determinant != 0).
    Infinite,      ///< They share a line or a plane: infinitely many solutions.
    Inconsistent,  ///< They have no common point at all: no solution.
};

/// Outcome of solving a system, including the determinants that produced it.
/// The intermediate values are part of the result on purpose: showing D, Dx, Dy
/// and Dz is what makes the tool useful for studying Cramer's rule.
struct Solution
{
    SolutionType type = SolutionType::Unique;
    Vector3 values{};      ///< (x, y, z); meaningful only when type == Unique.
    double determinant{};  ///< D, the determinant of the coefficient matrix.
    Vector3 numerators{};  ///< Dx, Dy, Dz: determinants with one column replaced.

    bool isUnique() const noexcept { return type == SolutionType::Unique; }
};

/// Solves `system` with Cramer's rule.
///
/// When D != 0 the solution is x = Dx/D, y = Dy/D, z = Dz/D.
///
/// When D == 0 Cramer's rule alone cannot tell "infinitely many solutions" from
/// "no solution": comparing Dx, Dy, Dz against zero is a popular shortcut, and
/// it is wrong (three identical planes with different constants give D = Dx =
/// Dy = Dz = 0 and yet no solution). The classification is therefore done with
/// the Rouché-Capelli theorem, by comparing the rank of the coefficient matrix
/// with the rank of the augmented matrix.
///
/// Comparisons against zero use a tolerance scaled by the magnitude of the
/// input, because coefficients are floating-point numbers.
Solution solve(const LinearSystem& system);

/// Rank of the coefficient matrix (0 to 3), computed by Gaussian elimination
/// with partial pivoting.
int rank(const Matrix3& matrix);

/// Rank of the augmented matrix [A|b] (0 to 3).
int rank(const LinearSystem& system);

}  // namespace cramer
