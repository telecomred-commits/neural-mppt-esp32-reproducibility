# Provenance and distinction between experimental sources

**Sources received for this repository candidate**

1. `Datos_MPPT_v8(1).zip` → `supplementary/` and `revision/`, preserved with original directory structure and file bytes.
2. `panelsolarMPPT Intelligent Copy (2).zip` → `wokwi_simulation/`, preserved with original directory structure and file bytes. The internal `wokwi-project.txt` names https://wokwi.com/projects/479082581022049281.
3. Scientific reference: the earlier 34-page manuscript version; its results, not a longer reconstructed draft, define the comparison targets in this audit. The manuscript PDF is **not included** here to avoid implying journal redistribution rights.

Every supplied material file is recorded in `checksums/SHA256SUMS.txt`. Do not change original measurements. Generated files go into a separate working copy and must not overwrite the primary archived evidence for deposition.

## Correspondence of firmware

The Wokwi snapshot's `mppt_neural.h`, `mppt_weights.h` and `solar.chip.c` are text-equivalent to their frozen NN-V host/plant counterparts after normalizing CRLF line endings. **Their raw SHA-256 hashes are not all identical** because of newline normalization. The Wokwi `sketch.ino` differs from the host NN-V sketch in compile-time selector configuration and console metadata, including default frozen mode and a configuration revision marker. Thus, the online Wokwi sketch must **not** be substituted for the fixed host-campaign sketch when reproducing published results.

The initial campaign NN-I/V6 uses a different controller, weight header and smaller original design. NN-C/V8 calibration and NN-V/V9 development are independent named versions. The original Wokwi logs contain incomplete firmware provenance for some traces; as stated in the manuscript, these logs alone do not establish a blind or independent final NN-V evaluation.

## Integrity and rights

The integrity manifest validates copies of each source file; a SHA-256 match does not prove originality, suitability for physical devices, or that a recorded log came from a particular unsupplied binary. Before publishing, authors should verify ownership/licensing for any externally sourced files or bibliographic materials. No code or dataset license has been selected in this draft.
