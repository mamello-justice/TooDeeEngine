import JSZip from 'jszip';

import './style.css';

import gradientSvg from '../../../assets/too-dee-icons/svg/too-dee-icon-gradient.svg?raw';
import whiteSvg from '../../../assets/too-dee-icons/svg/too-dee-icon-white.svg?raw';
import blackSvg from '../../../assets/too-dee-icons/svg/too-dee-icon-black.svg?raw';

const SIZES = [16, 32, 64, 128, 256, 512, 1024];

function loadSVG(svg: string) {
    const blob = new Blob([svg], { type: 'image/svg+xml' });
    return URL.createObjectURL(blob);
}

function drawSVG(ctx: CanvasRenderingContext2D, size: number, svgUrl: string) {
    const img = new Image();
    img.src = svgUrl;
    return new Promise<void>((resolve) => {
        img.onload = () => {
            ctx.clearRect(0, 0, size, size);
            ctx.drawImage(img, 0, 0, size, size);
            resolve();
        };
    });
}

async function generate() {
    const whiteUrl = loadSVG(whiteSvg);
    const blackUrl = loadSVG(blackSvg);
    const gradientUrl = loadSVG(gradientSvg);
    const zip = new JSZip();
    const variants = [
        { url: gradientUrl, prefix: '' },
        { url: whiteUrl, prefix: 'white-' },
        { url: blackUrl, prefix: 'black-' },
    ];

    for (const size of SIZES) {
        for (const { url, prefix } of variants) {
            const canvas = document.createElement('canvas');
            canvas.width = canvas.height = size;
            const ctx = canvas.getContext('2d');

            if (!ctx) {
                throw new Error('Failed to get canvas context');
            }
            await drawSVG(ctx, size, url);

            const blob = await new Promise<Blob | null>((resolve) => canvas.toBlob(resolve, 'image/png'));
            if (!blob) {
                throw new Error('Failed to convert canvas to blob');
            }
            zip.file(`icon-${prefix}${size}.png`, blob);
        }
    }

    const content = await zip.generateAsync({ type: 'blob' });
    const a = document.createElement('a');
    a.download = 'too-dee-icons.zip';
    a.href = URL.createObjectURL(content);
    a.click();
}

async function loadPreview() {
    const preview = document.getElementById('preview') as HTMLCanvasElement | null;
    const ctx = preview?.getContext('2d');
    const size = preview?.width;
    const gradientUrl = loadSVG(gradientSvg);

    if (!ctx || !size) {
        throw new Error('Failed to get canvas context or size');
    }
    await drawSVG(ctx, size, gradientUrl);
}

const logoImg = document.getElementById('logo') as HTMLImageElement | null;
if (logoImg) {
    logoImg.src = loadSVG(gradientSvg);
}

const downloadButton = document.getElementById('gen') as HTMLButtonElement | null;
if (downloadButton) {
    downloadButton.onclick = generate;
    downloadButton.removeAttribute('disabled');
    loadPreview();
}

const generatedIcons = SIZES.flatMap((size) => [
    `icon-${size}.png`,
    `icon-white-${size}.png`,
    `icon-black-${size}.png`,
]).map((name) => {
    const li = document.createElement('li');
    li.textContent = name;
    return li;
});
document.getElementById('generated-icons')?.append(...generatedIcons);