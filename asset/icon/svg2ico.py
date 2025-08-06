"""
Python Environment Configuration
1. Download and install GTK from
https://github.com/tschoonj/GTK-for-Windows-Runtime-Environment-Installer
2. Add "<GTK_path>/bin" to system path (this should've been done automatically)
3. run pip install pillow cairosvg
"""

import io

from PIL import Image
from cairosvg import svg2png

sizes = [16, 32, 48, 64, 128, 256]
svg_file = "EyeOfLewiki.svg"
png_bytes = svg2png(url=svg_file, output_width=256, output_height=256)
image = Image.open(io.BytesIO(png_bytes))
image.save("EyeOfLewiki.ico", format="ICO", sizes=[(s, s) for s in sizes])
