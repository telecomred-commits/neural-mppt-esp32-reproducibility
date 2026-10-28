# Reproducibility and evidence status — 28 October 2026

## Directly checked

- Original source inventory: 1673 content files with the supplied SHA-256 manifest; no digest mismatch in this package.
- Initial host campaign: 350 logs (7 profiles × 5 variants × 10 seeds).
- Matched host comparison: 880 logs (330 classical, 110 affine, 440 neural).
- Passive-only ablation: 60 logs.
- Wokwi simulation records: 29 files with 7491 complete 23-field rows; two incomplete fragments retained and excluded.
- Training: the recovered V5 training code regenerates the initial NN-I/V6 `mppt_weights.h` and JSON training report byte-for-byte in the tested software environment.
- Primary S0–S4 model-referenced efficiency values in archived metrics match the manuscript rounding; `python tools/check_manuscript_metrics.py` verifies them.

## Re-run distinction

In prior audit executions, all campaigns completed. **31/350 initial host logs** were not byte-identical. **82/440 neural matched metrics** had at least one differing field; several NN-V update/rejection counts were different although aggregate tracking efficiencies remained close. The 330 classical, 110 affine and 60 passive metric records were identical to their archived JSON under that audit environment. These differences remain unresolved and should not be concealed by loosening the claimed meaning of 'identical'.

The original calibration and Wokwi input histories include development conditions, not external blind source-module validation. The host simulation uses mocks and digital model equations, not processor-accurate ESP32 timing or a physical power converter. Some recorded Wokwi simulation trajectories are not matched treatment pairs.

## Absent from supplied files

- Exact original compiler/Arduino/ESP32 and Wokwi runtime lockfile.
- Positively identified historical ESP32 firmware `.bin`/`.elf` and custom-chip `.wasm` compiled binaries.
- Verified one-to-one mapping of all observed historic Wokwi logs to an immutable binary.
- Independent physical-converter/field validation.

## Publication gates

Before a public GitHub release/Zenodo deposit, the authors must review redistribution rights, approve a code/data license, verify the live Wokwi link and attest to the archived revision. A GitHub upload alone does not generate a DOI; the DOI is assigned after archival by the relevant repository service.

See `docs/AUDIT_REPORT.md` for technical details and `docs/RELEASE_CHECKLIST.md` for outstanding administrative work. The older note about missing NN-I training in that audit is superseded by the recovered V5 training recorded here.
