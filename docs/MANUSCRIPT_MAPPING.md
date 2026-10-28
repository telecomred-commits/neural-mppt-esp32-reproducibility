# Manuscript-to-evidence correspondence (34-page scientific version)

| Manuscript claim / experimental section | Source and reproduction path | Caution |
|---|---|---|
| Digital PV source Eqs. 1–5, virtual load Eqs. 4 and 6 | `supplementary/initial_host_campaign/referencia/solar.chip.c`; `wokwi_simulation/solar.chip.c`, `solar.chip.json` | Simulated source, no switching converter |
| ADC signal mapping, ACK and load control | `wokwi_simulation/sketch.ino`, `diagram.json`; historical V6 and frozen V9 sketches in `supplementary/` | Wokwi project and host are separate simulation routes |
| NN-I V6 current prediction and bounded output-layer update | `supplementary/initial_host_campaign/referencia/{mppt_neural.h,mppt_weights.h,sketch.ino}` | Original training code recovered in `historical_development/NN_I_original_training_v5/tools/train.py`; byte-identical weights reproduced |
| Four training sweeps, 1012 samples and constrained NN-C | `supplementary/calibration_NN_C/datos/curvas/`, `tools/train_v8.py`, firmware weights and reports | Some development traces used in fitting, not external blind test |
| NN-V V9 power-reference verification and guarded updates | `supplementary/matched_extension/neural_bench/firmware/`, `tests/`, `tools/bench_v9.py` | Code tested in host simulations, not validated in physical converter |
| Initial 350-run campaign | `supplementary/initial_host_campaign/resultados_host/`; `tools/run_host.py` | Seeds 1–10, seven scenarios, five variants |
| Matched 880-run campaign and affine comparator | `supplementary/matched_extension/{classical_matched,affine_matched,neural_bench/resultados/comparacion_congelada}` and `prepare_article_data.py` | Seeds 11–20, 11 scenarios, 8 variants |
| Passive-only 60-run ablation | `supplementary/matched_extension/neural_bench/resultados/ablacion_pasiva/` | Passive and active verification distinguished |
| 29 Wokwi logs / 7491 complete rows | `supplementary/matched_extension/wokwi_initial/` and `wokwi_additional/` | Excluded incomplete fragments remain in raw files; unmatched manual histories |
| Equal-input replay (32 variants) | `supplementary/fixed_input_diagnostic/` and `tools/investigate.py` | Not an independent closed-loop energy comparison |
| Literature/descriptive commercial reference | `revision/` and `supplementary/literature/` | No physical testing on the ten commercial modules |

The tracking efficiency is calculated against the **analytical source MPP**, not the neural-reference voltage. Current-prediction error, reference bias and regulation offset are different quantities. A low prediction error does not imply increased extracted energy.
