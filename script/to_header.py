# SynthCi_SDL2 - Fivewayses (TM)
#
# ===============================
#
# MIT License
# 
# Copyright (C) Fivewayses (2026).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
# (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# 
#  [*] The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# 
#  [*] THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
#      FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#      CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import subprocess
import os

root = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(root, "../example")

for item in os.listdir(path):
    abs_item = os.path.join(path, item)

    if os.path.isdir(abs_item): continue
    if item.endswith(".h"): continue

    item_no_ext = os.path.splitext(item)[0]
    abs_item_no_ext = os.path.join(path, item_no_ext)

    with open(abs_item_no_ext + ".h", 'w') as f:
        subprocess.run(
            ["xxd", "-i", "-n", item_no_ext, abs_item],
            stdout=f,
            check=True
        )

