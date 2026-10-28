#!/usr/bin/env python3
"""Read-only checks of key manuscript results against stored recalculated values.
Does not run simulations or infer physical validation.
"""
from pathlib import Path
import json, math, sys
ROOT=Path(__file__).resolve().parents[1]
M=ROOT/'supplementary/matched_extension'
comp=json.loads((M/'reanalysis_stdout.json').read_text())
actual={x['variant']:x['efficiency_mean_pct'] for x in comp['comparison']}
expected={
  'PO':98.125221,'INC':98.384267,'ADAPT_PO':97.142303,
  'AFFINE':99.429472,'V8_FROZEN':99.430243,'V8_ONLINE':99.430243,
  'V9_FROZEN':99.415929,'V9_ONLINE':99.415937,
}
result={x:{'archived':actual.get(x),'manuscript_rounded':v,
           'within_0.000001_pctpoint':actual.get(x) is not None and abs(actual[x]-v)<=1e-6}
        for x,v in expected.items()}
check=all(v['within_0.000001_pctpoint'] for v in result.values())
print(json.dumps({'status':'PASS' if check else 'CHECK','metric':'model-referenced tracking efficiency, host S0-S4 (%)','variants':result},indent=2))
sys.exit(0 if check else 1)
