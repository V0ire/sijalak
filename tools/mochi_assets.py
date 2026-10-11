#!/usr/bin/env python3
"""Convert Watcher Mochi GIFs -> 1-bit OLED frames, and the pop sound -> PCM.

LOCAL DEMO TOOL. The Dasai GIFs and the "Cute Character Sound" source are
third-party assets licensed for personal use with the Watcher Mochi project;
they are NOT covered by that project's Apache-2.0 source license. This script
therefore writes ONLY into a local, git-ignored output directory and never
copies assets into the repository. Do not commit or redistribute its output.

Inputs (from the local reference clone, path passed on the CLI):
  <mochi>/sd_content/*.gif        63 animations
  <mochi>/main/pop_sound.h        13,600 samples, 16 kHz mono, int16

Outputs (default under firmware/preview/assets/, git-ignored):
  frames/<name>.bin               packed 1-bit page data (128x64 => 1024 bytes)
  frames/index.json               per-name frame count + delay, for the loader
  pop.pcm                         raw little-endian int16 PCM for I2S

Packing matches jalak::Framebuffer::to_page_bytes(): 8 vertical pixels per byte,
LSB = topmost pixel of the page, pages top-to-bottom.

Usage:
  python3 tools/mochi_assets.py --mochi ~/Projects/watcher-mochi-ref \
      --out firmware/preview/assets
  python3 tools/mochi_assets.py --mochi ... --only happy,wink,sleepy,love
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

try:
    from PIL import Image, ImageSequence
except ImportError:  # pragma: no cover
    print("Pillow is required (pip install pillow). Not installing automatically.")
    sys.exit(2)

WIDTH = 128
HEIGHT = 64
PAGES = HEIGHT // 8  # 8
MAX_FRAMES = 24      # cap memory/CPU; GIFs longer than this are truncated
THRESHOLD = 128      # luminance cut for 1-bit; fonts/eyes read well near mid


def pack_frame(img: Image.Image) -> bytes:
    """RGBA/L RGB image -> 128x64 1-bit pages, centered, aspect preserved."""
    gray = img.convert("L")
    # Fit inside the panel preserving aspect (never upscale past 1:1 here).
    scale = min(WIDTH / gray.width, HEIGHT / gray.height, 1.0)
    if scale != 1.0:
        gray = gray.resize(
            (max(1, int(gray.width * scale)), max(1, int(gray.height * scale))),
            Image.LANCZOS,
        )
    canvas = Image.new("L", (WIDTH, HEIGHT), 0)  # black background
    canvas.paste(gray, ((WIDTH - gray.width) // 2, (HEIGHT - gray.height) // 2))

    px = canvas.load()
    out = bytearray(WIDTH * PAGES)
    for y in range(HEIGHT):
        page = y // 8
        bit = 1 << (y % 8)
        row = page * WIDTH
        for x in range(WIDTH):
            if px[x, y] >= THRESHOLD:
                out[row + x] |= bit
    return bytes(out)


def convert_gif(path: Path) -> tuple[bytes, int]:
    """Return (concatenated packed frames, frame_count) for one GIF."""
    frames: list[bytes] = []
    delays: list[int] = []
    with Image.open(path) as im:
        for frame in ImageSequence.Iterator(im):
            if len(frames) >= MAX_FRAMES:
                break
            frames.append(pack_frame(frame.convert("RGBA")))
            delays.append(frame.info.get("duration", 100))
    if not frames:
        raise ValueError(f"no frames in {path}")
    return b"".join(frames), len(frames)


def parse_pop_sound(header: Path) -> bytes:
    """Parse the int16 array out of pop_sound.h -> little-endian PCM bytes."""
    text = header.read_text()
    m = re.search(r"pop_sound_data\s*\[[^\]]*\]\s*=\s*\{(.*?)\};", text, re.S)
    if not m:
        raise ValueError(f"could not find pop_sound_data in {header}")
    nums = [int(n) for n in re.findall(r"-?\d+", m.group(1))]
    return b"".join(int(n).to_bytes(2, "little", signed=True) for n in nums)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--mochi", required=True, type=Path, help="watcher-mochi-ref clone")
    ap.add_argument("--out", required=True, type=Path, help="output dir (git-ignored)")
    ap.add_argument("--only", default="", help="comma-separated GIF basenames")
    args = ap.parse_args(argv)

    sd = args.mochi / "sd_content"
    if not sd.is_dir():
        print(f"ERROR: {sd} not found (pass --mochi pointing at the clone)")
        return 1

    names = [n.strip() for n in args.only.split(",") if n.strip()]
    gifs = sorted(sd.glob("*.gif"))
    if names:
        gifs = [g for g in gifs if g.stem in names]
    if not gifs:
        print("ERROR: no GIFs selected")
        return 1

    frames_dir = args.out / "frames"
    frames_dir.mkdir(parents=True, exist_ok=True)
    index: dict[str, dict] = {}

    for gif in gifs:
        try:
            blob, count = convert_gif(gif)
        except Exception as e:  # keep going; report at the end
            print(f"  SKIP {gif.name}: {e}")
            continue
        (frames_dir / f"{gif.stem}.bin").write_bytes(blob)
        index[gif.stem] = {"frames": count, "bytes_per_frame": WIDTH * PAGES}
        print(f"  {gif.stem:20s} {count:2d} frames -> {len(blob):6d} bytes")

    # Pop sound (optional; skip cleanly if the header is absent).
    pop = args.mochi / "main" / "pop_sound.h"
    if pop.is_file():
        pcm = parse_pop_sound(pop)
        (args.out / "pop.pcm").write_bytes(pcm)
        index["_pop"] = {"samples": len(pcm) // 2, "rate": 16000, "bits": 16}
        print(f"  pop.pcm              {len(pcm)//2} samples @16kHz mono int16")
    else:
        print(f"  (no {pop}; skipping pop.pcm)")

    (frames_dir / "index.json").write_text(json.dumps(index, indent=2))
    print(f"\nWrote {len([k for k in index if not k.startswith('_')])} animations to {frames_dir}")
    print("REMINDER: these are third-party assets. Do not commit or redistribute.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
