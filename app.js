const expression = document.querySelector('#expression');
const result = document.querySelector('#result');
const history = document.querySelector('#history');
const angleMode = document.querySelector('#angle-mode');
const angleButton = document.querySelector('#angle-button');
let current = '';
let degrees = true;

function format(value) {
  if (!Number.isFinite(value)) throw new Error('undefined');
  return Number(value.toPrecision(12)).toString();
}

function evaluate(source) {
  let text = source.replaceAll('π', 'Math.PI').replaceAll('^', '**').replaceAll('√', 'Math.sqrt');
  text = text.replace(/\be\b/g, 'Math.E').replace(/(sin|cos|tan|asin|acos|atan|log|ln|sqrt)\(/g, (_, fn) => {
    const mapped = { sin:'sin', cos:'cos', tan:'tan', asin:'asin', acos:'acos', atan:'atan', log:'log10', ln:'log', sqrt:'sqrt' }[fn];
    return `Math.${mapped}(`;
  });
  if (degrees) text = text.replace(/Math\.(sin|cos|tan)\(([^()]*)\)/g, 'Math.$1(($2) * Math.PI / 180)');
  if (degrees) text = text.replace(/Math\.(asin|acos|atan)\(([^()]*)\)/g, '(Math.$1($2) * 180 / Math.PI)');
  if (!/^[0-9+\-*/().,%\sA-Za-z]+$/.test(text)) throw new Error('invalid');
  return Function(`"use strict"; return (${text})`)();
}

function render() { expression.textContent = current || '0'; }
function addHistory(input, answer) {
  const item = document.createElement('div');
  item.className = 'history-item';
  item.innerHTML = `<span>${input.replaceAll('*', '×').replaceAll('/', '÷')}</span><strong>${answer}</strong><small>just now · ${degrees ? 'DEG' : 'RAD'}</small>`;
  history.prepend(item);
  while (history.children.length > 5) history.lastElementChild.remove();
}
function calculate() {
  if (!current) return;
  try { const answer = format(evaluate(current)); result.textContent = answer; addHistory(current, answer); }
  catch { result.textContent = 'Error'; }
}

document.querySelector('#keypad').addEventListener('click', event => {
  const key = event.target.closest('button');
  if (!key) return;
  if (key.dataset.action === 'clear') { current = ''; result.textContent = '0'; }
  else if (key.dataset.action === 'backspace') { current = current.slice(0, -1); }
  else if (key.dataset.action === 'calculate') calculate();
  else { current += key.dataset.value; }
  render();
});
angleButton.addEventListener('click', () => { degrees = !degrees; const mode = degrees ? 'DEG' : 'RAD'; angleButton.textContent = mode; angleMode.textContent = mode; });
document.querySelector('#clear-history').addEventListener('click', () => { history.innerHTML = ''; });
document.addEventListener('keydown', event => { if (event.key === '/' && document.activeElement.tagName !== 'INPUT') { event.preventDefault(); document.querySelector('#search').focus(); } if (event.key === 'Enter' && (event.metaKey || event.ctrlKey)) calculate(); });
document.querySelectorAll('.calc-select').forEach(card => card.addEventListener('click', () => { const name = card.dataset.name; document.querySelector('#calculator-title').textContent = name; document.querySelector('#calc-model').textContent = name; document.querySelector('#calculator-brand').textContent = card.dataset.brand; document.querySelector('#calculator-brand').className = `brand-chip ${card.dataset.brand === 'CASIO' ? 'casio' : 'ti'}`; document.querySelector('.workbench').scrollIntoView({ behavior:'smooth', block:'start' }); }));
document.querySelector('#theme-toggle').addEventListener('click', () => document.body.classList.toggle('warm-mode'));
document.querySelector('#search').addEventListener('input', event => { const query = event.target.value.toLowerCase(); document.querySelectorAll('.model-pill').forEach(model => { model.hidden = query && !model.textContent.toLowerCase().includes(query); }); document.querySelectorAll('.timeline-row').forEach(row => { row.hidden = query && ![...row.querySelectorAll('.model-pill')].some(model => !model.hidden); }); });
document.querySelector('#view-all').addEventListener('click', () => { document.querySelector('#timeline').scrollIntoView({ behavior:'smooth' }); });
document.querySelector('#copy-history').addEventListener('click', async event => { const text = [...history.querySelectorAll('.history-item')].map(item => `${item.children[0].textContent} = ${item.children[1].textContent}`).join('\n'); try { await navigator.clipboard.writeText(text); event.currentTarget.innerHTML = 'Copied <span>✓</span>'; setTimeout(() => { event.currentTarget.innerHTML = 'Export history <span>↗</span>'; }, 1400); } catch { event.currentTarget.textContent = 'Select history to copy'; } });