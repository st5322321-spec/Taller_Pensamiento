#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>          // solo para comparar con std::
#include "taylor_math.hpp"

using namespace taylor;

// ── Helpers de presentación ──────────────────────────────────────────────────

void separator(const std::string& unit = "─", int width = 72) {
    for (int i = 0; i < width; ++i) std::cout << unit;
    std::cout << '\n';
}

void header(const std::string& title) {
    separator("═");
    std::cout << "  " << title << '\n';
    separator("─");
}

void print_row(const std::string& label,
               double input,
               double our_val,
               double std_val)
{
    double error = abs_val(our_val - std_val);
    std::cout << std::left  << std::setw(18) << label
              << std::right << std::setw(10) << std::setprecision(4) << input
              << "  │  "
              << std::setw(16) << std::setprecision(10) << our_val
              << "  │  "
              << std::setw(16) << std::setprecision(10) << std_val
              << "  │  "
              << std::scientific << std::setprecision(2) << error
              << std::fixed << '\n';
}

void col_header() {
    std::cout << std::left  << std::setw(18) << "Función"
              << std::right << std::setw(10) << "Entrada"
              << "  │  "
              << std::setw(16) << "Taylor (nuestro)"
              << "  │  "
              << std::setw(16) << "std:: (referencia)"
              << "  │  "
              << "Error abs.\n";
    separator("─");
}

// ── Demo de cada función ─────────────────────────────────────────────────────

void demo_abs() {
    header("VALOR ABSOLUTO  |x|");
    col_header();

    double vals[] = { 0.0, 3.7, -3.7, -100.0, 0.001 };
    for (double v : vals)
        print_row("abs_val", v, abs_val(v), std::abs(v));
}

void demo_sqrt() {
    header("RAÍZ CUADRADA  √x  (Newton-Raphson)");
    col_header();

    double vals[] = { 0.0, 1.0, 2.0, 9.0, 144.0, 0.25 };
    for (double v : vals)
        print_row("sqrt_val", v, sqrt_val(v), std::sqrt(v));
}

void demo_exp() {
    header("EXPONENCIAL  e^x  (Serie de Taylor)");
    std::cout << "  e^x = 1 + x + x²/2! + x³/3! + ...\n";
    separator("─");
    col_header();

    double vals[] = { 0.0, 0.5, 1.0, -1.0, 2.0, 5.0, -3.0, 10.0 };
    for (double v : vals)
        print_row("exp_val", v, exp_val(v), std::exp(v));
}

void demo_sin() {
    header("SENO  sin(x)  (Serie de Taylor)");
    std::cout << "  sin(x) = x − x³/3! + x⁵/5! − ...\n";
    separator("─");
    col_header();

    double vals[] = { 0.0, PI/6, PI/4, PI/3, PI/2, PI, 2*PI, -PI/2 };
    const char* labels[] = { "sin(0)", "sin(π/6)", "sin(π/4)",
                              "sin(π/3)", "sin(π/2)", "sin(π)",
                              "sin(2π)", "sin(-π/2)" };
    for (int i = 0; i < 8; ++i)
        print_row(labels[i], vals[i], sin_val(vals[i]), std::sin(vals[i]));
}

void demo_cos() {
    header("COSENO  cos(x)  (Serie de Taylor)");
    std::cout << "  cos(x) = 1 − x²/2! + x⁴/4! − ...\n";
    separator("─");
    col_header();

    double vals[] = { 0.0, PI/6, PI/4, PI/3, PI/2, PI, 2*PI, -PI };
    const char* labels[] = { "cos(0)", "cos(π/6)", "cos(π/4)",
                              "cos(π/3)", "cos(π/2)", "cos(π)",
                              "cos(2π)", "cos(-π)" };
    for (int i = 0; i < 8; ++i)
        print_row(labels[i], vals[i], cos_val(vals[i]), std::cos(vals[i]));
}

// ── Serie de Taylor visible (educativa) ──────────────────────────────────────

void show_taylor_terms(double x, int max_terms = 10) {
    header("SERIE DE TAYLOR  sin(x)  —  términos visibles");
    std::cout << "  x = " << x << " rad\n\n";
    std::cout << std::left << std::setw(6) << "n"
              << std::setw(22) << "Término"
              << std::setw(24) << "Suma parcial"
              << "Error acumulado vs std\n";
    separator("─");

    double x2     = x * x;
    double term   = x;
    double sum    = x;
    double exact  = std::sin(x);

    for (int n = 0; n < max_terms; ++n) {
        std::cout << std::left  << std::setw(6)  << n
                  << std::right << std::setw(22) << std::setprecision(10) << term
                  << std::setw(24) << sum
                  << std::scientific << std::setprecision(2)
                  << abs_val(sum - exact) << std::fixed << '\n';
        term *= -x2 / ((2*n+2)*(2*n+3));
        sum  += term;
    }
    std::cout << '\n';
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    std::cout << std::fixed;

    separator("═");
    std::cout << "  FUNCIONES MATEMÁTICAS — SERIES DE TAYLOR EN C++\n";
    separator("═");
    std::cout << '\n';

    demo_abs();    std::cout << '\n';
    demo_sqrt();   std::cout << '\n';
    demo_exp();    std::cout << '\n';
    demo_sin();    std::cout << '\n';
    demo_cos();    std::cout << '\n';

    show_taylor_terms(PI / 4.0);

    separator("═");
    std::cout << "  Fin de la demostración.\n";
    separator("═");

    return 0;
}

