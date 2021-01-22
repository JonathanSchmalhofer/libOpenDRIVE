#pragma once

#include <map>
#include <memory>
#include <stddef.h>
#include <vector>

namespace odr
{
struct Poly3
{
    Poly3() = default;
    Poly3(double s_start, double a, double b, double c, double d);
    virtual ~Poly3() = default;

    double get(double s) const;
    double get_grad(double s) const;

    std::vector<double> approximate_linear(double eps, double s0, double s1) const;

    void negate();

    double a = 0, b = 0, c = 0, d = 0;
};

struct CubicSpline
{
    CubicSpline() = default;
    virtual ~CubicSpline() = default;

    size_t size() const;
    double get(double s) const;
    double get_grad(double s) const;

    CubicSpline negate() const;
    CubicSpline add(const CubicSpline& other) const;
    Poly3       get_poly(double s) const;

    std::map<double, Poly3> s_start_to_poly;
};

} // namespace odr