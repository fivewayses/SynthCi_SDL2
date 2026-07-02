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

