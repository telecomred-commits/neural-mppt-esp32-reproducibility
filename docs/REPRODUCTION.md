# Reproduction protocol / Protocolo de reproducción

The primary, detailed protocol supplied by the investigators is [`supplementary/PROTOCOLO_ORIGINAL_ES.md`](../supplementary/PROTOCOLO_ORIGINAL_ES.md); it is retained verbatim. This document maps that protocol to concrete commands. **Run in a disposable checkout, not in the archival snapshot, because original scripts rewrite some output files.**

## Requirements

- Python 3, NumPy, Matplotlib; the original NN-I/V6 training source has now been recovered from V5; it requires scikit-learn. Run `python tools/reproduce_nn_i_training.py` from the repository root to regenerate and byte-compare the 65 weights and original training report in a temporary copy.
- `gcc` (C11), `g++` (C++11). Windows users can use WSL or an equivalent toolchain. The test machine for this audit had Python 3.13.5, NumPy 2.3.5, Matplotlib 3.10.8, GCC/G++ 14.2.0. These are **audit-machine versions**, not claimed as the original experimental toolchain.
- Wokwi online simulation requires external access. The archived `.c`/`.json` chip files support rebuilding the custom component; no frozen `.wasm`, ESP32 `.bin`, `.elf` or historical toolchain lockfile was supplied.

## 0. Integrity (read-only)

From repository root:

```bash
python tools/verify_archive.py
```

## 1. Initial 350 host simulations (NN-I/V6, P&O, INC, adaptive P&O)

```bash
cd supplementary/initial_host_campaign
python tools/run_host.py 10
```

Five controller variants × seven profiles × seeds 1–10. Source: `referencia/sketch.ino`, `mppt_neural.h`, `mppt_weights.h`, `solar.chip.c`; test harness in `tests/`, canonical schedules in `perfiles/`. `resultados_host/` contains reference logs, metrics and aggregate efficiency. The script checks source SHA-256 before compiling. Full energy measurement uses the **analytical maximum of the simulated plant**, never the neural voltage reference as the validation target.

## 2. Calibrated NN-C (V8)

```bash
cd supplementary/calibration_NN_C
python tools/train_v8.py
```

This rebuilds constrained output-layer calibration from four stored IV sweeps and five development (frozen) traces. Do not treat these development traces or the previously inspected 600 W/m² / 37 °C condition as blind cross-module validation. Verify generated weights against the archived firmware header; the script may overwrite local report files. NN-V uses the calibrated NN-C weights; it is a separate guarded-update method, not a new offline neural training run.

## 3. Matched 880 host simulations (seeds 11–20)

```bash
cd supplementary/matched_extension
python benchmark_classical.py --project neural_bench --chip ../initial_host_campaign/referencia/solar.chip.c --output classical_matched
python benchmark_affine.py --project neural_bench --chip ../initial_host_campaign/referencia/solar.chip.c --output affine_matched
python neural_bench/tools/bench_v9.py --chip ../initial_host_campaign/referencia/solar.chip.c --seed-start 11 --seeds 10 --profiles S0 S1 S2 S3 S4 R1 R2 H0 H1 H2 H3 --variants V8_FROZEN V8_ONLINE V9_FROZEN V9_ONLINE --label comparacion_congelada
```

These commands produce 330 classical, 110 affine, 440 neural simulation records. Identical seed labels do **not** guarantee byte-identical logs on all compiler/toolchain builds. Under the audit environment some NN-V update counters differed while aggregate MPPT efficiencies remained very close. See the audit report.

## 4. Passive verification ablation, 60 host simulations

```bash
cd supplementary/matched_extension
python neural_bench/tools/bench_v9.py --chip ../initial_host_campaign/referencia/solar.chip.c --seed-start 11 --seeds 10 --profiles S0 S2 S4 --variants V9_PASSIVE_FROZEN V9_PASSIVE_ONLINE --label ablacion_pasiva
```

## 5. Rebuild manuscript aggregate data and diagnostic replay

```bash
cd supplementary/matched_extension
python prepare_article_data.py
cd ../fixed_input_diagnostic
python tools/investigate.py
```

The first script recomputes comparison tables, contrasts and 29/7491 Wokwi inventory from stored sources. The second inspects eight records and produces 32 input-replay combinations. **Equal-input replay is not a closed-loop MPPT efficiency test.**

## 6. Wokwi (independent route)

1. Open <https://wokwi.com/projects/479082581022049281>.
2. Confirm that project files and code match `wokwi_simulation/` before testing. Wokwi's version is mutable; the snapshot provides source provenance.
3. Set `MPPT_MODE` to 0 (P&O), 1 (INC), 2 (adaptive P&O), or 3 (NN-V) in its `sketch.ino`. NN-V `MPPT_ONLINE_LEARNING` is 0 (frozen) or 1 (online). The archived Wokwi copy defaults to frozen and active verification.
4. Control the digital source through the slider settings, respecting the command range and its handshake. Compare the recorded telemetry only after fixing initial conditions and observation windows.
5. The supplied archives do not contain the compiled ESP32 firmware or custom-chip WASM artifact. Do not assert firmware timing, computational cost, or complete Wokwi replay identity without those binary/build details.

See Wokwi's official [custom-chip compilation guide](https://docs.wokwi.com/guides/custom-chips-to-wasm) and [project configuration guide](https://docs.wokwi.com/vscode/project-config). A standalone VS Code project may require `wokwi.toml`, a compiled `.wasm` chip, and ESP32 `.bin`/`.elf` after building.

## 7. Known missing pieces

The original script that fit NN-I/V6’s initial 65 parameters is not included; its final weights are. Original dependency/toolchain versions and an immutable Wokwi project snapshot URL are unconfirmed. The final NN-V simulator code exists, but independent final Wokwi evaluation with a definitively associated binary is not established by these source files. See `docs/AUDIT_REPORT.md`.
