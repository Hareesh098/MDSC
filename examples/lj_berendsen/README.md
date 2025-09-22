# Example: LJ with Berendsen thermostat

Run from repo root:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/mdsc examples/lj_berendsen/in.mdsc
```
Outputs in the current working directory: `traj.xyz`, `run.log`, `thermo.dat`.
