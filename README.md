# Neural MPPT: online current identification and power-reference verification

**Status: repository candidate, undergoing reproducibility audit; not yet a certified or independently replicated release.**

This repository accompanies the research manuscript *Online Identification and Power-Reference Verification for Neural MPPT in an ESP32-Oriented Photovoltaic Simulation* (Suárez Suárez and Penagos Espinel). The experiments are **simulations**. No physical ESP32 switching-converter validation is claimed.

## Interactive Wokwi simulation

**[Open the Wokwi photovoltaic simulation](https://wokwi.com/projects/479082581022049281)**. A downloaded snapshot is preserved under [`wokwi_simulation/`](wokwi_simulation/). The project contains `sketch.ino`, neural model and weight headers, `diagram.json`, and the custom digital PV source (`solar.chip.c`, `solar.chip.json`). The access and exact live version of the external URL should be checked before journal submission.

The downloaded Wokwi `sketch.ino` differs from the host-campaign sketch in its **experiment mode selection and telemetry header**; the normalized neural model, stored weights, and PV source match their corresponding frozen host sources. Do not substitute one sketch for another when reproducing historical results. See [provenance](docs/PROVENANCE.md).

## What is available

- `supplementary/initial_host_campaign/`: five modes × seven profiles × ten paired seeds = **350 host simulations** (NN-I / V6 study and classical baselines).
- `supplementary/calibration_NN_C/`: four IV sweeps (**1012 observations**), five development traces, constrained NN-C calibration script and measured-data anchors.
- `supplementary/matched_extension/`: **880 matched host simulations** (330 classical + 110 affine + 440 neural NN-C/NN-V) and **60 passive-ablation simulations**, with source code, profiles and outputs.
- `supplementary/fixed_input_diagnostic/`: **32 equal-input replay combinations** derived from eight observed traces, separately from closed-loop energy experiments.
- `supplementary/matched_extension/wokwi_initial/` and `wokwi_additional/`: **29 Wokwi-simulation records (7491 complete observations)**. Some are not matched trajectory pairs, and not all are attributable to a known compiled firmware binary.
- `supplementary/wokwi_20261027/`: an earlier audit of the initial 18 Wokwi files; these are not extra independent observations.
- `revision/`: internal audit artifacts and literature records from the supplied archive, retained for traceability rather than merged with primary measurements.

## Historical development and recovered training

The original **V5 current-model training script** has now been recovered and reproduced. Its 65 trainable parameters and report reproduce **byte-for-byte** from the four calibration sweeps (1012 observations) in the tested environment. The resulting `mppt_weights.h` also matches the frozen NN-I/V6 weights used in the host campaign. Execute `python tools/reproduce_nn_i_training.py` for an isolated check (requires NumPy and scikit-learn). See [historical provenance](historical_development/HISTORY.md) for V1–V6 and original supplied archives. Earlier policy-imitation prototypes must not be conflated with final current-identification models.

**Outstanding:** NN-V online update-counter differences on host re-execution, missing identified historic Wokwi ESP32 binary/WASM, original toolchain lockfile and attribution of some Wokwi records. No new physical validation is implied.

## Quick integrity check

Run `python tools/preflight.py` from the repository root for a read-only audit of archived files and efficiency figures. Then follow [Execution order](docs/EXECUTION_ORDER.md) and [Wokwi simulation guide](docs/WOKWI_GUIDE.md). The precise limits of successful reproduction are recorded in [Reproducibility status](docs/REPRODUCIBILITY_STATUS.md).

## Reproduction pathway

1. Reproduce NN-I original neural fitting: `python tools/reproduce_nn_i_training.py` (runs in a temporary directory).
2. Read [Reproduction protocol](docs/REPRODUCTION.md).
3. Verify the inventory: `python tools/verify_archive.py` (does not change source data).
4. Recalculate the figures and aggregate results: `cd supplementary/matched_extension && python prepare_article_data.py` (writes generated results in the working copy).
5. Re-run the host simulations with the exact source and profile definitions. **Use a disposable copy** of the repository to avoid replacing archived results.
6. Review [audit limitations](docs/AUDIT_REPORT.md) before claiming numerical identity across platforms.
7. Explore the Wokwi link and compare its configuration with the fixed host firmware; Wokwi results are simulated, not physical measurements.

[Readme en español](README_ES.md) · [Manuscript-to-file mapping](docs/MANUSCRIPT_MAPPING.md) · [Publication checklist](docs/RELEASE_CHECKLIST.md)

**DOI:** Not yet assigned. **License:** Authors must confirm rights and choose licenses for code and data before public release. The presence of this ZIP does not imply redistribution rights for third-party research PDFs, datasheets, fonts or external sources.

For the authors: [public-release instructions in Spanish](docs/PUBLISHING_ES.md). `CITATION.cff` identifies the contributors but does not assert a DOI or release publication.
