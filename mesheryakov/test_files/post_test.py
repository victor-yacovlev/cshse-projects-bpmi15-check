#!/usr/bin/env python3
import os, sys, json
data = sys.stdin.read(int(os.environ.get('HTTP_CONTENT_LENGTH', 0)))
if data:
    print(list(json.loads(data).keys()])
