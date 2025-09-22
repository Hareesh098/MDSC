# MDSC — Molecular Dynamics Simulator with CUDA (Flat `src/`)

A modular C/CUDA Lennard–Jones MD starter kit demonstrating **best research software practices**. This version uses a **flat `src/`** directory: all `.c` and `.cu` files at one level for clarity in workshops.

**Author:** Harish Charan • **Email:** charan.harish@gmail.com • **Repo:** https://github.com/Hareesh098/MDSC

## Features
- Lennard–Jones MD (reduced units: σ = ε = m = 1)
- Periodic boundaries, cell-list neighbor search (no double counting)
- Velocity–Verlet integrator
- Thermostats: **Berendsen** and **Andersen**
- Optional CUDA acceleration (`-DUSE_CUDA=ON`) with LJ forces + potential energy reduction
- File-based inputs (`in.mdsc`) — simple `key value` pairs
- Outputs:
  - **XYZ** trajectory (every 10 steps by default)
  - **Log** (plain text)
  - **Thermodynamics** `.dat` with header and **time-first** columns: `time KE PE T Etot`
- Build: **CMake** + **CTest**, warnings-as-errors, optional sanitizers/LTO/native flags
- Quality: clang-format, clang-tidy, EditorConfig, pre-commit
- Docs: **Doxygen** HTML + PDF (LaTeX), GitHub Pages deploy via CI
- Tests: Unity-based unit tests

## Layout
```
MDSC/
├─ CMakeLists.txt
├─ include/mdsc/*.h
├─ src/*.c, *.cu     # flat layout
├─ tests/*.c
├─ third_party/unity/*
├─ examples/lj_berendsen/*
├─ docs/Doxyfile
├─ .github/workflows/ci.yml
└─ configs (.clang-format/.clang-tidy/.editorconfig/.pre-commit-config.yaml)
```

## Quick Start (file-driven)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/mdsc examples/lj_berendsen/in.mdsc
```

## Build types & flags
- `CMAKE_BUILD_TYPE`: `Release`, `RelWithDebInfo`, `Debug`, `MinSizeRel`
- Options:
  - `-DUSE_CUDA=ON` — compile CUDA path (CI compiles without GPU)
  - `-DMDSC_ENABLE_DEBUG_SYMBOLS=ON` → `-g3`
  - `-DMDSC_ENABLE_PROFILING=ON` → `-pg` (+ link)
  - `-DMDSC_ENABLE_SANITIZERS=ON` (Debug) → Address/UB sanitizers
  - `-DMDSC_NATIVE_OPT=ON` → `-march=native`
  - `-DMDSC_LTO=ON` → Interprocedural Optimization (Release/RelWithDebInfo)

## Input file format (`.mdsc`)
Keys (one per line, `#` comments allowed):
```
n, rho, steps, dt, rc, cuda (0|1)
thermo (none|berendsen|andersen), T0, tau, nu
xyz <file>, log <file>, thermo_out <file>, thermo_every <int>
```

## Example
```text
# examples/lj_berendsen/in.mdsc
n 256
rho 0.7
steps 2000
dt 0.004
rc 2.5

thermo berendsen
T0 1.0
tau 2.0

xyz traj.xyz
log run.log
thermo_out thermo.dat
thermo_every 50

cuda 0
```

## Publish to GitHub
```bash
git init
git config user.name "Harish Charan"
git config user.email "charan.harish@gmail.com"
git remote add origin https://github.com/Hareesh098/MDSC.git
git add .
git commit -m "MDSC: flat src layout, CPU+CUDA, file-driven inputs"
git branch -M main
git push -u origin main
```

## Docs website + PDF
- CI builds Doxygen HTML + PDF and deploys to **GitHub Pages**.
- After first push, enable **Settings → Pages → Source: GitHub Actions**.
