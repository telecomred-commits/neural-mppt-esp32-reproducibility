# Execution order and traceability

The workflow below reconstructs **what the supplied files support**, not physical conversion or a full independently certified experiment. Read [REPRODUCTION.md](REPRODUCTION.md) for commands and [REPRODUCIBILITY_STATUS.md](REPRODUCIBILITY_STATUS.md) for unresolved deviations. **Work in a disposable copy** because research scripts can overwrite generated outputs.

| Stage | Inputs | Script or source | Expected evidence |
|---|---|---|---|
| 0. Verify snapshots | The repository as distributed | `python tools/preflight.py` | SHA-256 checks, 350 + 880 + 60 host logs, 29 Wokwi records, 7491 complete rows and primary metric check |
| 1. Rebuild initial NN-I fit | Four calibrated I–V sweeps, 1012 observations | `python tools/reproduce_nn_i_training.py` | 2–16–1 model, 65 initial parameters; `mppt_weights.h` and report byte-identical in the tested environment |
| 2. Reconstruct NN-I integration | V6 photovoltaic source and ACK-controlled firmware | `historical_development/closed_loop_integration_v6/tools/verify.py` | Host integration check, not a physical or Wokwi executable test |
| 3. Re-run original comparison | NN-I/V6 and P&O/INC/adaptive P&O | `supplementary/initial_host_campaign/tools/run_host.py 10` from its campaign directory | 350 host simulations, profiles S0–S4 and R1/R2, seeds 1–10 |
| 4. Constrained recalibration | Four initial sweeps + five development frozen Wokwi records | `supplementary/calibration_NN_C/tools/train_v8.py` | NN-C/V8 calibration; development data are not blind validation |
| 5. Matched comparisons | Archived model, controllers and 11 scheduled profiles | `benchmark_classical.py`, `benchmark_affine.py`, `neural_bench/tools/bench_v9.py` under `supplementary/matched_extension` | 330 classical, 110 affine and 440 neural simulations (880 total), seeds 11–20 |
| 6. Passive-only ablation | NN-V with probing disabled | `neural_bench/tools/bench_v9.py` using passive variants | 60 runs; this is distinct from matched 880 |
| 7. Recalculate published metrics | Frozen experimental outputs | `supplementary/matched_extension/prepare_article_data.py` | Tables, contrasts, figures and 29-file Wokwi inventory |
| 8. Fixed-input replay | Eight recorded Wokwi trajectories | `supplementary/fixed_input_diagnostic/tools/investigate.py` | 32 diagnostic replays; **not** independent closed-loop MPPT energy experiments |
| 9. Explore interactive simulation | `wokwi_simulation/` source files | Public project [Wokwi](https://wokwi.com/projects/479082581022049281) | Interactive ESP32/digital-source simulation, not physical converter validation |

The host campaigns **do not** create a hardware firmware image as their output. ESP32 firmware is compiled separately from selected fixed controller sources, weights and compatible build tools. A frozen historical ESP32 binary/custom-chip WASM and its exact build toolchain were not supplied. Do not state that they were rebuilt identically.

## Journal manuscript result attribution

- Tracking efficiency: ratio of extracted energy to the independently derived source-model maximum-energy reference, not to the neural voltage reference.
- Current-prediction error: measured minus pre-update neural current prediction (A).
- Reference bias: signed mean of `V_ref - V_mp*` (V).
- Regulation offset: signed mean of `V_meas - V_ref` (V).
- These endpoints cannot be interchanged. A current predictor can improve without increasing energy or reducing reference error.

## When preparing a paper-linked DOI

Deposit an immutable GitHub release and archive *that particular release* in Zenodo. Enter the assigned, version-specific DOI into the journal's data availability statement and this repository. Never invent a DOI or assert that the live Wokwi project is version-locked.
