# Interactive photovoltaic simulation (Wokwi)

**Project URL supplied by the authors:** https://wokwi.com/projects/479082581022049281

The URL has been recorded in the provided `wokwi-project.txt`. It has not been independently confirmed to resolve and display the same version at the time this repository was assembled. The local source snapshot is in `wokwi_simulation/`:

- `diagram.json`: ESP32 DevKit and the custom solar component, analog connections and control UART.
- `sketch.ino`: simulated-controller firmware, data acquisition and telemetry.
- `mppt_neural.h`, `mppt_weights.h`: neural implementation and fixed initial parameters.
- `solar.chip.c`, `solar.chip.json`: custom digital photovoltaic source and component description.

## Interactive examination

1. Visit the URL and compare the displayed project to the archived source files.
2. Set the solar component to automatic mode, a single module/string and load slider at 50 (neutral disturbance); preserve the published analog ranges and gains.
3. In the archived Wokwi `sketch.ino`, `MPPT_MODE=0` is P&O, `1` INC, `2` adaptive P&O, `3` NN-V. When `MPPT_MODE=3`, `MPPT_ONLINE_LEARNING=0` freezes parameter updates and `=1` enables them. `MPPT_ACTIVE_VERIFICATION=1` is the active verification setting used in the NN-V comparison.
4. Compare received `# CONFIG` and `# EXP_START` messages with the intended mode. Keep a copy of the telemetry log together with the simulator configuration and date.
5. A current online Wokwi run is **not** automatically equivalent to a historical log. Archived Wokwi trajectories can be unmatched, some old logs lack positive firmware identifiers, and Wokwi files are distinct from host executable sources.

The digital source has a virtual load percentage input. This is **not** a switching converter and does not establish physical electrical efficiency, CPU latency, hardware stability or partial-shading global-MPP tracking.

## Build reproducibility boundary

A verified, historic ESP32 `.bin`/`.elf` file, custom-chip `.wasm`, original toolchain lockfile and a one-to-one mapping of every old Wokwi trace to a binary are not present. Custom chip and firmware source are present. Building a new project can aid exploration but must not be presented as a byte-identical reconstruction of historical firmware without further evidence.
