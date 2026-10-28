#!/usr/bin/env python3
"""Read-only audit. Does not invoke scripts that overwrite archived results."""
import subprocess,sys,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
commands=[('Source integrity and run counts',['tools/verify_archive.py']),('Manuscript metrics',['tools/check_manuscript_metrics.py'])]
statuses={}
for label,parts in commands:
    run=subprocess.run([sys.executable,*parts],cwd=ROOT,capture_output=True,text=True)
    statuses[label]='PASS' if run.returncode==0 else 'FAIL'
    print(f'\n=== {label}: {statuses[label]} ===')
    if run.stdout:print(run.stdout)
    if run.stderr:print(run.stderr,file=sys.stderr)
print('\nPREFLIGHT:',json.dumps(statuses))
sys.exit(0 if all(status=='PASS' for status in statuses.values()) else 1)
