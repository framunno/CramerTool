// Unit tests for the solver. The core has no Qt dependency, but QTest is
// already available in a Qt project and integrates with CTest, so it is the
// natural test framework here.

#include "core/CramerSolver.hpp"

#include <QtTest>

using cramer::LinearSystem;
using cramer::Matrix3;
using cramer::Solution;
using cramer::SolutionType;
using cramer::Vector3;

namespace
{

LinearSystem makeSystem(std::array<std::array<double, 4>, 3> rows)
{
    LinearSystem system;
    for (std::size_t row = 0; row < 3; ++row)
    {
        for (std::size_t column = 0; column < 3; ++column)
        {
            system.coefficients.at(row, column) = rows[row][column];
        }
        system.constants[row] = rows[row][3];
    }
    return system;
}

}  // namespace

class CramerSolverTests : public QObject
{
    Q_OBJECT

private slots:
    // --- determinant ------------------------------------------------------
    void determinantOfIdentityIsOne();
    void determinantMatchesSarrusRule();
    void determinantOfSingularMatrixIsZero();

    // --- unique solutions -------------------------------------------------
    void solvesTextbookSystem();
    void solvesSystemWithFractionalSolution();
    void reportsTheIntermediateDeterminants();

    // --- degenerate systems ----------------------------------------------
    void detectsInfinitelyManySolutions();
    void detectsInconsistentSystem();
    void detectsInconsistentSystemWhereEveryDeterminantIsZero();
    void detectsInconsistentSystemWithTwoParallelPlanes();

    // --- numerical behaviour ---------------------------------------------
    void handlesLargeCoefficients();
    void handlesSmallCoefficients();

    // --- rank -------------------------------------------------------------
    void rankCountsIndependentRows();
};

void CramerSolverTests::determinantOfIdentityIsOne()
{
    const Matrix3 identity{{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}}};
    QCOMPARE(cramer::determinant(identity), 1.0);
}

void CramerSolverTests::determinantMatchesSarrusRule()
{
    // | 6  1  1 |
    // | 4 -2  5 |  =  -306
    // | 2  8  7 |
    const Matrix3 matrix{{{{6, 1, 1}, {4, -2, 5}, {2, 8, 7}}}};
    QCOMPARE(cramer::determinant(matrix), -306.0);
}

void CramerSolverTests::determinantOfSingularMatrixIsZero()
{
    // Third row is the sum of the first two.
    const Matrix3 matrix{{{{1, 2, 3}, {4, 5, 6}, {5, 7, 9}}}};
    QCOMPARE(cramer::determinant(matrix), 0.0);
}

void CramerSolverTests::solvesTextbookSystem()
{
    //  2x +  y -  z =   8
    // -3x -  y + 2z = -11   ->  x = 2, y = 3, z = -1
    // -2x +  y + 2z =  -3
    const Solution solution = cramer::solve(makeSystem({{{2, 1, -1, 8}, {-3, -1, 2, -11}, {-2, 1, 2, -3}}}));

    QCOMPARE(solution.type, SolutionType::Unique);
    QVERIFY(qFuzzyCompare(solution.values.x(), 2.0));
    QVERIFY(qFuzzyCompare(solution.values.y(), 3.0));
    QVERIFY(qFuzzyCompare(solution.values.z(), -1.0));
}

void CramerSolverTests::solvesSystemWithFractionalSolution()
{
    // x + y + z = 1 ; x - y = 0 ; x + z = 0  ->  x = y = 1, z = -1
    const Solution solution = cramer::solve(makeSystem({{{1, 1, 1, 1}, {1, -1, 0, 0}, {1, 0, 1, 0}}}));

    QCOMPARE(solution.type, SolutionType::Unique);
    QVERIFY(qFuzzyCompare(solution.values.x(), 1.0));
    QVERIFY(qFuzzyCompare(solution.values.y(), 1.0));
    QVERIFY(qFuzzyCompare(solution.values.z(), -1.0));
}

void CramerSolverTests::reportsTheIntermediateDeterminants()
{
    const Solution solution = cramer::solve(makeSystem({{{2, 1, -1, 8}, {-3, -1, 2, -11}, {-2, 1, 2, -3}}}));

    // D = -1, and x = Dx/D must hold for each unknown.
    QCOMPARE(solution.determinant, -1.0);
    for (std::size_t i = 0; i < 3; ++i)
    {
        QVERIFY(qFuzzyCompare(solution.numerators[i] / solution.determinant, solution.values[i]));
    }
}

void CramerSolverTests::detectsInfinitelyManySolutions()
{
    // The second equation is twice the first, the third is independent:
    // the planes intersect along a line.
    const Solution solution = cramer::solve(makeSystem({{{1, 1, 1, 6}, {2, 2, 2, 12}, {1, -1, 0, 0}}}));

    QCOMPARE(solution.type, SolutionType::Infinite);
    QCOMPARE(solution.determinant, 0.0);
}

void CramerSolverTests::detectsInconsistentSystem()
{
    // Same left-hand side, different constants: parallel planes.
    const Solution solution = cramer::solve(makeSystem({{{1, 1, 1, 1}, {2, 2, 2, 5}, {1, -1, 0, 0}}}));

    QCOMPARE(solution.type, SolutionType::Inconsistent);
    QCOMPARE(solution.determinant, 0.0);
}

void CramerSolverTests::detectsInconsistentSystemWhereEveryDeterminantIsZero()
{
    // The case that breaks the "D = Dx = Dy = Dz = 0 means infinitely many
    // solutions" shortcut: three identical planes with three different
    // constants have no common point, yet every determinant vanishes.
    const Solution solution = cramer::solve(makeSystem({{{1, 1, 1, 1}, {1, 1, 1, 2}, {1, 1, 1, 3}}}));

    QCOMPARE(solution.determinant, 0.0);
    QCOMPARE(solution.numerators[0], 0.0);
    QCOMPARE(solution.numerators[1], 0.0);
    QCOMPARE(solution.numerators[2], 0.0);
    QCOMPARE(solution.type, SolutionType::Inconsistent);
}

void CramerSolverTests::detectsInconsistentSystemWithTwoParallelPlanes()
{
    const Solution solution = cramer::solve(makeSystem({{{1, 1, 1, 1}, {1, 1, 1, 2}, {0, 0, 1, 0}}}));
    QCOMPARE(solution.type, SolutionType::Inconsistent);
}

void CramerSolverTests::handlesLargeCoefficients()
{
    // Scaling every equation by 1e6 must not change the solution, and must not
    // make the (scaled) tolerance mistake a valid system for a singular one.
    const Solution solution =
        cramer::solve(makeSystem({{{2e6, 1e6, -1e6, 8e6}, {-3e6, -1e6, 2e6, -11e6}, {-2e6, 1e6, 2e6, -3e6}}}));

    QCOMPARE(solution.type, SolutionType::Unique);
    QVERIFY(qFuzzyCompare(solution.values.x(), 2.0));
    QVERIFY(qFuzzyCompare(solution.values.y(), 3.0));
    QVERIFY(qFuzzyCompare(solution.values.z(), -1.0));
}

void CramerSolverTests::handlesSmallCoefficients()
{
    const Solution solution = cramer::solve(
        makeSystem({{{2e-6, 1e-6, -1e-6, 8e-6}, {-3e-6, -1e-6, 2e-6, -11e-6}, {-2e-6, 1e-6, 2e-6, -3e-6}}}));

    QCOMPARE(solution.type, SolutionType::Unique);
    QVERIFY(qFuzzyCompare(solution.values.x(), 2.0));
    QVERIFY(qFuzzyCompare(solution.values.y(), 3.0));
    QVERIFY(qFuzzyCompare(solution.values.z(), -1.0));
}

void CramerSolverTests::rankCountsIndependentRows()
{
    const Matrix3 independent{{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}}};
    QCOMPARE(cramer::rank(independent), 3);

    const Matrix3 twoIndependent{{{{1, 2, 3}, {2, 4, 6}, {0, 1, 0}}}};
    QCOMPARE(cramer::rank(twoIndependent), 2);

    const Matrix3 oneIndependent{{{{1, 1, 1}, {2, 2, 2}, {3, 3, 3}}}};
    QCOMPARE(cramer::rank(oneIndependent), 1);

    const Matrix3 zero{};
    QCOMPARE(cramer::rank(zero), 0);
}

QTEST_APPLESS_MAIN(CramerSolverTests)
#include "CramerSolverTests.moc"
