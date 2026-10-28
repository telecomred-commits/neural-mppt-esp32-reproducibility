#!/usr/bin/env python3
"""Reproduce original V5 NN-I neural fitting in a disposable copy and compare bytes.
Requires Python, numpy, scikit-learn and scipy. Tested on Python 3.13.5, NumPy 2.3.5, scikit-learn 1.8.0, SciPy 1.17.0. This is a tested runtime, not a historically proven original lockfile.
"""
from pathlib import Path
import subprocess,tempfile,shutil,sys,hashlib,json
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'historical_development'/'NN_I_original_training_v5'
BASELINE=SOURCE/'entrega'/'mppt_weights.h'
REPORT=SOURCE/'validacion'/'entrenamiento.json'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
with tempfile.TemporaryDirectory() as td:
    p=Path(td)/'training'
    shutil.copytree(SOURCE,p)
    run=subprocess.run([sys.executable,str(p/'tools'/'train.py')],cwd=p,capture_output=True,text=True,timeout=120)
    if run.returncode:
        print(run.stdout,run.stderr,file=sys.stderr)
        raise SystemExit(run.returncode)
    checks={'weights_identical':(p/'entrega'/'mppt_weights.h').read_bytes()==BASELINE.read_bytes(),
            'training_report_identical':(p/'validacion'/'entrenamiento.json').read_bytes()==REPORT.read_bytes(),
            'weights_sha256':sha(p/'entrega'/'mppt_weights.h'),
            'training_report_sha256':sha(p/'validacion'/'entrenamiento.json')}
    print(json.dumps(checks,indent=2))
    if not checks['weights_identical'] or not checks['training_report_identical']:
        raise SystemExit('Mismatch: record software versions, compare numerical parameters and report discrepancy.')
