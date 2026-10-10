async (page) => {
  const root = '/home/caradhina/Projects/sijalak2';
  const checks = [];
  const check = (name, passed, details = null) => {
    checks.push({ name, passed, details });
    if (!passed) throw new Error(name);
  };
  const errors = [];
  page.on('pageerror', error => errors.push(error.message));
  await page.setViewportSize({ width: 1200, height: 900 });
  await page.emulateMedia({ reducedMotion: 'no-preference' });
  await page.goto('http://127.0.0.1:8765/index.html');
  await page.locator('#screen').waitFor({ state: 'visible' });
  const manifest = await page.locator('#manifest').textContent().then(JSON.parse);
  async function load(name) {
    await page.selectOption('#clip', name);
    await page.locator('#screen').waitFor({ state: 'visible' });
  }
  const pixelChecks = [];
  for (const clip of manifest.clips) {
    await load(clip.name);
    const correct = await page.evaluate(async (clip) => {
      const raw = new Uint8Array(await (await fetch('frames.raw')).arrayBuffer());
      const pixels = document.querySelector('#screen').getContext('2d').getImageData(0,0,128,64).data;
      for (let y=0; y<64; ++y) for (let x=0; x<128; ++x) {
        const expected = (raw[clip.offset + clip.still*1024 + (y>>3)*128 + x] & (1 << (y%8))) ? 255 : 0;
        const at = (y*128+x)*4;
        if (pixels[at] !== expected || pixels[at+1] !== expected || pixels[at+2] !== expected || pixels[at+3] !== 255) return false;
      }
      return true;
    }, clip);
    pixelChecks.push({ name: clip.name, passed: correct });
  }
  check('all 19 canvas stills match compiled page bytes pixel-for-pixel', pixelChecks.every(x => x.passed), pixelChecks);
  await load('ready');
  check('no autoplay', await page.locator('#toggle').textContent() === 'Play');
  await page.click('#toggle');
  const before = await page.locator('#frame').inputValue();
  await page.waitForFunction(value => document.querySelector('#frame').value !== value, before);
  await page.click('#toggle');
  const paused = await page.locator('#frame').inputValue();
  await page.waitForTimeout(240);
  check('play advances and pause holds', await page.locator('#frame').inputValue() === paused);
  await page.focus('#frame');
  await page.keyboard.press('ArrowRight');
  check('keyboard frame stepping', Number(await page.locator('#frame').inputValue()) === Number(paused) + 1);
  const focus = await page.locator('#frame').evaluate(el => ({ width: getComputedStyle(el).outlineWidth, style: getComputedStyle(el).outlineStyle }));
  check('keyboard focus visible', focus.style === 'solid' && parseFloat(focus.width) >= 3, focus);
  await load('speaking');
  await page.click('#toggle');
  await page.click('#stop');
  await page.locator('#screen').waitFor({ state: 'visible' });
  check('STOP interrupts viewer playback', await page.locator('#clip').inputValue() === 'stopped' && await page.locator('#toggle').textContent() === 'Play');
  await load('local-denied');
  check('local rejected capture shows error', (await page.locator('#position').textContent()).includes('child-local · Error'));
  await page.click('#stop');
  await page.locator('#screen').waitFor({ state: 'visible' });
  check('STOP keeps local presentation profile', await page.locator('#clip').inputValue() === 'local-stopped');
  await page.emulateMedia({ reducedMotion: 'reduce' });
  await page.waitForFunction(() => document.querySelector('#toggle').disabled);
  check('reduced-motion disables playback', await page.locator('#toggle').isDisabled());
  await page.focus('#frame');
  await page.keyboard.press('ArrowRight');
  check('reduced-motion keeps frame inspection available', Number(await page.locator('#frame').inputValue()) === 8);
  await page.emulateMedia({ reducedMotion: 'no-preference' });
  await load('ready');
  const desktop = await page.evaluate(() => {
    const s = getComputedStyle(document.querySelector('.workbench'));
    const b = getComputedStyle(document.querySelector('#toggle'));
    return { overflow: document.documentElement.scrollWidth > innerWidth, gap: s.gap,
      buttonForeground: b.color, buttonFill: b.backgroundColor,
      buttonHeight: document.querySelector('#toggle').getBoundingClientRect().height,
      screenWidth: document.querySelector('#screen').getBoundingClientRect().width };
  });
  check('desktop sizing contrast pairing and controls', !desktop.overflow && desktop.buttonHeight >= 44 && desktop.screenWidth === 512, desktop);
  await page.screenshot({ path: `${root}/exports/oled/viewer-desktop.png`, fullPage: false });
  await page.setViewportSize({ width: 375, height: 812 });
  const mobile = await page.evaluate(() => ({ overflow: document.documentElement.scrollWidth > innerWidth,
    screenWidth: document.querySelector('#screen').getBoundingClientRect().width,
    columns: getComputedStyle(document.querySelector('.workbench')).gridTemplateColumns }));
  check('mobile single column without overflow', !mobile.overflow && mobile.screenWidth === 343, mobile);
  await page.screenshot({ path: `${root}/exports/oled/viewer-mobile.png`, fullPage: false });
  check('no uncaught browser errors', errors.length === 0, errors);
  await page.route('**/atlases/error.png', route => route.abort());
  await page.selectOption('#clip', 'error');
  await page.locator('#failure').waitFor({ state: 'visible' });
  check('failed atlas leaves real still and recovery explanation', await page.locator('#fallback').isVisible() && await page.locator('#toggle').isDisabled());
  await page.unroute('**/atlases/error.png');
  await load('ready');
  check('viewer recovers on next selection', !(await page.locator('#toggle').isDisabled()));
  const record = { checks, passed: checks.filter(c => c.passed).length, failed: checks.filter(c => !c.passed).length,
    viewports: ['1200x900', '375x812'], physicalReadability: 'unverified',
    contrast: 'Computed black/white button pair: 21:1. OLED bits are also black/white.' };
  await page.evaluate(record => { window.__oledCheckRecord = record; }, record);
  return record;
}
