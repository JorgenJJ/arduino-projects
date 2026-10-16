// Docsify plugin: replaces a link to diagram.json with a drawing of the Wokwi circuit
(() => {
  const elements = import('https://cdn.jsdelivr.net/npm/@wokwi/elements@1.9.2/+esm');

  // @wokwi/elements has only the SPI OLED, so draw a simple I2C one
  customElements.define('board-ssd1306', class extends HTMLElement {
    pinInfo = ['GND', 'VCC', 'SCL', 'SDA'].map((name, i) => ({ name, x: 61 + i * 9.6, y: 6 }));
    connectedCallback() {
      this.innerHTML = `<svg width="150" height="110" font-family="sans-serif" font-size="5" fill="#fff" text-anchor="middle">
        <rect width="150" height="110" rx="4" fill="#1d3f75"/>
        <rect x="10" y="24" width="130" height="72" fill="#111"/>
        ${this.pinInfo.map(p => `<circle cx="${p.x}" cy="${p.y}" r="3" fill="#c9a33a"/><text x="${p.x}" y="${p.y + 10}">${p.name}</text>`).join('')}
      </svg>`;
    }
  });

  // Seeed XIAO ESP32-C6 (top view, USB up)
  customElements.define('board-xiao-esp32-c6', class extends HTMLElement {
    pinInfo = [...['D0', 'D1', 'D2', 'D3', 'D4', 'D5', 'D6'].map((name, i) => ({ name, x: 5, y: 22 + i * 10 })),
      ...['5V', 'GND', '3V3', 'D10', 'D9', 'D8', 'D7'].map((name, i) => ({ name, x: 67, y: 22 + i * 10 }))];
    connectedCallback() {
      this.innerHTML = `<svg width="72" height="92" font-family="sans-serif" font-size="5" fill="#fff">
        <rect width="72" height="92" rx="4" fill="#1a1a1a"/>
        <rect x="24" y="-4" width="24" height="12" rx="2" fill="#999"/>
        <rect x="22" y="36" width="28" height="28" fill="#333"/>
        <text x="36" y="86" text-anchor="middle">XIAO C6</text>
        ${this.pinInfo.map(p => `<circle cx="${p.x}" cy="${p.y}" r="3" fill="#c9a33a"/><text x="${p.x < 36 ? p.x + 5 : p.x - 5}" y="${p.y + 2}" text-anchor="${p.x < 36 ? 'start' : 'end'}">${p.name}</text>`).join('')}
      </svg>`;
    }
  });

  const walk =([x, y], steps) => [[x, y], ...steps.map(s => s[0] === 'v' ? [x, y += +s.slice(1)] : [x += +s.slice(1), y])];

  async function draw(link, url) {
    const [, diagram] = await Promise.all([elements, fetch(url).then(r => r.json())]);
    const wrapper = document.createElement('div');
    const canvas = wrapper.appendChild(document.createElement('div'));
    const inner = canvas.appendChild(document.createElement('div'));
    wrapper.className = 'wokwi';
    canvas.style.position = 'relative';
    inner.style.position = 'absolute';

    const parts = {};
    for (const p of diagram.parts) {
      const el = parts[p.id] = inner.appendChild(document.createElement(p.type));
      Object.entries(p.attrs ?? {}).forEach(([k, v]) => el.setAttribute(k, v));
      el.style.cssText = `position:absolute;left:${p.left}px;top:${p.top}px`;
    }
    link.replaceWith(wrapper);
    await Promise.all(Object.values(parts).map(el => el.updateComplete));

    const at = ref => {
      const [id, name] = ref.split(':');
      const pin = parts[id].pinInfo.find(p => p.name === name);
      return [parts[id].offsetLeft + pin.x, parts[id].offsetTop + pin.y];
    };
    const wires = diagram.connections.filter(c => c[2]).map(([from, to, color, path = []]) => {
      const star = path.indexOf('*');
      const src = walk(at(from), star < 0 ? path : path.slice(0, star));
      const [tx, ty] = at(to);
      const back = star < 0 ? [] : path.slice(star + 1).reverse().map(s => s[0] + -s.slice(1));
      const dst = walk([tx, ty], back).reverse();
      const [sx, sy] = src.at(-1);
      return { color, points: [...src, [dst[0][0], sy], ...dst] };
    });

    const xs = [], ys = [];
    Object.values(parts).forEach(el => { xs.push(el.offsetLeft, el.offsetLeft + el.offsetWidth); ys.push(el.offsetTop, el.offsetTop + el.offsetHeight); });
    wires.forEach(w => w.points.forEach(([x, y]) => { xs.push(x); ys.push(y); }));
    const [minX, minY, pad] = [Math.min(...xs), Math.min(...ys), 10];
    canvas.style.width = `${Math.max(...xs) - minX + 2 * pad}px`;
    canvas.style.height = `${Math.max(...ys) - minY + 2 * pad}px`;
    inner.style.left = `${pad - minX}px`;
    inner.style.top = `${pad - minY}px`;

    inner.insertAdjacentHTML('beforeend', `<svg style="position:absolute;overflow:visible;pointer-events:none" width="1" height="1">
      ${wires.map(w => `<polyline points="${w.points.join(' ')}" stroke="${w.color}" stroke-width="2" fill="none" stroke-linejoin="round"/>`).join('')}
    </svg>`);
  }

  window.$docsify.plugins = [...(window.$docsify.plugins ?? []), (hook, vm) => hook.doneEach(() => {
    document.querySelectorAll('.markdown-section a[href$=".json"]').forEach(link =>
      draw(link, vm.route.file.replace(/[^/]*$/, link.getAttribute('href').split('/').pop())));
  })];
})();
