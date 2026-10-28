# Checklist before public GitHub/Zenodo publication

- [ ] Confirm the Wokwi link opens publicly and freeze an identifiable export of the exact project used for publication.
- [x] Recover and reproduce the original NN-I/V6 65-parameter training script (byte-identical weights).
- [ ] Document a historical training dependency lockfile; the current tested environment is not the proven original toolchain.
- [ ] Resolve the small numerical nonidentity in some host-reexecuted traces, especially NN-V online acceptance/update counters, or set scientifically justified tolerances and state them in the repository.
- [ ] Produce and verify build instructions for custom-chip WASM and ESP32 `.bin`/`.elf`, or explicitly scope reproducibility to host simulation and Wokwi browser execution.
- [ ] If claiming final NN-V Wokwi validation, perform and archive matched runs with a positively identified firmware configuration and full logs. No physical-converter validation is implied.
- [ ] Inspect third-party and internal records before public redistribution. Choose appropriate software/data licenses and confirm all authors consent to release.
- [ ] Verify all figures/tables against the 34-page manuscript and the final accepted revision, retaining original source data.
- [ ] Add authors' approved repository citation and institutional affiliations; verify preferred spelling and ORCID identifiers if used.
- [ ] Create a GitHub release, deposit the exact version in Zenodo and record its version-specific DOI. Do not cite a DOI that has not yet been assigned.
- [ ] Update the manuscript's *Data availability statement* with the repository URL, Wokwi link and archived DOI.
- [ ] If journal requests, include a concise description of AI assistance according to its editorial policy.
