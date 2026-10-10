# OLED renderer previews

Open `index.html` directly in a browser. Select a state or sequence, press Play,
or scrub the frame slider. Playback is opt-in and pauses when the tab is hidden;
reduced-motion preference keeps it paused. `contact-sheet.png` provides all nine
adult states, six local states, and a denied local capture request.

`animations/` contains 19 GIFs generated from the compiled C++ renderer.
`stills/` contains native 128×64 PNGs; `atlases/` provides the same frame pixels
to the HTML canvas. `frames.raw` stores 1024 page-format bytes per frame, with
offsets in `frames.json`. No separate illustration or Python character renderer
is involved. `provenance.json` records source/raw hashes; `pixel-hashes.json`
records every decoded frame hash.

Previews sample at 10 FPS. GIF may combine repeated identical frames; cumulative
duration still matches the C++ sample sequence. Speech uses a synthetic TX
envelope fixture, not audio playback or lip-sync evidence. Local-flow fixtures
demonstrate presentation only, not deployment isolation or approved child content.

Hardware/controller/pinout/offset, actual display size/contrast, 0.5 m readability,
target-device FPS and audio scheduling remain **unverified**.

Reproduction and integration: `../../workstreams/oled/README.md`.
Test evidence: `../../workstreams/oled/RESULTS.md`.
