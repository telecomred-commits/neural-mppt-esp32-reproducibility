# Independent reproducibility audit — 28 October 2026

This audit used both supplied ZIPs and compared them with the 34-page scientific manuscript's reported experiments. **This is a verification of available artifacts, not evidence of physical device performance or independent laboratory validation.**

## Present and successfully executed

- 1,666 extracted files in `Datos_MPPT_v8(1).zip` and seven Wokwi project files. Primary raw data, host sources, tests and calibration scripts are present. Duplicate historical Wokwi collections are preserved as duplicates, not counted again.
- The 350-run initial V6 campaign compiled and executed to completion with `python tools/run_host.py 10`.
- The 330 classical, 110 affine and 440 NN-C/NN-V host cases, plus the 60 passive cases, compiled and executed in an independent workspace. The 60 passive metrics and 330/110 classical/affine metrics were exactly equal to their stored JSON versions under the audit toolchain.
- `matched_extension/prepare_article_data.py` successfully recalculated the matched campaign tables and the Wokwi inventory (29 files, 7491 complete observations) from archived logs.
- `calibration_NN_C/tools/train_v8.py` successfully executed with the four curves and five development datasets.
- `fixed_input_diagnostic/tools/investigate.py` successfully executed and produced its fixed-input diagnostic outputs.

## Nuance: numerical identity not achieved for all re-runs

- Initial host campaign: **31 of 350 raw host logs** were not byte-identical to archived logs. The primary averaged efficiency for V6 online was about **99.42736847% archived vs 99.42737843%** reexecuted; NN-I/V6 frozen and classical primary averages matched to reported precision.
- Matched neural runs: **82 of 440 JSON metric records** differed in at least one field. In **three** records the per-run tracking efficiency differed slightly. NN-V online update/rejection counters sometimes differed by several accepted steps, even with the same logged firmware/chip SHA-256 and identical seed/profile selections. Do not describe those counters as bitwise deterministic from this reproduction.
- The 330 classical, 110 affine and 60 passive metrics were identical to archived JSON. Aggregate manuscript interpretations were unchanged, but exact identical logs/learned trajectories across environments are not yet demonstrated for all NN runs.
- The cause of the differences is **not identified**. Potential toolchain/floating-point or numerical-threshold sensitivity requires targeted investigation; these are hypotheses, not verified explanations.

## Reproduced primary matched results from archived logs

| Variant | Model-referenced efficiency on primary S0–S4 (%) |
|---|---:|
| P&O | 98.125221 |
| INC | 98.384267 |
| Adaptive P&O | 97.142303 |
| Calibrated affine reference | 99.429472 |
| NN-C frozen | 99.430243 |
| NN-C online | 99.430243 |
| NN-V frozen | 99.415929 |
| NN-V online | 99.415937 |

These outputs do **not** show an energetic benefit of online identification in the tested simulated plant. The almost identical affine and NN-C efficiency is relevant to scientific attribution.

## Pending for full reproduction

1. Original complete NN-I/V6 initial model-training code and dependency lockfile (final frozen weights exist).
2. Verified, version-frozen Wokwi project accessibility. A public URL is supplied, but it was not independently opened in this audit environment. No `.wasm` custom-chip build artifact or ESP32 `.bin`/`.elf` was delivered.
3. Definitive mapping of all historical Wokwi logs to an archived, compiled firmware binary; the existing logs have limitations stated in the manuscript.
4. Origin/cause of the reexecution differences noted above; define exactness or tolerance thresholds for any future automated CI.
5. Permission/licensing, final DOI and author approval before public deposit.

The original material is preserved byte-for-byte and indexed with SHA-256 in the packaged repository. Scripts in `supplementary/` may rewrite generated outputs. Reproduce in a separate working clone.


## Addendum: recovered historical V5 training (2026-10-28)

The prior finding that original NN-I/V6 training source was absent is **superseded**: `historical_development/NN_I_original_training_v5/tools/train.py` was supplied later. An independent re-run using the four calibrated sweeps generated `mppt_weights.h` and `entrenamiento.json` byte-identically. This does not resolve the NN-V host counter differences, old toolchain-version uncertainty or Wokwi binary provenance.
