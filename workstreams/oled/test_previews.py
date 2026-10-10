#!/usr/bin/env python3
"""Verify delivered artifacts against the exact compiled C++ framebuffer stream."""
import hashlib
import json
from pathlib import Path
import unittest

from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "exports/oled"


class PreviewContract(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.manifest = json.loads((OUT / "frames.json").read_text())
        cls.raw = (OUT / "frames.raw").read_bytes()

    def pixels(self, clip, i):
        raw = self.raw[clip["offset"] + i * 1024:clip["offset"] + (i + 1) * 1024]
        # Independent pixel-order check, separate from generation's comprehension.
        result = bytearray(128 * 64)
        for page in range(8):
            for x in range(128):
                for bit in range(8):
                    result[(page * 8 + bit) * 128 + x] = 255 * ((raw[page * 128 + x] >> bit) & 1)
        return bytes(result)

    def test_manifest_contiguous_all_states(self):
        offset = 0
        for clip in self.manifest["clips"]:
            self.assertEqual(clip["offset"], offset)
            self.assertEqual(len(clip["visible_states"]), clip["count"])
            offset += clip["count"] * 1024
        self.assertEqual(offset, len(self.raw))
        self.assertEqual(len(self.manifest["clips"]), 19)
        self.assertEqual({c["visible_states"][0] for c in self.manifest["clips"][:9]},
                         {"Boot", "Ready", "Listening", "Uploading", "Processing", "Speaking", "Stopped", "Error", "BookHandoff"})

    def test_pngs_and_every_atlas_pixel_match_cpp(self):
        for clip in self.manifest["clips"]:
            with Image.open(OUT / "stills" / f"{clip['name']}.png") as still:
                self.assertEqual(still.size, (128, 64))
                self.assertEqual(still.convert("L").tobytes(), self.pixels(clip, clip["still"]))
            with Image.open(OUT / "atlases" / f"{clip['name']}.png") as atlas:
                for i in range(clip["count"]):
                    x, y = (i % 10) * 128, (i // 10) * 64
                    self.assertEqual(atlas.crop((x, y, x + 128, y + 64)).tobytes(), self.pixels(clip, i))

    def test_gif_every_decoded_frame_and_duration_match_cpp(self):
        for clip in self.manifest["clips"]:
            with Image.open(OUT / "animations" / f"{clip['name']}.gif") as gif:
                index = 0
                for i in range(gif.n_frames):
                    gif.seek(i)
                    self.assertEqual(gif.size, (512, 256))
                    duration = gif.info["duration"]
                    self.assertEqual(duration % 100, 0)
                    actual = gif.convert("L").resize((128, 64), Image.Resampling.NEAREST).tobytes()
                    for _ in range(duration // 100):
                        self.assertLess(index, clip["count"])
                        self.assertEqual(actual, self.pixels(clip, index))
                        index += 1
                self.assertEqual(index, clip["count"])

    def test_child_never_exports_capture_or_network_state(self):
        for clip in self.manifest["clips"]:
            if clip["profile"] == "child-local":
                self.assertFalse(set(clip["visible_states"]) & {"Listening", "Uploading", "Processing"})

    def test_stopped_handoff_and_denied_are_static(self):
        for clip in self.manifest["clips"]:
            if clip["name"] in {"stopped", "book-handoff", "local-stopped", "local-book", "local-denied", "error"}:
                self.assertEqual(len({self.pixels(clip, i) for i in range(clip["count"])}), 1)

    def test_provenance_matches_current_source(self):
        record = json.loads((OUT / "provenance.json").read_text())
        self.assertEqual(record["raw_sha256"], hashlib.sha256(self.raw).hexdigest())
        for name, expected in record["source_sha256"].items():
            self.assertEqual(hashlib.sha256((ROOT / name).read_bytes()).hexdigest(), expected, name)


if __name__ == "__main__":
    unittest.main(verbosity=2)
