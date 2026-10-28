#!/usr/bin/env python3
"""Read-only integrity and file-count audit for the MPPT research package."""
from pathlib import Path
import hashlib
import json
import re
import sys

ROOT = Path(__file__).resolve().parents[1]

def main():
    altered = []
    lines = (ROOT / 'checksums/SHA256SUMS.txt').read_text().splitlines()
    for line in lines:
        digest, rel = line.split('  ', 1)
        path = ROOT / rel
        if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != digest:
            altered.append(rel)
    root = ROOT / 'supplementary'
    m = root / 'matched_extension'
    cnt = lambda path: len(list(path.glob('*.txt')))
    counts = {
        'initial_host_logs': len(list((root/'initial_host_campaign/resultados_host').glob('*_seed??.txt'))),
        'matched_classical_logs': cnt(m/'classical_matched'),
        'matched_affine_logs': cnt(m/'affine_matched'),
        'matched_neural_logs': cnt(m/'neural_bench/resultados/comparacion_congelada'),
        'passive_neural_logs': cnt(m/'neural_bench/resultados/ablacion_pasiva'),
        'wokwi_original_files': sum(1 for d in (m/'wokwi_initial',m/'wokwi_additional') for x in d.iterdir() if x.is_file() and x.suffix in ('.txt','.md'))
    }
    rows = 0
    incomplete = 0
    files = [x for d in (m/'wokwi_initial',m/'wokwi_additional') for x in d.iterdir() if x.is_file() and x.suffix in ('.txt','.md')]
    for file in files:
        s = file.read_text(encoding='utf-8-sig')
        s = s.replace('\\r\\n','\n').replace('\\n','\n').replace('\\t','\t').replace('\\r','\r')
        for line in s.splitlines():
            fields = line.split()
            if fields and re.fullmatch(r'\d+',fields[0]):
                if len(fields)==23:
                    rows+=1
                else:
                    incomplete+=1
    counts['wokwi_complete_rows']=rows
    counts['wokwi_incomplete_fragments']=incomplete
    expected = {'initial_host_logs':350,'matched_classical_logs':330,'matched_affine_logs':110,
        'matched_neural_logs':440,'passive_neural_logs':60,'wokwi_original_files':29,
        'wokwi_complete_rows':7491}
    good = not altered and all(counts[k]==v for k,v in expected.items())
    result={'status':'PASS' if good else 'CHECK','source_files_checked':len(lines),'integrity_mismatches':altered[:15],
            'counts':counts,'expected':expected}
    print(json.dumps(result,ensure_ascii=False,indent=2))
    return 0 if good else 1
if __name__=='__main__':sys.exit(main())
