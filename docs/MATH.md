# The mathematics behind CramerTool

## 1. Cramer's rule

A system of three equations in three unknowns

```
a₁x + b₁y + c₁z = d₁
a₂x + b₂y + c₂z = d₂
a₃x + b₃y + c₃z = d₃
```

is the matrix equation **A·v = b**. Cramer's rule builds four determinants: the
determinant of the coefficient matrix, and three copies of it with one column
replaced by the constants.

```
      | a₁ b₁ c₁ |          | d₁ b₁ c₁ |          | a₁ d₁ c₁ |          | a₁ b₁ d₁ |
D  =  | a₂ b₂ c₂ |   Dx  =  | d₂ b₂ c₂ |   Dy  =  | a₂ d₂ c₂ |   Dz  =  | a₂ b₂ d₂ |
      | a₃ b₃ c₃ |          | d₃ b₃ c₃ |          | a₃ d₃ c₃ |          | a₃ b₃ d₃ |
```

If **D ≠ 0** the system has exactly one solution:

```
x = Dx / D        y = Dy / D        z = Dz / D
```

Geometrically each equation is a plane, and D ≠ 0 means the three planes meet in
a single point.

In the code: `determinant()` and `withColumnReplaced()` in
[`src/core/LinearSystem.cpp`](../src/core/LinearSystem.cpp), `solve()` in
[`src/core/CramerSolver.cpp`](../src/core/CramerSolver.cpp).

## 2. When D = 0, Cramer's rule stops

A zero determinant means the three planes are not independent. Two things can
happen, and they are very different for the user:

- **infinitely many solutions** — the planes share a line or coincide;
- **no solution** — the planes are incompatible (think of two parallel planes).

A widespread shortcut claims that D = Dx = Dy = Dz = 0 means "infinitely many
solutions". **It is false.** Take

```
x + y + z = 1
x + y + z = 2
x + y + z = 3
```

Every determinant is zero, because every matrix involved has three identical
rows or columns of constants that cancel out. Yet the system is plainly
impossible: a number cannot be 1 and 2 at the same time. CramerTool has a unit
test for exactly this case
(`detectsInconsistentSystemWhereEveryDeterminantIsZero`).

## 3. Rouché-Capelli

The correct criterion compares two ranks:

- the rank of the coefficient matrix **A**;
- the rank of the augmented matrix **[A|b]**, that is **A** with the constants
  added as a fourth column.

| Condition | Meaning |
|---|---|
| rank(A) = rank([A\|b]) = 3 | one solution (this is the case D ≠ 0) |
| rank(A) = rank([A\|b]) < 3 | infinitely many solutions |
| rank(A) < rank([A\|b]) | no solution |

Intuitively: if adding the constants column raises the rank, then the constants
carry information that contradicts the equations.

CramerTool computes both ranks with Gaussian elimination and partial pivoting
(`rank()` in [`src/core/CramerSolver.cpp`](../src/core/CramerSolver.cpp)):
at each step it picks the largest available pivot, which is the standard way to
limit the growth of rounding error.

## 4. Floating point: when is a determinant "zero"?

Coefficients are `double`s, so exact comparison against zero is meaningless: a
singular system computed from decimal input may produce a determinant of
`1e-17` instead of `0`, and a perfectly regular system written in millionths may
produce `1e-18` while being far from singular.

The tolerance therefore scales with the data:

| Comparison | Tolerance | Why |
|---|---|---|
| determinant vs zero | `1e-12 · m³` | a 3×3 determinant is a sum of products of three entries, so it grows with the cube of the largest entry `m` |
| pivot vs zero, during elimination | `1e-12 · m` | elimination compares single entries, which grow linearly |

Two unit tests pin this behaviour down: the same system scaled by 10⁶ and by
10⁻⁶ must still be reported as having a unique solution
(`handlesLargeCoefficients`, `handlesSmallCoefficients`).

## 5. Why Cramer's rule at all?

For 3×3 systems it is compact, easy to verify by hand and it exposes the
determinants, which is exactly what a teaching tool wants. It is not, however,
the right tool for large systems: the number of operations grows factorially,
and Gaussian elimination (already used here for the ranks) or an LU
factorisation is both faster and numerically better behaved. For a 3×3 system
the difference is irrelevant; for a 10×10 one it is the difference between
microseconds and millions of years.
