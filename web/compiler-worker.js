const maximumSourceBytes =
  65536;

const maximumStdinBytes =
  65536;

const maximumReportBytes =
  6000000;

const textEncoder =
  new TextEncoder();

const textDecoder =
  new TextDecoder();

let wasmExports = null;

async function instantiateCompiler() {
  const response =
    await fetch("cinder.wasm");

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
    "stdin_ptr",
    "input_capacity",
    "stdin_capacity",
    "set_stdin_len",
    "report_ptr",
    "report_len",
    "compile"
  ];

  for (
    const exportName of requiredExports
  ) {
    if (!exports[exportName]) {
      throw new Error(
        `Missing WebAssembly export: ${exportName}`
      );
    }
  }

  return exports;
}

function createFailureReport(message) {
  return {
    ok: false,
    error: message,
    errorStart: 0,
    errorEnd: 0,
    result: 0,
    steps: 0,
    stdout: "",
    diagnostics: [
      {
        kind: "error",
        message,
        token: 0,
        start: 0,
        end: 0,
        line: 1,
        column: 1
      }
    ],
    tokens: [],
    nodes: [],
    types: [],
    functions: [],
    instructions: []
  };
}

function writeTextToMemory({
  text,
  pointer,
  capacity,
  label
}) {
  const bytes =
    textEncoder.encode(text);

  if (
    bytes.length >=
    capacity
  ) {
    throw new Error(
      `${label} exceeds Cinder's ${Math.floor(
        capacity / 1024
      )} KB limit.`
    );
  }

  const memory =
    new Uint8Array(
      wasmExports.memory.buffer
    );

  if (
    pointer < 0 ||
    pointer +
        bytes.length +
        1 >
      memory.length
  ) {
    throw new Error(
      `${label} does not fit in WebAssembly memory.`
    );
  }

  memory.set(
    bytes,
    pointer
  );

  memory[
    pointer +
    bytes.length
  ] = 0;

  return bytes.length;
}

function compileSource(
  sourceCode,
  stdinText
) {
  if (!wasmExports) {
    throw new Error(
      "The compiler engine is not ready."
    );
  }

  const sourceBytes =
    textEncoder.encode(
      sourceCode
    );

  if (
    sourceBytes.length >=
    maximumSourceBytes
  ) {
    return createFailureReport(
      "Source exceeds Cinder's 64 KB limit."
    );
  }

  const stdinBytes =
    textEncoder.encode(
      stdinText
    );

  if (
    stdinBytes.length >=
    maximumStdinBytes
  ) {
    return createFailureReport(
      "Program input exceeds Cinder's 64 KB limit."
    );
  }

  const inputCapacity =
    wasmExports.input_capacity();

  const stdinCapacity =
    wasmExports.stdin_capacity();

  const inputPointer =
    wasmExports.input_ptr();

  const stdinPointer =
    wasmExports.stdin_ptr();

  const writtenSourceLength =
    writeTextToMemory({
      text: sourceCode,
      pointer: inputPointer,
      capacity: inputCapacity,
      label: "Source"
    });

  if (
    writtenSourceLength !==
    sourceBytes.length
  ) {
    throw new Error(
      "Source encoding length mismatch."
    );
  }

  const writtenStdinLength =
    writeTextToMemory({
      text: stdinText,
      pointer: stdinPointer,
      capacity: stdinCapacity,
      label: "Program input"
    });

  if (
    writtenStdinLength !==
    stdinBytes.length
  ) {
    throw new Error(
      "Program input encoding length mismatch."
    );
  }

  wasmExports.set_stdin_len(
    writtenStdinLength
  );

  wasmExports.compile();

  const reportPointer =
    wasmExports.report_ptr();

  const reportLength =
    wasmExports.report_len();

  if (
    reportPointer < 0 ||
    reportLength < 0 ||
    reportLength >
      maximumReportBytes ||
    reportPointer +
        reportLength >
      wasmExports.memory.buffer
        .byteLength
  ) {
    throw new Error(
      "The compiler returned an invalid report."
    );
  }

  const reportBytes =
    new Uint8Array(
      wasmExports.memory.buffer,
      reportPointer,
      reportLength
    );

  const reportText =
    textDecoder.decode(
      reportBytes
    );

  return JSON.parse(
    reportText
  );
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
    const message =
      event.data;

    if (
      !message ||
      message.type !== "compile"
    ) {
      return;
    }

    try {
      const report =
        compileSource(
          String(
            message.source || ""
          ),
          String(
            message.stdin || ""
          )
        );

      self.postMessage({
        type: "result",
        requestId:
          message.requestId,
        report
      });
    } catch (error) {
      self.postMessage({
        type: "compile-error",
        requestId:
          message.requestId,
        message:
          error instanceof Error
            ? error.message
            : "Compilation failed unexpectedly."
      });
    }
  }
);

startWorker();