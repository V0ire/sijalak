#!/usr/bin/env python3
"""Decode actual C++ OLED pages. No character geometry is drawn in Python."""
import hashlib
import html
import json
import math
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "exports/oled"
SOURCE = ROOT / "workstreams/oled"


def decode(data):
    if len(data) != 1024:
        raise ValueError("frame must contain exactly eight 128-byte pages")
    return Image.frombytes("L", (128, 64), bytes(
        255 if data[(y // 8) * 128 + x] & (1 << (y % 8)) else 0
        for y in range(64) for x in range(128)
    ))


def main():
    manifest = json.loads((OUT / "frames.json").read_text())
    raw = (OUT / "frames.raw").read_bytes()
    if len(raw) != sum(c["count"] * 1024 for c in manifest["clips"]):
        raise ValueError("manifest/frame length mismatch")
    for name in ("stills", "animations", "atlases"):
        (OUT / name).mkdir(exist_ok=True)
    font = ImageFont.load_default(size=18)
    small = ImageFont.load_default(size=14)
    sheet_clips = manifest["clips"][:16]
    sheet = Image.new("RGB", (1600, 110 + math.ceil(len(sheet_clips) / 3) * 320), "white")
    draw = ImageDraw.Draw(sheet)
    draw.text((24, 18), "SI JALAK BEDAS / OLED CHARACTER", fill="black", font=font)
    draw.text((24, 50), "Actual 128 x 64 framebuffer, 4x nearest-neighbour. Host fixtures; hardware and 0.5 m readability unverified.",
              fill="black", font=small)
    hashes = {}
    for index, clip in enumerate(manifest["clips"]):
        frames = [decode(raw[clip["offset"] + i * 1024:clip["offset"] + (i + 1) * 1024])
                  for i in range(clip["count"])]
        native = frames[clip["still"]]
        native.save(OUT / "stills" / f"{clip['name']}.png")
        enlarged = [f.resize((512, 256), Image.Resampling.NEAREST) for f in frames]
        enlarged[0].save(OUT / "animations" / f"{clip['name']}.gif", save_all=True,
                         append_images=enlarged[1:], duration=manifest["duration_ms"],
                         loop=0, optimize=False, disposal=2)
        atlas = Image.new("L", (1280, math.ceil(len(frames) / 10) * 64))
        for i, frame in enumerate(frames):
            atlas.paste(frame, ((i % 10) * 128, (i // 10) * 64))
        atlas.save(OUT / "atlases" / f"{clip['name']}.png")
        hashes[clip["name"]] = [hashlib.sha256(f.tobytes()).hexdigest() for f in frames]
        if index < len(sheet_clips):
            x, y = 24 + (index % 3) * 526, 110 + (index // 3) * 320
            draw.text((x, y), clip["label"], fill="black", font=small)
            sheet.paste(enlarged[clip["still"]].convert("RGB"), (x, y + 25))
    sheet.save(OUT / "contact-sheet.png")
    (OUT / "pixel-hashes.json").write_text(json.dumps(hashes, indent=2) + "\n")
    template = (SOURCE / "viewer.html").read_text()
    options = "\n".join(f'<option value="{c["name"]}">{html.escape(c["label"])}</option>'
                        for c in manifest["clips"])
    template = template.replace("__OPTIONS__", options).replace("__MANIFEST__", json.dumps(manifest))
    (OUT / "index.html").write_text(template)
    (OUT / "viewer.js").write_text((SOURCE / "viewer.js").read_text())
    source_paths = [*sorted((ROOT / "firmware/components/jalak_oled").rglob("*.cpp")),
                    *sorted((ROOT / "firmware/components/jalak_oled").rglob("*.h")),
                    SOURCE / "render_preview.cpp", SOURCE / "generate_previews.py",
                    SOURCE / "viewer.html", SOURCE / "viewer.js"]
    for name in ("expression", "framebuffer", "state_machine"):
        source_paths.extend([ROOT / f"firmware/components/jalak_logic/src/{name}.cpp",
                             ROOT / f"firmware/components/jalak_logic/include/jalak/{name}.h"])
    provenance = {
        "pipeline": "C++ Adapter/render -> 1024 page bytes/frame -> PIL decode -> PNG/GIF/atlas -> HTML canvas",
        "audio": "Synthetic envelope fixtures only. No actual audio or measured lip synchronization.",
        "raw_sha256": hashlib.sha256(raw).hexdigest(),
        "renderer_binary_sha256": hashlib.sha256((SOURCE / "build/oled_render").read_bytes()).hexdigest(),
        "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths},
        "clips": len(manifest["clips"]), "frames": len(raw) // 1024,
    }
    (OUT / "provenance.json").write_text(json.dumps(provenance, indent=2) + "\n")
    print(f"Generated {provenance['clips']} GIFs, PNG stills/atlases, contact sheet and offline viewer from {provenance['frames']} C++ frames.")


if __name__ == "__main__":
    main()
