#!/usr/bin/env python3
"""Verify complete release manifest; excludes the manifest itself."""
from pathlib import Path
import hashlib,sys
root=Path(__file__).resolve().parents[1]
manifest=root/'checksums/RELEASE_SHA256SUMS.txt'
errors=[]
for line in manifest.read_text(encoding='utf-8').splitlines():
  digest,rel=line.split('  ',1)
  p=root/rel
  if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=digest: errors.append(rel)
print(f'Checked {len(manifest.read_text().splitlines())} release files; mismatches: {len(errors)}')
for f in errors[:10]:print(' -',f)
returncode = 0 if not errors else 1
sys.exit(returncode)
