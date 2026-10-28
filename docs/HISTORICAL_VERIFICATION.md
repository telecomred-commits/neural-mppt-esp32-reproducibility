# Historical verification addendum

Run on 2026-10-28 in the preparation environment, without running Wokwi or physical hardware.

| Check | Outcome | Scope |
|---|---|---|
| V5 `tools/train.py` on 1012 rows | Passed | `mppt_weights.h` and `entrenamiento.json` byte-identical to supplied V5 ZIP (SHA-256 e35c8a122b08c27ff22b487cd622522be75045868d473f9d1738982b40458428 for weights) |
| V5 weights vs archived NN-I/V6 | Passed | SHA-256 identical to `supplementary/initial_host_campaign/referencia/mppt_weights.h` |
| V6 `tools/verify.py` host compilation + model bridge | Passed | g++/gcc with Arduino/Wokwi API mocks; **not** actual ESP32 or Wokwi-run evidence |
| Execution kit `tools/test_tools.py` | Passed | 11 host unit tests; not a Wokwi experiment |
| Two separately supplied `sketch(7)` files | Identical | SHA-256 `0d03c2a98461d42aad8a49af18dc63f14e9c1592a63f3c945f34ccf578439366`; provenance of published firmware not established |

Tested version set for training: Python 3.13.5; NumPy 2.3.5; scikit-learn 1.8.0; SciPy 1.17.0. Earlier training environment versions have not been independently established. These versions describe this successful re-run only.

**Unresolved:** The earlier audit found NN-V update-count deviations in some host re-executions. Distinct Wokwi simulation records do not supply verified versioned ESP32 firmware binaries. These do not become validated merely because V5 training now reproduces exactly. No physical-panel testing is claimed.
