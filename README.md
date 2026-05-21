# Taller_Pensamiento
# Funciones Matemáticas con Series de Taylor en C++

---

## Autores

| Nombre                      | Rol               |
|-----------------------------|-------------------|
| Juan Sebastian Tovar Estrada | Desarrollador     |
| Kaleth Mena                 | Desarrollador     |

---

## Descripción general

Este proyecto implementa desde cero, **sin usar `<cmath>`**, las siguientes
funciones matemáticas en C++ puro:

| Función        | Método de aproximación          |
|----------------|---------------------------------|
| `abs_val(x)`   | Condicional directo — exacta    |
| `sqrt_val(x)`  | Método de Newton-Raphson        |
| `exp_val(x)`   | Serie de Taylor + reducción     |
| `sin_val(x)`   | Serie de Taylor + reducción 2π  |
| `cos_val(x)`   | Serie de Taylor + reducción 2π  |

Cada función se compara contra su equivalente de `std::` y se muestra el
error absoluto, que en todos los casos es menor a **10⁻¹⁰**.

---

## Estructura del proyecto

```
.
├── taylor_math.hpp   ← Biblioteca: todas las funciones matemáticas
├── main.cpp          ← Programa principal: demos y tablas comparativas
└── Makefile          ← Sistema de compilación (build / run / clean)
```

---

## Requisitos

- Compilador `g++` con soporte para **C++17** (`-std=c++17`)
- GNU Make

---

## Comandos de uso

```bash
make build   # Compila el proyecto → genera el ejecutable ./taylor_demo
make run     # Compila (si es necesario) y ejecuta el programa
make clean   # Elimina el ejecutable generado
```

---

---

# Documentación línea por línea

---

## `taylor_math.hpp` — Biblioteca de funciones matemáticas

```cpp
#pragma once
```
> Directiva de inclusión única: garantiza que el header se procese una sola vez
> aunque sea incluido desde varios archivos. Equivalente a los guardas `#ifndef`.

```cpp
#include <stdexcept>
```
> Incluye las clases de excepción estándar. Se usa `std::domain_error`
> para señalar argumentos inválidos (ej. raíz de número negativo).

```cpp
namespace taylor {
```
> Todo el código se encierra en el espacio de nombres `taylor` para evitar
> colisiones con identificadores de otras bibliotecas.

---

### Constantes globales

```cpp
constexpr double PI = 3.14159265358979323846;
```
> Valor de π con 20 dígitos significativos. `constexpr` indica que es una
> constante evaluada en tiempo de compilación; no ocupa memoria en runtime.

```cpp
constexpr double E = 2.71828182845904523536;
```
> Número de Euler *e*, base del logaritmo natural. Mismo criterio que PI.

```cpp
constexpr int TERMS = 50;
```
> Número máximo de términos de las series de Taylor. En la práctica la
> convergencia se alcanza mucho antes (el bucle rompe cuando el término
> es menor a 10⁻¹⁶).

---

### `abs_val(x)` — Valor absoluto

```cpp
inline double abs_val(double x) {
```
> Función `inline`: el compilador puede sustituir la llamada por el cuerpo
> directamente, eliminando el overhead del call stack. Recibe un `double`.

```cpp
    return (x < 0.0) ? -x : x;
```
> Operador ternario: si `x` es negativo devuelve `-x` (lo hace positivo),
> en caso contrario devuelve `x` sin modificar. Resultado exacto, sin error.

---

### `factorial(n)` — Factorial auxiliar

```cpp
inline double factorial(int n) {
```
> Devuelve `n!` como `double` (no `int` ni `long`) para evitar desbordamiento
> de enteros. `20!` ya supera el rango de `unsigned long long`.

```cpp
    double result = 1.0;
    for (int i = 2; i <= n; ++i)
        result *= i;
    return result;
```
> Bucle iterativo desde 2 hasta `n`, acumulando el producto. Se omite el
> factor 1 (i=1) porque no cambia el resultado. Función auxiliar interna;
> no se exporta directamente en la interfaz pública.

---

### `power_int(base, exp)` — Potencia entera

```cpp
inline double power_int(double base, int exp) {
```
> Calcula `base^exp` donde el exponente es un entero (puede ser negativo).

```cpp
    if (exp == 0) return 1.0;
```
> Caso base: cualquier número elevado a 0 es 1, por definición matemática.

```cpp
    bool negative = (exp < 0);
    int  n        = negative ? -exp : exp;
```
> Si el exponente es negativo se trabaja con su valor absoluto y al final
> se toma el recíproco. Esto evita iterar con índices negativos.

```cpp
    for (int i = 0; i < n; ++i)
        result *= base;
```
> Multiplicación repetida `n` veces. Algoritmo O(n); suficiente para los
> exponentes pequeños usados en las series de Taylor.

```cpp
    return negative ? 1.0 / result : result;
```
> Si el exponente original era negativo se devuelve `1/resultado`.

---

### `sqrt_val(x)` — Raíz cuadrada (Newton-Raphson)

```cpp
inline double sqrt_val(double x) {
```
> Implementa √x usando el método iterativo de Newton-Raphson, que converge
> cuadráticamente (duplica los dígitos correctos en cada iteración).

```cpp
    if (x < 0.0)
        throw std::domain_error("sqrt: argumento negativo");
```
> La raíz cuadrada de un número negativo no es real. Se lanza una excepción
> del tipo `domain_error` (dominio matemático inválido) para informar al llamador.

```cpp
    if (x == 0.0) return 0.0;
```
> Caso trivial: √0 = 0. Evita la división por cero en la iteración siguiente.

```cpp
    double guess = x > 1.0 ? x / 2.0 : 1.0;
```
> Estimación inicial: para x > 1 se parte de x/2 (razonablemente cerca de √x);
> para x ≤ 1 se usa 1.0. Una buena semilla acelera la convergencia.

```cpp
    for (int i = 0; i < 100; ++i) {
        double next = 0.5 * (guess + x / guess);
```
> Fórmula de Newton: `next = ½ · (guess + x/guess)`.
> Derivada de f(g) = g² − x → g_siguiente = g − f(g)/f'(g) = ½(g + x/g).

```cpp
        if (abs_val(next - guess) < 1e-15) break;
        guess = next;
    }
```
> Criterio de parada: si la mejora es menor a 10⁻¹⁵ (límite de precisión
> `double`) se detiene. Máximo 100 iteraciones como salvaguarda.

```cpp
    return guess;
```
> Devuelve la mejor aproximación encontrada de √x.

---

### `exp_val(x)` — Función exponencial e^x (Taylor)

**Serie matemática:**
```
e^x = 1 + x + x²/2! + x³/3! + x⁴/4! + ...
```

```cpp
inline double exp_val(double x) {
```
> Calcula e^x usando la serie de Taylor con reducción de rango previa.

```cpp
    int    k    = 0;
    double xred = x;
    while (abs_val(xred) > 0.5) {
        xred /= 2.0;
        ++k;
    }
```
> **Reducción de rango:** divide x entre 2 repetidamente hasta que |x| ≤ 0.5.
> La serie converge muy rápido para valores pequeños. Se cuenta en `k` cuántas
> veces se dividió, para revertirlo después.

```cpp
    double term   = 1.0;
    double result = 1.0;
    for (int n = 1; n <= TERMS; ++n) {
        term   *= xred / n;
        result += term;
        if (abs_val(term) < 1e-16) break;
    }
```
> Serie de Taylor para `xred`. Se calcula cada término de forma incremental:
> `term_n = term_{n-1} * x/n`, evitando calcular factoriales completos.
> Se detiene cuando el término es despreciable (< 10⁻¹⁶).

```cpp
    for (int i = 0; i < k; ++i)
        result *= result;
```
> **Reversión del rango:** si `xred = x / 2^k`, entonces `e^x = (e^xred)^(2^k)`.
> Se eleva al cuadrado `k` veces: `result = result²` repetido k veces.

---

### `sin_val(x)` — Seno (Taylor)

**Serie matemática:**
```
sin(x) = x − x³/3! + x⁵/5! − x⁷/7! + ...
       = Σ (-1)^n · x^(2n+1) / (2n+1)!
```

```cpp
inline double sin_val(double x) {
```
> Calcula sin(x) en radianes mediante la serie de Taylor con reducción módulo 2π.

```cpp
    double twopi = 2.0 * PI;
    x -= twopi * (int)(x / twopi);
    if (x >  PI) x -= twopi;
    if (x < -PI) x += twopi;
```
> **Reducción de rango:** la función seno es periódica con período 2π.
> Primero se reduce `x` al rango (-2π, 2π) truncando la división entera.
> Luego se ajusta al intervalo (-π, π] para mayor precisión numérica.

```cpp
    double term   = x;
    double result = x;
    double x2     = x * x;
```
> Se precalcula `x²` para usarlo en cada paso sin recalcular.
> El primer término de la serie es simplemente `x`.

```cpp
    for (int n = 1; n <= TERMS; ++n) {
        term   *= -x2 / ((2*n) * (2*n + 1));
        result += term;
        if (abs_val(term) < 1e-16) break;
    }
```
> Recurrencia entre términos consecutivos:
> `term_n = term_{n-1} * (-x²) / ((2n)(2n+1))`
> El signo alterna automáticamente por el factor `-x²`.

---

### `cos_val(x)` — Coseno (Taylor)

**Serie matemática:**
```
cos(x) = 1 − x²/2! + x⁴/4! − x⁶/6! + ...
       = Σ (-1)^n · x^(2n) / (2n)!
```

```cpp
inline double cos_val(double x) {
```
> Calcula cos(x) en radianes. Misma estrategia que `sin_val`.

```cpp
    // Reducción de rango — idéntica a sin_val
    double twopi = 2.0 * PI;
    x -= twopi * (int)(x / twopi);
    if (x >  PI) x -= twopi;
    if (x < -PI) x += twopi;
```
> Reduce x al intervalo (-π, π] aprovechando la periodicidad de cos.

```cpp
    double term   = 1.0;
    double result = 1.0;
    double x2     = x * x;
```
> El primer término de la serie del coseno es 1 (a diferencia del seno que es x).

```cpp
    for (int n = 1; n <= TERMS; ++n) {
        term   *= -x2 / ((2*n - 1) * (2*n));
        result += term;
        if (abs_val(term) < 1e-16) break;
    }
```
> Recurrencia: `term_n = term_{n-1} * (-x²) / ((2n-1)(2n))`.
> Los denominadores son 2·1, 4·3, 6·5, ... que corresponden a (2n)!/(2(n-1))!.

```cpp
} // namespace taylor
```
> Cierra el espacio de nombres.

---

---

## `main.cpp` — Programa principal

```cpp
#include <iostream>
```
> Entrada/salida estándar: `std::cout`, `std::cin`, etc.

```cpp
#include <iomanip>
```
> Manipuladores de formato: `std::setw`, `std::setprecision`, `std::fixed`,
> `std::left`, `std::right`, `std::scientific`. Controlan el ancho de columnas
> y los decimales mostrados.

```cpp
#include <string>
```
> Clase `std::string` para manejar cadenas de texto en las funciones de
> presentación.

```cpp
#include <cmath>
```
> Se incluye **únicamente** para obtener los valores de referencia de `std::sin`,
> `std::cos`, `std::exp`, `std::sqrt` y comparar con nuestra implementación.
> No se usa para calcular nada dentro de las funciones propias.

```cpp
#include "taylor_math.hpp"
```
> Incluye nuestra biblioteca propia con todas las funciones matemáticas.
> Las comillas `""` indican que es un archivo local (no del sistema).

```cpp
using namespace taylor;
```
> Importa todos los nombres del namespace `taylor` al ámbito global.
> Permite escribir `abs_val(x)` en lugar de `taylor::abs_val(x)`.

---

### Función `separator(unit, width)`

```cpp
void separator(const std::string& unit = "─", int width = 72) {
    for (int i = 0; i < width; ++i) std::cout << unit;
    std::cout << '\n';
}
```
> Imprime una línea horizontal de `width` repeticiones del carácter `unit`.
> Por defecto usa `─` y ancho 72. Se usan `═` para encabezados principales
> y `─` para subencabezados.

---

### Función `header(title)`

```cpp
void header(const std::string& title) {
    separator("═");
    std::cout << "  " << title << '\n';
    separator("─");
}
```
> Imprime un bloque de encabezado: línea doble, título con sangría de 2 espacios
> y línea simple. Separa visualmente cada sección de la demo.

---

### Función `print_row(...)`

```cpp
void print_row(const std::string& label, double input,
               double our_val, double std_val)
```
> Imprime una fila de la tabla con 4 columnas:
> nombre de la función, valor de entrada, resultado Taylor y resultado `std::`.

```cpp
    double error = abs_val(our_val - std_val);
```
> Calcula el error absoluto entre nuestra aproximación y la referencia.

```cpp
    std::cout << std::left  << std::setw(18) << label
              << std::right << std::setw(10) << ...
```
> `std::left` / `std::right` alinean el texto dentro del ancho definido por
> `std::setw(n)`. Produce columnas alineadas independientemente del valor.

```cpp
              << std::scientific << std::setprecision(2) << error
```
> El error se muestra en notación científica (ej. `2.22e-16`) para que sea
> legible incluso cuando es muy pequeño.

---

### Funciones `demo_*()`

Cada función (`demo_abs`, `demo_sqrt`, `demo_exp`, `demo_sin`, `demo_cos`)
sigue el mismo patrón:

```cpp
void demo_xxx() {
    header("NOMBRE DE LA FUNCIÓN");  // Título de sección
    col_header();                    // Encabezado de columnas

    double vals[] = { ... };         // Valores de prueba
    for (double v : vals)
        print_row("nombre", v, xxx_val(v), std::xxx(v));
}
```

> El bucle for-range itera sobre el array de valores de prueba y llama a
> `print_row` con el resultado de nuestra función y el de `std::` para comparar.

---

### Función `show_taylor_terms(x, max_terms)`

```cpp
void show_taylor_terms(double x, int max_terms = 10) {
```
> Función educativa: muestra **término a término** cómo converge la serie de
> Taylor del seno, con el error acumulado en cada paso.

```cpp
    double x2    = x * x;
    double term  = x;
    double sum   = x;
    double exact = std::sin(x);
```
> Se inicializa la suma con el primer término (x) y se precalcula el valor
> exacto de referencia para medir el error en cada iteración.

```cpp
    for (int n = 0; n < max_terms; ++n) {
        // imprime n, term, sum, error
        term *= -x2 / ((2*n+2)*(2*n+3));
        sum  += term;
    }
```
> En cada vuelta imprime el estado actual y **luego** calcula el siguiente
> término. Se observa cómo el error decrece exponencialmente.

---

### Función `main()`

```cpp
int main() {
    std::cout << std::fixed;
```
> Activa el modo de punto fijo para `std::cout`. Todos los números flotantes
> se mostrarán con decimales fijos (no notación científica) salvo que se
> especifique `std::scientific` explícitamente.

```cpp
    demo_abs();    std::cout << '\n';
    demo_sqrt();   std::cout << '\n';
    demo_exp();    std::cout << '\n';
    demo_sin();    std::cout << '\n';
    demo_cos();    std::cout << '\n';
```
> Ejecuta cada demo en orden, con una línea en blanco de separación entre
> secciones para mejorar la legibilidad de la salida.

```cpp
    show_taylor_terms(PI / 4.0);
```
> Muestra la convergencia término a término para x = π/4 ≈ 0.785 rad,
> cuyo seno es √2/2 ≈ 0.7071067812.

```cpp
    return 0;
```
> Código de retorno 0 al sistema operativo: indica que el programa terminó
> sin errores.

---

---

## `Makefile` — Sistema de compilación

```makefile
CXX      := g++
```
> Define el compilador C++ a usar. `:=` es asignación inmediata en Make
> (se evalúa en el momento de la definición).

```makefile
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
```
> Flags de compilación:
> - `-std=c++17` → habilita el estándar C++17.
> - `-Wall` → activa todos los warnings básicos.
> - `-Wextra` → activa warnings adicionales.
> - `-O2` → optimización de nivel 2 (buen balance velocidad/tiempo de compilación).

```makefile
TARGET   := taylor_demo
SRCS     := main.cpp
HEADERS  := taylor_math.hpp
```
> Variables que definen el nombre del ejecutable, los archivos fuente y
> los headers. Cambiar `TARGET` aquí cambia el nombre en todo el Makefile.

```makefile
.PHONY: all build run clean
```
> Declara que `all`, `build`, `run` y `clean` son objetivos "falsos" (no
> son archivos reales). Make no buscará archivos con esos nombres; siempre
> ejecutará los comandos asociados.

```makefile
all: build
```
> El objetivo por defecto (ejecutado con `make` sin argumentos) es `build`.

```makefile
build: $(TARGET)
$(TARGET): $(SRCS) $(HEADERS)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
```
> Regla de compilación. Make solo recompila si `main.cpp` o `taylor_math.hpp`
> son más recientes que el ejecutable (`taylor_demo`). Evita compilaciones
> innecesarias.

```makefile
run: build
    @./$(TARGET)
```
> Primero garantiza que el ejecutable está compilado (`build`), luego lo
> ejecuta. El prefijo `@` suprime la impresión del comando en la terminal.

```makefile
clean:
    @rm -f $(TARGET)
```
> Elimina el ejecutable. `-f` hace que `rm` no falle si el archivo no existe.

---

## Salida esperada (extracto)

```
════════════════════════════════════════════════════════════════
  FUNCIONES MATEMÁTICAS — SERIES DE TAYLOR EN C++
════════════════════════════════════════════════════════════════

  EXPONENCIAL  e^x  (Serie de Taylor)
  e^x = 1 + x + x²/2! + x³/3! + ...
────────────────────────────────────────────────────────────────
Función            Entrada  │  Taylor (nuestro)  │  std::  │  Error abs.
────────────────────────────────────────────────────────────────
exp_val              1.0000  │    2.7182818285    │  2.7182818285  │  1.33e-15
exp_val             10.0000  │  22026.4657948067  │  22026.4657948067  │  1.82e-11
```

---

## Precisión numérica obtenida

| Función   | Error máximo observado |
|-----------|------------------------|
| `abs_val` | 0 (exacta)             |
| `sqrt_val`| ~10⁻¹⁶                 |
| `exp_val` | ~10⁻¹¹ (para x=10)     |
| `sin_val` | ~10⁻¹⁶                 |
| `cos_val` | ~10⁻¹⁶                 |

---

*Proyecto académico — C++ · Series de Taylor · 2025*
