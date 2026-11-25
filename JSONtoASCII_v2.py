#!/usr/bin/env python3

import sys
import json

with open(sys.argv[1]) as f:
    run_lumi_map = json.load(f)

for run, lumis in run_lumi_map.items():
    for lumi in lumis:
        print(run, " ".join(str(num) for num in lumi))
