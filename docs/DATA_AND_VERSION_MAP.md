# Data and version map

| Scientific item | Data/code location | Interpretation |
|---|---|---|
| Digital PV source | `supplementary/initial_host_campaign/referencia/solar.chip.c`; snapshot `wokwi_simulation/solar.chip.c` | Simulated PV characteristic and virtual load. |
| Original NN-I (V6) weights | `historical_development/NN_I_original_training_v5/entrega/mppt_weights.h` | 65-parameter offline fit; later online updating is limited to output layer. |
| Original training | `historical_development/NN_I_original_training_v5/tools/train.py` | Four sweeps, 1012 samples; reproducible by root training checker. |
| NN-C (V8) | `supplementary/calibration_NN_C/` | Constrained recalibration and projected online updates. |
| NN-V (V9) | `supplementary/matched_extension/neural_bench/` | Guarded current/slope updates plus measurement-assisted verification. |
| Classical and affine comparisons | `supplementary/matched_extension/classical_matched`, `affine_matched` | Same scheduled host scenario/seed conditions; affine is source-calibrated. |
| Initial experiment | `supplementary/initial_host_campaign/resultados_host/` | 350 host simulations, not Wokwi measurements. |
| Matched experiment | `supplementary/matched_extension/{classical_matched,affine_matched,neural_bench/resultados/comparacion_congelada}` | 880 host simulations. |
| Passive verification | `supplementary/matched_extension/neural_bench/resultados/ablacion_pasiva/` | 60 host simulations. |
| Observational Wokwi logs | `supplementary/matched_extension/wokwi_initial/`, `wokwi_additional/` | 29 archives, 7491 complete rows; not all paired/firmware-identified. |
| Equal-input replay | `supplementary/fixed_input_diagnostic/` | 32 diagnostic executions on fixed observations, not closed-loop energy results. |
| Historical V1–V6 | `historical_development/` | Prior alternatives and development record; do not pool as final NN-V trials. |

Provenance and file-level digests: `docs/PROVENANCE.md`, `historical_development/HISTORY.md`, and `checksums/SHA256SUMS.txt`.
