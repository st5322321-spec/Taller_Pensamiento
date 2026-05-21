CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET   := taylor_demo
SRCS     := main.cpp
HEADERS  := taylor_math.hpp


.PHONY: all build run clean

all: build

 ── Compilar ─────────────────────────────────────────────────
build: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	@echo "──────────────────────────────────────────"
	@echo "  Compilando $(TARGET)..."
	@echo "──────────────────────────────────────────"
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
	@echo "  ✓ Compilación exitosa → ./$(TARGET)"
	@echo "──────────────────────────────────────────"

# ── Compilar y ejecutar ───────────────────────────────────────
run: build
	@echo "──────────────────────────────────────────"
	@echo "  Ejecutando ./$(TARGET)"
	@echo "──────────────────────────────────────────"
	@./$(TARGET)

# ── Limpiar ───────────────────────────────────────────────────
clean:
	@echo "  Eliminando artefactos..."
	@rm -f $(TARGET)
	@echo "  ✓ Listo."

