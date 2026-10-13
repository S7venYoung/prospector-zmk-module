import fs from 'node:fs';
import path from 'node:path';

// Mechanical 4bpp -> 2bpp conversion of uncompressed LVGL glyph bitmaps.
// Keep every glyph's geometry and advance; verify every output alpha sample.
for (const name of ['impact_48', 'impact_56']) {
    const file = path.resolve('boards/shields/prospector_scanner/src/fonts_codex', name + '.c');
    let src = fs.readFileSync(file, 'utf8');
    if (!/\.bpp = 4,/.test(src) || !/\.bitmap_format = 0,/.test(src))
        throw new Error('Expected uncompressed 4bpp font: ' + name);
    const bitmap = /glyph_bitmap\[\] = \{([\s\S]*?)\n\};/.exec(src);
    const glyphs = /glyph_dsc\[\] = \{([\s\S]*?)\n\};/.exec(src);
    if (!bitmap || !glyphs) throw new Error('Missing font tables');
    const clean = bitmap[1].replace(/\/\*[\s\S]*?\*\//g, '');
    const input = [...clean.matchAll(/0x([0-9a-f]+)/gi)].map(m => parseInt(m[1], 16));
    const output = [];
    let count = 0;
    const descriptors = glyphs[1].replace(
        /\.bitmap_index = (\d+), \.adv_w = (\d+), \.box_w = (\d+), \.box_h = (\d+)/g,
        (whole, offset, advance, width, height) => {
            offset = Number(offset);
            const pixels = Number(width) * Number(height);
            if (offset + Math.ceil(pixels / 2) > input.length) throw new Error('Input bounds');
            const start = output.length;
            for (let i = 0; i < pixels; i += 4) {
                let byte = 0;
                for (let j = 0; j < 4 && i + j < pixels; j++) {
                    const n = i + j;
                    const sample = (input[offset + (n >> 1)] >> ((n & 1) ? 0 : 4)) & 15;
                    const q = Math.round(sample / 5);
                    byte |= q << (6 - j * 2);
                }
                output.push(byte);
            }
            for (let i = 0; i < pixels; i++) {
                const before = (input[offset + (i >> 1)] >> ((i & 1) ? 0 : 4)) & 15;
                const after = ((output[start + (i >> 2)] >> (6 - (i & 3) * 2)) & 3) * 5;
                if (Math.abs(before - after) > 2) throw new Error('Alpha mismatch');
            }
            count++;
            return whole.replace('.bitmap_index = ' + offset, '.bitmap_index = ' + start);
        });
    if (count < 90) throw new Error('Unexpected glyph count');
    const lines = [];
    for (let i = 0; i < output.length; i += 16)
        lines.push('    ' + output.slice(i, i + 16).map(v => '0x' + v.toString(16).padStart(2, '0')).join(', ') + ',');
    src = src.replace(bitmap[0], 'glyph_bitmap[] = {\n' + lines.join('\n') + '\n};');
    src = src.replace(glyphs[0], 'glyph_dsc[] = {' + descriptors + '\n};');
    src = src.replace('.bpp = 4,', '.bpp = 2,').replace('--bpp 4', '--bpp 2');
    fs.writeFileSync(file, src);
    console.log(name + ': ' + count + ' glyphs verified; saved ' + (input.length-output.length) + ' flash bytes');
}
