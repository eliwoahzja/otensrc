// Minimal static server for the site preview. No dependencies, so the preview
// command is just `node site/serve.mjs`; PORT is injected by the host.
import { createServer } from "node:http";
import { readFile, stat } from "node:fs/promises";
import { extname, join, normalize, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";

const root = resolve(fileURLToPath(new URL(".", import.meta.url)));
const port = Number(process.env.PORT || 4173);
const host = process.env.HOST || "0.0.0.0";

const TYPES = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "text/javascript; charset=utf-8",
  ".mjs": "text/javascript; charset=utf-8",
  ".svg": "image/svg+xml",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".webp": "image/webp",
  ".ico": "image/x-icon",
  ".json": "application/json; charset=utf-8",
  ".txt": "text/plain; charset=utf-8",
  ".md": "text/markdown; charset=utf-8"
};

async function resolveFile(urlPath) {
  let rel = decodeURIComponent(urlPath.split("?")[0].split("#")[0]);
  if (rel.endsWith("/")) rel += "index.html";
  const target = resolve(join(root, normalize(rel)));
  if (target !== root && !target.startsWith(root + sep)) return null;
  try {
    const info = await stat(target);
    if (info.isDirectory()) return resolveFile(rel + "/");
    if (info.isFile()) return target;
  } catch {
    return null;
  }
  return null;
}

createServer(async (req, res) => {
  const file = await resolveFile(req.url || "/");
  if (!file) {
    res.writeHead(404, { "content-type": "text/plain; charset=utf-8" });
    res.end("404 — " + (req.url || "/") + "\n");
    return;
  }
  try {
    const body = await readFile(file);
    res.writeHead(200, {
      "content-type": TYPES[extname(file)] || "application/octet-stream",
      "content-length": body.byteLength,
      "cache-control": "no-store"
    });
    res.end(req.method === "HEAD" ? undefined : body);
  } catch (err) {
    res.writeHead(500, { "content-type": "text/plain; charset=utf-8" });
    res.end("500 — " + err.message + "\n");
  }
}).listen(port, host, () => {
  console.log(`equinox site preview -> http://${host}:${port}/ (root: ${root})`);
});
