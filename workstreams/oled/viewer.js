"use strict";
(() => {
  const data = JSON.parse(document.getElementById("manifest").textContent);
  const choose = document.getElementById("clip");
  const slider = document.getElementById("frame");
  const toggle = document.getElementById("toggle");
  const screen = document.getElementById("screen");
  const ctx = screen.getContext("2d");
  const reduced = matchMedia("(prefers-reduced-motion: reduce)");
  let clip, atlas, timer = null, frame = 0, request = 0;
  const pause = () => { clearInterval(timer); timer = null; toggle.textContent = "Play"; };
  function draw() {
    if (!atlas) return;
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(atlas, (frame % 10) * 128, Math.floor(frame / 10) * 64, 128, 64, 0, 0, 128, 64);
    slider.value = frame;
    const description = `${clip.profile} · ${clip.visible_states[frame]} · frame ${frame + 1}/${clip.count} · ${frame * data.duration_ms} ms`;
    document.getElementById("position").textContent = description;
    screen.setAttribute("aria-label", `Actual Jalak framebuffer: ${description}`);
  }
  async function load(name) {
    pause(); atlas = null; toggle.disabled = true;
    const token = ++request;
    clip = data.clips.find(c => c.name === name);
    choose.value = clip.name;
    frame = clip.still;
    slider.max = clip.count - 1;
    const fallback = document.getElementById("fallback");
    fallback.src = `stills/${clip.name}.png`;
    fallback.alt = `Actual Jalak ${clip.label} framebuffer`;
    fallback.hidden = false; screen.hidden = true;
    document.getElementById("download").href = `animations/${clip.name}.gif`;
    const image = new Image();
    image.src = `atlases/${clip.name}.png`;
    try {
      await image.decode();
      if (token !== request) return;
      atlas = image; fallback.hidden = true; screen.hidden = false;
      document.getElementById("failure").hidden = true;
      toggle.disabled = reduced.matches; draw();
    } catch (_) {
      if (token !== request) return;
      document.getElementById("failure").textContent = "Frame atlas unavailable. The still and contact sheet remain available. Regenerate previews to restore playback.";
      document.getElementById("failure").hidden = false;
    }
  }
  choose.addEventListener("change", () => load(choose.value));
  slider.addEventListener("input", () => { pause(); frame = Number(slider.value); draw(); });
  toggle.addEventListener("click", () => {
    if (timer !== null) { pause(); return; }
    if (!atlas || reduced.matches) return;
    toggle.textContent = "Pause";
    timer = setInterval(() => { frame = (frame + 1) % clip.count; draw(); }, data.duration_ms);
  });
  document.getElementById("stop").addEventListener("click", () =>
    load(clip.profile === "child-local" ? "local-stopped" : "stopped"));
  document.addEventListener("visibilitychange", () => { if (document.hidden) pause(); });
  reduced.addEventListener("change", () => { pause(); toggle.disabled = reduced.matches || !atlas; });
  load("ready");
})();
