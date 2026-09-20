/*
 * Cinder compiler worker
 *
 * Compilation happens outside the browser's main interface
 * thread. If a program takes too long, app.js can terminate
 * this worker without freezing the editor or page.
 */

const textEncoder = new TextEncoder();
const textDecoder = new TextDecoder();

let wasmExports = null;

async function instantiateCompiler() {
  const response = await fetch(
    "cinder.wasm"
  );

  if (!response.ok) {
    throw new Error(
      `Could not fetch cinder.wasm: ${response.status}`
    );
  }

  let module;

  try {
    module =
      await WebAssembly.instantiateStreaming(
        response.clone(),
        {}
      );
  } catch {
    const bytes =
      await response.arrayBuffer();

    module =
      await WebAssembly.instantiate(
        bytes,
        {}
      );
  }

  const exports =
    module.instance.exports;

  const requiredExports = [
    "memory",
    "input_ptr",
    "report_ptr",
    "report_len",
    "compile"
  ];

  for (const exportName of requiredExports) {
    if (!exports[exportName]) {
      throw new Error(
        `Missing WebAssembly export: ${exportName}`
      );
    }
  }

  return exports;
}

function compileSource(sourceCode) {
  if (!wasmExports) {
    throw new Error(
      "The compiler engine is not ready."
    );
  }

  const sourceBytes =
    textEncoder.encode(sourceCode);

  if (sourceBytes.length >= 32768) {
    return {
      ok: false,
      error:
        "Source exceeds Cinder's 32 KB limit.",
      errorStart: 0,
      errorEnd: 0,
      result: 0,
      steps: 0,
      stdout: "",
      tokens: [],
      nodes: [],
      functions: [],
      instructions: []
    };
  }

  const inputPointer =
    wasmExports.input_ptr();

  const memory =
    new Uint8Array(
      wasmExports.memory.buffer
    );

  memory.set(
    sourceBytes,
    inputPointer
  );

  memory[
    inputPointer + sourceBytes.length
  ] = 0;

  wasmExports.compile();

  const reportPointer =
    wasmExports.report_ptr();

  const reportLength =
    wasmExports.report_len();

  if (
    reportLength < 0 ||
    reportLength > 3000000
  ) {
    throw new Error(
      "The compiler returned an invalid report length."
    );
  }

  const reportBytes =
    new Uint8Array(
      wasmExports.memory.buffer,
      reportPointer,
      reportLength
    );

  const reportText =
    textDecoder.decode(reportBytes);

  return JSON.parse(reportText);
}

async function startWorker() {
  try {
    wasmExports =
      await instantiateCompiler();

    self.postMessage({
      type: "ready"
    });
  } catch (error) {
    self.postMessage({
      type: "engine-error",
      message:
        error instanceof Error
          ? error.message
          : "The compiler could not be loaded."
    });
  }
}

self.addEventListener(
  "message",
  (event) => {
    const message = event.data;

    if (
      !message ||
      message.type !== "compile"
    ) {
      return;
    }

    try {
      const report =
        compileSource(
          String(message.source || "")
        );

      self.postMessage({
        type: "result",
        requestId: message.requestId,
        report
      });
    } catch (error) {
      self.postMessage({
        type: "compile-error",
        requestId: message.requestId,
        message:
          error instanceof Error
            ? error.message
            : "Compilation failed unexpectedly."
      });
    }
  }
);

startWorker();