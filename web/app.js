const developerEmail =
  "ccompilerinc@gmail.com";

const compilationTimeoutMilliseconds =
  3000;

const maximumStdinBytes =
  65535;

const textEncoder =
  new TextEncoder();

const maximumRenderedTokens =
  1200;

const maximumRenderedInstructions =
  1500;

const maximumRenderedTreeNodes =
  1500;

const examples = {
  fibonacci: `int fibonacci(int number) {
  if (number < 2) {
    return number;
  }

  return fibonacci(number - 1) + fibonacci(number - 2);
}

int main() {
  int index = 0;

  while (index < 10) {
    printf(
      "fib(%d) = %d\\n",
      index,
      fibonacci(index)
    );

    index = index + 1;
  }

  return 0;
}`,

  factorial: `int factorial(int number) {
  if (number <= 1) {
    return 1;
  }

  return number * factorial(number - 1);
}

int main() {
  int answer = factorial(7);

  printf("7! = %d\\n", answer);

  return 0;
}`,

  fizzbuzz: `int main() {
  int number = 1;

  while (number <= 30) {
    if (number % 15 == 0) {
      printf("FizzBuzz\\n");
    } else if (number % 3 == 0) {
      printf("Fizz\\n");
    } else if (number % 5 == 0) {
      printf("Buzz\\n");
    } else {
      printf("%d\\n", number);
    }

    number = number + 1;
  }

  return 0;
}`,

  controlFlow: `int main() {
  int total = 0;

  for (
    int number = 0;
    number < 20;
    number = number + 1
  ) {
    if (number == 4) {
      continue;
    }

    if (number == 12) {
      break;
    }

    total = total + number;
  }

  printf("total = %d\\n", total);

  return total;
}`,

    memory: `int global_numbers[5] = {
  3,
  5,
  8,
  13,
  21
};

char global_message[] =
  "Cinder memory works";

int sum_values(
  int values[],
  int count
) {
  int index = 0;
  int total = 0;

  while (index < count) {
    total += values[index];
    index++;
  }

  return total;
}

int main() {
  int local_numbers[5] = {
    2,
    4,
    6,
    8,
    10
  };

  int matrix[2][3] = {
    {
      1,
      2,
      3
    },
    {
      4,
      5,
      6
    }
  };

  char message[] =
    "arrays + pointers + strings";

  int *pointer =
    &local_numbers[0];

  int global_total =
    sum_values(
      global_numbers,
      5
    );

  int local_total =
    sum_values(
      pointer,
      5
    );

  printf("%s\\n", global_message);
  printf("%s\\n", message);

  printf(
    "global=%d local=%d matrix=%d\\n",
    global_total,
    local_total,
    matrix[1][2]
  );

  printf(
    "pointer[2]=%d sizeof=%u\\n",
    *(pointer + 2),
    sizeof(local_numbers)
  );

  return global_total + local_total;
}`,

  structures: `enum Status {
  STATUS_IDLE = 2,
  STATUS_RUNNING,
  STATUS_DONE = 8
};

struct Point {
  int x;
  int y;
};

struct Record {
  struct Point position;
  unsigned short code;
  char grade;
};

int classify(int value) {
  switch (value) {
    case 0:
      return 100;

    case 1:
    case 2:
      return 200;

    case 3:
      return 300;

    default:
      return -1;
  }
}

int point_total(
  struct Point *point
) {
  return point->x + point->y;
}

int main() {
  const int fixed = 7;

  struct Point point = {
    12,
    30
  };

  struct Record record = {
    {
      4,
      9
    },
    65530,
    'A'
  };

  enum Status status =
    STATUS_RUNNING;

  printf(
    "point=%d,%d total=%d\\n",
    point.x,
    point.y,
    point_total(&point)
  );

  printf(
    "record=%d,%d code=%u grade=%c\\n",
    record.position.x,
    record.position.y,
    record.code,
    record.grade
  );

  printf(
    "enum=%d switch=%d const=%d\\n",
    status,
    classify(2),
    fixed
  );

  printf(
    "sizes=%u,%u,%u,%u\\n",
    sizeof(char),
    sizeof(short),
    sizeof(int),
    sizeof(struct Point)
  );

  return point_total(&point);
}`,

  standardInput: `int main() {
  char text[64];
  int length = 0;
  int character;

  character = getchar();

  while (
    character != -1 &&
    character != '\\n' &&
    length < 63
  ) {
    text[length] =
      (char)character;

    length++;
    character = getchar();
  }

  text[length] = '\\0';

  printf(
    "input=%s\\n",
    text
  );

  printf(
    "length=%d\\n",
    length
  );

  return length;
}`,

  error: `int main() {
  int answer = 40;

  answer = answer + missing;

  printf("answer = %d\\n", answer);

  return 0;
}`
};

const exampleInputs = {
  fibonacci: "",
  factorial: "",
  fizzbuzz: "",
  controlFlow: "",
  memory: "",
  structures: "",
  standardInput:
    "Cinder browser input\n",
  error: ""
};

/* DOM references */

const codeEditor =
  document.querySelector("#codeEditor");

const lineNumbers =
  document.querySelector("#lineNumbers");

const sourceInformation =
  document.querySelector("#sourceInformation");

const sourceErrorMarker =
  document.querySelector("#sourceErrorMarker");

const stdinEditor =
  document.querySelector("#stdinEditor");

const stdinSize =
  document.querySelector("#stdinSize");

const clearStdinButton =
  document.querySelector(
    "#clearStdinButton"
  );

const exampleSelect =
  document.querySelector("#exampleSelect");

const runButton =
  document.querySelector("#runButton");

const resetButton =
  document.querySelector("#resetButton");

const programConsole =
  document.querySelector("#programConsole");

const compilerDiagnostic =
  document.querySelector("#compilerDiagnostic");

const diagnosticMessage =
  document.querySelector("#diagnosticMessage");

const diagnosticSourceButton =
  document.querySelector("#diagnosticSourceButton");

const tokenGrid =
  document.querySelector("#tokenGrid");

const syntaxTree =
  document.querySelector("#syntaxTree");

const instructionList =
  document.querySelector("#instructionList");

const tokenCount =
  document.querySelector("#tokenCount");

const instructionCount =
  document.querySelector("#instructionCount");

const outputCount =
  document.querySelector("#outputCount");

const copyOutputButton =
  document.querySelector("#copyOutputButton");

const executionFooter =
  document.querySelector("#executionFooter");

const executionStateText =
  document.querySelector("#executionStateText");

const executionInstructionStat =
  document.querySelector(
    "#executionInstructionStat"
  );

const executionStepStat =
  document.querySelector(
    "#executionStepStat"
  );

const executionExitStat =
  document.querySelector(
    "#executionExitStat"
  );

const compilationProgress =
  document.querySelector(
    "#compilationProgress"
  );

const headerEngineLight =
  document.querySelector(
    "#headerEngineLight"
  );

const headerEngineStatus =
  document.querySelector(
    "#headerEngineStatus"
  );

const footerEngineLight =
  document.querySelector(
    "#footerEngineLight"
  );

const footerEngineStatus =
  document.querySelector(
    "#footerEngineStatus"
  );

const contactDeveloperButton =
  document.querySelector(
    "#contactDeveloperButton"
  );

const footerContactButton =
  document.querySelector(
    "#footerContactButton"
  );

const toastElement =
  document.querySelector("#toast");

/* Application state */

let compilerWorker = null;
let compilerWorkerReady = false;

let pendingCompilation = null;
let nextRequestId = 1;

let currentExample = "fibonacci";
let latestReport = null;

let currentErrorStart = 0;
let currentErrorEnd = 0;

let toastTimer = null;
let initialCompilationStarted = false;

/* Utilities */

function escapeHtml(value) {
  return String(value).replace(
    /[&<>"']/g,
    (character) => {
      const entities = {
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;"
      };

      return entities[character];
    }
  );
}

function showToast(message) {
  toastElement.textContent = message;
  toastElement.classList.add("visible");

  window.clearTimeout(toastTimer);

  toastTimer = window.setTimeout(
    () => {
      toastElement.classList.remove(
        "visible"
      );
    },
    1800
  );
}

function wait(milliseconds) {
  return new Promise(
    (resolve) => {
      window.setTimeout(
        resolve,
        milliseconds
      );
    }
  );
}

function lineIndexAtOffset(offset) {
  return (
    codeEditor.value
      .slice(0, offset)
      .split("\n")
      .length - 1
  );
}

function lineCount() {
  return codeEditor.value
    .split("\n")
    .length;
}

/* Editor lines */

function updateLineNumbers(
  errorLine = -1
) {
  const totalLines =
    lineCount();

  let content = "";

  for (
    let index = 0;
    index < totalLines;
    index++
  ) {
    const className =
      index === errorLine
        ? "error-line"
        : "";

    content +=
      `<span class="${className}">` +
      `${index + 1}` +
      `</span>`;
  }

  lineNumbers.innerHTML =
    content;

  sourceInformation.textContent =
    `${totalLines} ` +
    `${totalLines === 1 ? "line" : "lines"}` +
    " · Cinder C";

  lineNumbers.scrollTop =
    codeEditor.scrollTop;
}

function updateActiveLine() {
  const activeLine =
    lineIndexAtOffset(
      codeEditor.selectionStart
    );

  const lineElements =
    lineNumbers.querySelectorAll("span");

  lineElements.forEach(
    (element, index) => {
      element.classList.toggle(
        "active-line",
        index === activeLine
      );
    }
  );
}

/* Source selection */

function selectSource(start, end) {
  const safeStart = Math.max(
    0,
    Math.min(
      start,
      codeEditor.value.length
    )
  );

  const safeEnd = Math.max(
    safeStart,
    Math.min(
      end,
      codeEditor.value.length
    )
  );

  const selectedLine =
    lineIndexAtOffset(safeStart);

  codeEditor.focus({
    preventScroll: true
  });

  codeEditor.setSelectionRange(
    safeStart,
    safeEnd
  );

  codeEditor.scrollTop = Math.max(
    0,
    selectedLine * 28 - 126
  );

  lineNumbers.scrollTop =
    codeEditor.scrollTop;

  updateActiveLine();
}

function selectToken(tokenIndex) {
  if (
    !latestReport ||
    !latestReport.tokens ||
    !latestReport.tokens[tokenIndex]
  ) {
    return;
  }

  const token =
    latestReport.tokens[tokenIndex];

  selectSource(
    token.start,
    token.end
  );
}

/* Standard input */

function getUtf8ByteCount(value) {
  return textEncoder.encode(value).length;
}

function formatByteCount(bytes) {
  if (bytes < 1024) {
    return `${bytes} B`;
  }

  const kibibytes =
    bytes / 1024;

  return `${kibibytes.toFixed(1)} KB`;
}

function updateStdinSize() {
  const bytes =
    getUtf8ByteCount(
      stdinEditor.value
    );

  stdinSize.textContent =
    `${formatByteCount(bytes)} / 64 KB`;

  stdinSize.classList.toggle(
    "warning",
    bytes >= maximumStdinBytes * 0.9 &&
      bytes <= maximumStdinBytes
  );

  stdinSize.classList.toggle(
    "error",
    bytes > maximumStdinBytes
  );

  return bytes;
}

function clearStandardInput(
  shouldAnnounce = true
) {
  stdinEditor.value = "";
  updateStdinSize();

  if (shouldAnnounce) {
    showToast(
      "Program input cleared."
    );
  }

  stdinEditor.focus();
}

/* Examples */

function loadExample(
  name,
  shouldRun = true
) {
  if (!examples[name]) {
    return;
  }

  currentExample = name;
  exampleSelect.value = name;
    codeEditor.value = examples[name];

  stdinEditor.value =
    exampleInputs[name] || "";

  updateStdinSize();

  sourceErrorMarker.hidden = true;
  compilerDiagnostic.hidden = true;

  updateLineNumbers();
  updateActiveLine();

  if (
    shouldRun &&
    compilerWorkerReady
  ) {
    runCode();
  }
}

/* Engine status */

function setEngineStatus(
  status,
  text
) {
  headerEngineLight.classList.remove(
    "ready",
    "error"
  );

  footerEngineLight.classList.remove(
    "ready",
    "error"
  );

  if (status) {
    headerEngineLight.classList.add(
      status
    );

    footerEngineLight.classList.add(
      status
    );
  }

  headerEngineStatus.textContent =
    text;

  footerEngineStatus.textContent =
    status === "ready"
      ? "C engine ready"
      : status === "error"
        ? "C engine unavailable"
        : "Loading C engine";
}

/* Worker lifecycle */

function rejectPendingCompilation(
  error
) {
  if (!pendingCompilation) {
    return;
  }

  window.clearTimeout(
    pendingCompilation.timeout
  );

  pendingCompilation.reject(error);
  pendingCompilation = null;
}

function destroyCompilerWorker() {
  rejectPendingCompilation(
    new Error(
      "The compiler worker was stopped."
    )
  );

  if (compilerWorker) {
    compilerWorker.terminate();
  }

  compilerWorker = null;
  compilerWorkerReady = false;
}

function createCompilerWorker() {
  destroyCompilerWorker();

  setEngineStatus(
    "",
    "loading"
  );

  const worker =
    new Worker(
      "compiler-worker.js",
      {
        type: "module"
      }
    );

  compilerWorker = worker;

  worker.addEventListener(
    "message",
    (event) => {
      if (worker !== compilerWorker) {
        return;
      }

      const message =
        event.data;

      if (!message) {
        return;
      }

      if (message.type === "ready") {
        compilerWorkerReady = true;

        setEngineStatus(
          "ready",
          "local"
        );

        if (!initialCompilationStarted) {
          initialCompilationStarted = true;
          runCode();
        }

        return;
      }

      if (
        message.type === "engine-error"
      ) {
        compilerWorkerReady = false;

        setEngineStatus(
          "error",
          "offline"
        );

        rejectPendingCompilation(
          new Error(message.message)
        );

        showCompilerDiagnostic(
          "Cinder could not load cinder.wasm. " +
          "Run scripts\\build.ps1 and reload the page."
        );

        return;
      }

      if (
        !pendingCompilation ||
        message.requestId !==
          pendingCompilation.requestId
      ) {
        return;
      }

      window.clearTimeout(
        pendingCompilation.timeout
      );

      const currentRequest =
        pendingCompilation;

      pendingCompilation = null;

      if (message.type === "result") {
        currentRequest.resolve(
          message.report
        );
      } else if (
        message.type === "compile-error"
      ) {
        currentRequest.reject(
          new Error(message.message)
        );
      }
    }
  );

  worker.addEventListener(
    "error",
    (event) => {
      if (worker !== compilerWorker) {
        return;
      }

      compilerWorkerReady = false;

      setEngineStatus(
        "error",
        "offline"
      );

      rejectPendingCompilation(
        new Error(
          event.message ||
          "The compiler worker crashed."
        )
      );
    }
  );
}

/* Worker compilation request */

function compileWithWorker(
  sourceCode,
  stdinText
) {
  if (
    !compilerWorker ||
    !compilerWorkerReady
  ) {
    return Promise.reject(
      new Error(
        "The C compiler is not ready."
      )
    );
  }

  if (pendingCompilation) {
    return Promise.reject(
      new Error(
        "A compilation is already running."
      )
    );
  }

  const requestId =
    nextRequestId++;

  return new Promise(
    (resolve, reject) => {
      const timeout =
        window.setTimeout(
          () => {
            if (
              !pendingCompilation ||
              pendingCompilation.requestId !==
                requestId
            ) {
              return;
            }

            pendingCompilation = null;

            compilerWorker.terminate();
            compilerWorker = null;
            compilerWorkerReady = false;

            reject(
              new Error(
                "Compilation exceeded the 3-second browser limit."
              )
            );

            createCompilerWorker();
          },
          compilationTimeoutMilliseconds
        );

      pendingCompilation = {
        requestId,
        resolve,
        reject,
        timeout
      };

            compilerWorker.postMessage({
        type: "compile",
        requestId,
        source: sourceCode,
        stdin: stdinText
      });
    }
  );
}

/* Run compilation */

async function runCode() {
  if (!compilerWorkerReady) {
    showToast(
      "The C engine is still loading."
    );

        return null;
  }

  const stdinBytes =
    updateStdinSize();

  if (
    stdinBytes >
    maximumStdinBytes
  ) {
    showToast(
      "Program input exceeds the 64 KB limit."
    );

    stdinEditor.focus();

    return null;
  }

  runButton.disabled = true;
  runButton.classList.add("running");

  compilationProgress.classList.add(
    "running"
  );

  setExecutionState(
    "running",
    "Compiling",
    0,
    0,
    null
  );

  await wait(120);

  try {
    const report =
            await compileWithWorker(
        codeEditor.value,
        stdinEditor.value
      );

    latestReport = report;
    renderReport(report);

    return report;
  } catch (error) {
    console.error(error);

    const failureReport = {
      ok: false,
      error:
        error instanceof Error
          ? error.message
          : "Compilation failed unexpectedly.",
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

    latestReport =
      failureReport;

    renderReport(
      failureReport
    );

    return failureReport;
  } finally {
    runButton.disabled = false;
    runButton.classList.remove(
      "running"
    );

    compilationProgress.classList.remove(
      "running"
    );
  }
}
/* Report rendering */

function renderReport(report) {
  const tokens =
    report.tokens || [];

  const nodes =
    report.nodes || [];

  const functions =
    report.functions || [];

  const instructions =
    report.instructions || [];

  renderOutput(report);

  renderTokens(tokens);

  renderSyntaxTree({
    nodes,
    functions
  });

  renderInstructions(
    instructions,
    nodes
  );

  tokenCount.textContent =
    tokens.length;

  instructionCount.textContent =
    instructions.length;

  if (report.stdout) {
    const outputLines =
      report.stdout
        .split("\n")
        .filter(Boolean)
        .length;

    outputCount.hidden = false;
    outputCount.textContent =
      outputLines;
  } else {
    outputCount.hidden = true;
  }

  if (report.ok) {
    compilerDiagnostic.hidden = true;
    sourceErrorMarker.hidden = true;

    updateLineNumbers();

    setExecutionState(
      "success",
      "Finished",
      instructions.length,
      report.steps || 0,
      report.result
    );
  } else {
    const errorLine =
      lineIndexAtOffset(
        report.errorStart || 0
      );

    currentErrorStart =
      report.errorStart || 0;

    currentErrorEnd =
      Math.max(
        report.errorEnd || 0,
        currentErrorStart + 1
      );

    updateLineNumbers(errorLine);

    sourceErrorMarker.hidden = false;

    sourceErrorMarker.style.top =
      `${19.2 + errorLine * 28}px`;

    showCompilerDiagnostic(
      `${report.error} ` +
      `(line ${errorLine + 1})`
    );

    setExecutionState(
      "error",
      "Stopped",
      instructions.length,
      report.steps || 0,
      null
    );

    openResultPanel("output");
  }
}

/* Program output */

function renderOutput(report) {
  programConsole.classList.toggle(
    "empty",
    !report.stdout
  );

  if (report.stdout) {
    programConsole.textContent =
      report.stdout;
  } else if (report.ok) {
    programConsole.textContent =
      "(program produced no output)";
  } else {
    programConsole.textContent =
      "Compilation stopped.";
  }
}

/* Compiler diagnostic */

function showCompilerDiagnostic(message) {
  compilerDiagnostic.hidden = false;

  diagnosticMessage.textContent =
    message;
}

/* Execution footer */

function setExecutionState(
  state,
  label,
  generatedInstructions,
  steps,
  exitValue
) {
  executionFooter.classList.remove(
    "success",
    "error",
    "running"
  );

  if (state) {
    executionFooter.classList.add(
      state
    );
  }

  executionStateText.textContent =
    label;

  executionInstructionStat.textContent =
    `${generatedInstructions} ` +
    `${
      generatedInstructions === 1
        ? "instruction"
        : "instructions"
    }`;

  executionStepStat.textContent =
    `${steps} ` +
    `${steps === 1 ? "step" : "steps"}`;

  executionExitStat.textContent =
    exitValue === null
      ? "exit —"
      : `exit ${exitValue}`;
}

/* Token rendering */

function renderTokens(tokens) {
  if (!tokens.length) {
    tokenGrid.innerHTML = `
      <p class="empty-view-message">
        No tokens were generated.
      </p>
    `;

    return;
  }

  const visibleTokens =
    tokens.slice(
      0,
      maximumRenderedTokens
    );

  const cards =
    visibleTokens
      .map(
        (token, index) => {
          return `
            <button
              class="token-card ${escapeHtml(token.kind)}"
              type="button"
              data-token-index="${index}"
              title="Select this token in the source"
            >
              <span class="token-card-value">
                ${escapeHtml(token.text)}
              </span>

              <span class="token-card-kind">
                ${escapeHtml(token.kind)}
              </span>
            </button>
          `;
        }
      )
      .join("");

  const remaining =
    tokens.length -
    visibleTokens.length;

  const limitMessage =
    remaining > 0
      ? `
        <p class="empty-view-message">
          ${remaining} additional tokens are hidden
          to keep the interface responsive.
        </p>
      `
      : "";

  tokenGrid.innerHTML =
    cards + limitMessage;
}

/* Syntax-tree rendering */

function renderSyntaxTree(report) {
  const nodes =
    report.nodes || [];

  const functions =
    report.functions || [];

  if (
    !nodes.length ||
    !functions.length
  ) {
    syntaxTree.innerHTML = `
      <p class="empty-view-message">
        The parser did not produce a syntax tree.
      </p>
    `;

    return;
  }

  const nodeById = new Map();

  for (const node of nodes) {
    nodeById.set(
      node.id,
      node
    );
  }

  let renderedNodeCount = 0;
  let treeWasLimited = false;

  function renderBranch(
    nodeId,
    depth = 0,
    path = new Set()
  ) {
    if (
      renderedNodeCount >=
      maximumRenderedTreeNodes
    ) {
      treeWasLimited = true;
      return "";
    }

    const node =
      nodeById.get(nodeId);

    if (
      !node ||
      depth > 80 ||
      path.has(nodeId)
    ) {
      return "";
    }

    renderedNodeCount++;

    const nextPath =
      new Set(path);

    nextPath.add(nodeId);

    const children =
      Array.isArray(node.children)
        ? node.children
        : [];

    const value =
      node.value !== 0
        ? `
          <span class="tree-node-value">
            ${escapeHtml(node.value)}
          </span>
        `
        : "";

    const childrenHtml =
      children
        .map(
          (childId) =>
            renderBranch(
              childId,
              depth + 1,
              nextPath
            )
        )
        .join("");

    return `
      <div class="tree-branch">
        <button
          class="tree-node-button"
          type="button"
          data-tree-token="${node.token}"
          title="Select this node in the source"
        >
          <span class="tree-node-kind">
            ${escapeHtml(node.kind)}
          </span>

          ${value}
        </button>

        ${childrenHtml}
      </div>
    `;
  }

  let treeContent = "";

  for (const compilerFunction of functions) {
    if (
      renderedNodeCount >=
      maximumRenderedTreeNodes
    ) {
      treeWasLimited = true;
      break;
    }

    const localLabel =
      compilerFunction.locals === 1
        ? "1 local"
        : `${compilerFunction.locals} locals`;

    const parameterLabel =
      compilerFunction.parameters === 1
        ? "1 parameter"
        : `${compilerFunction.parameters} parameters`;

    treeContent += `
      <section class="tree-function">
        <header class="tree-function-header">
          <strong>
            ${escapeHtml(compilerFunction.name)}()
          </strong>

          <span>
            ${parameterLabel} · ${localLabel}
          </span>
        </header>

        ${renderBranch(compilerFunction.root)}
      </section>
    `;
  }

  if (treeWasLimited) {
    treeContent += `
      <p class="empty-view-message">
        The remaining tree nodes are hidden
        to keep the interface responsive.
      </p>
    `;
  }

  syntaxTree.innerHTML =
    treeContent;
}

/* Instruction rendering */

function renderInstructions(
  instructions,
  nodes
) {
  if (!instructions.length) {
    instructionList.innerHTML = `
      <p class="empty-view-message">
        No instructions were generated.
      </p>
    `;

    return;
  }

  const nodeById = new Map();

  for (const node of nodes) {
    nodeById.set(
      node.id,
      node
    );
  }

  const visibleInstructions =
    instructions.slice(
      0,
      maximumRenderedInstructions
    );

  const rows =
    visibleInstructions
      .map(
        (instruction, index) => {
          const node =
            nodeById.get(
              instruction.node
            );

          const tokenIndex =
            node
              ? node.token
              : -1;

          let operand = "";

          if (
            instruction.arg !== 0 ||
            instruction.extra !== 0
          ) {
            operand =
              String(instruction.arg);

            if (
              instruction.extra !== 0
            ) {
              operand +=
                ` · ${instruction.extra}`;
            }
          }

          const address =
            String(
              instruction.index ?? index
            ).padStart(4, "0");

          return `
            <button
              class="instruction-row"
              type="button"
              data-instruction-token="${tokenIndex}"
              title="Select this instruction's source"
            >
              <span class="instruction-address">
                ${address}
              </span>

              <span class="instruction-operation">
                ${escapeHtml(instruction.op)}
              </span>

              <span class="instruction-argument">
                ${escapeHtml(operand)}
              </span>
            </button>
          `;
        }
      )
      .join("");

  const remaining =
    instructions.length -
    visibleInstructions.length;

  const limitMessage =
    remaining > 0
      ? `
        <p class="empty-view-message">
          ${remaining} additional instructions are hidden
          to keep the interface responsive.
        </p>
      `
      : "";

  instructionList.innerHTML =
    rows + limitMessage;
}

/* Result tabs */

function openResultPanel(panelName) {
  const tabs =
    document.querySelectorAll(
      ".result-tab"
    );

  const panels =
    document.querySelectorAll(
      ".compiler-panel"
    );

  tabs.forEach(
    (tab) => {
      const isActive =
        tab.dataset.panel ===
        panelName;

      tab.classList.toggle(
        "active",
        isActive
      );

      tab.setAttribute(
        "aria-selected",
        String(isActive)
      );
    }
  );

  panels.forEach(
    (panel) => {
      const isActive =
        panel.dataset.compilerPanel ===
        panelName;

      panel.hidden = !isActive;

      panel.classList.toggle(
        "active",
        isActive
      );
    }
  );
}
/* Contact the developer */

function openDeveloperContact() {
  const subject =
    encodeURIComponent(
      "Cinder Compiler"
    );

  const body =
    encodeURIComponent(
      "Hello,\n\n" +
      "I am contacting you about the Cinder compiler."
    );

  const gmailUrl =
    "https://mail.google.com/mail/" +
    "?view=cm" +
    "&fs=1" +
    `&to=${encodeURIComponent(developerEmail)}` +
    `&su=${subject}` +
    `&body=${body}`;

  const contactWindow =
    window.open(
      gmailUrl,
      "_blank",
      "noopener,noreferrer"
    );

  if (!contactWindow) {
    window.location.href =
      `mailto:${developerEmail}` +
      `?subject=${subject}` +
      `&body=${body}`;
  }
}

/* Copy program output */

async function copyProgramOutput() {
  const output =
    latestReport?.stdout || "";

  if (!output) {
    showToast(
      "There is no program output to copy."
    );

    return;
  }

  try {
    await navigator.clipboard.writeText(
      output
    );

    showToast(
      "Program output copied."
    );
  } catch {
    const temporaryInput =
      document.createElement("textarea");

    temporaryInput.value = output;

    temporaryInput.style.position =
      "fixed";

    temporaryInput.style.opacity =
      "0";

    document.body.appendChild(
      temporaryInput
    );

    temporaryInput.select();

    document.execCommand("copy");

    temporaryInput.remove();

    showToast(
      "Program output copied."
    );
  }
}

/* Editor controls */

function insertEditorText(text) {
  const start =
    codeEditor.selectionStart;

  const end =
    codeEditor.selectionEnd;

  codeEditor.setRangeText(
    text,
    start,
    end,
    "end"
  );

  updateLineNumbers();
  updateActiveLine();
}

function handleEditorKeydown(event) {
  if (event.key === "Tab") {
    event.preventDefault();

    insertEditorText("  ");
    return;
  }

  const isRunShortcut =
    (
      event.ctrlKey ||
      event.metaKey
    ) &&
    (
      event.key === "Enter" ||
      event.code === "Enter" ||
      event.code === "NumpadEnter"
    );

  if (isRunShortcut) {
    event.preventDefault();
    event.stopPropagation();

    showToast(
      "Running with Ctrl+Enter."
    );

    runCode();
  }
}

/* Editor events */

codeEditor.addEventListener(
  "input",
  () => {
    sourceErrorMarker.hidden = true;
    compilerDiagnostic.hidden = true;

    updateLineNumbers();
    updateActiveLine();
  }
);

codeEditor.addEventListener(
  "scroll",
  () => {
    lineNumbers.scrollTop =
      codeEditor.scrollTop;
  }
);

codeEditor.addEventListener(
  "click",
  updateActiveLine
);

codeEditor.addEventListener(
  "keyup",
  updateActiveLine
);

codeEditor.addEventListener(
  "keydown",
  handleEditorKeydown
);

/* Standard-input events */

stdinEditor.addEventListener(
  "input",
  updateStdinSize
);

stdinEditor.addEventListener(
  "keydown",
  (event) => {
    const isRunShortcut =
      (
        event.ctrlKey ||
        event.metaKey
      ) &&
      (
        event.key === "Enter" ||
        event.code === "Enter" ||
        event.code === "NumpadEnter"
      );

    if (!isRunShortcut) {
      return;
    }

    event.preventDefault();
    event.stopPropagation();

    showToast(
      "Running with Ctrl+Enter."
    );

    runCode();
  }
);

clearStdinButton.addEventListener(
  "click",
  () => {
    clearStandardInput(true);
  }
);

/* Workbench buttons */

runButton.addEventListener(
  "click",
  runCode
);

resetButton.addEventListener(
  "click",
  () => {
    loadExample(
      currentExample,
      true
    );

    showToast(
      "Example reset."
    );
  }
);

exampleSelect.addEventListener(
  "change",
  (event) => {
    loadExample(
      event.target.value,
      true
    );
  }
);

copyOutputButton.addEventListener(
  "click",
  copyProgramOutput
);

diagnosticSourceButton.addEventListener(
  "click",
  () => {
    selectSource(
      currentErrorStart,
      currentErrorEnd
    );
  }
);

contactDeveloperButton.addEventListener(
  "click",
  openDeveloperContact
);

footerContactButton.addEventListener(
  "click",
  openDeveloperContact
);

/* Result tabs */

document
  .querySelectorAll(".result-tab")
  .forEach(
    (tab) => {
      tab.addEventListener(
        "click",
        () => {
          openResultPanel(
            tab.dataset.panel
          );
        }
      );
    }
  );

/* Header navigation */

document
  .querySelectorAll("[data-open-panel]")
  .forEach(
    (link) => {
      link.addEventListener(
        "click",
        () => {
          openResultPanel(
            link.dataset.openPanel
          );
        }
      );
    }
  );

/* Token source navigation */

tokenGrid.addEventListener(
  "click",
  (event) => {
    const tokenCard =
      event.target.closest(
        "[data-token-index]"
      );

    if (!tokenCard) {
      return;
    }

    selectToken(
      Number(
        tokenCard.dataset.tokenIndex
      )
    );
  }
);

/* Syntax-tree source navigation */

syntaxTree.addEventListener(
  "click",
  (event) => {
    const treeNode =
      event.target.closest(
        "[data-tree-token]"
      );

    if (!treeNode) {
      return;
    }

    selectToken(
      Number(
        treeNode.dataset.treeToken
      )
    );
  }
);

/* Instruction source navigation */

instructionList.addEventListener(
  "click",
  (event) => {
    const instruction =
      event.target.closest(
        "[data-instruction-token]"
      );

    if (!instruction) {
      return;
    }

    const tokenIndex =
      Number(
        instruction.dataset
          .instructionToken
      );

    if (tokenIndex >= 0) {
      selectToken(tokenIndex);
    }
  }
);

/* Optional browser-agent integration */

function registerBrowserCompilerTool() {
  const context =
    document.modelContext;

  if (!context?.registerTool) {
    return;
  }

  try {
    Promise.resolve(
      context.registerTool({
        name:
          "compile_and_run_cinder_c",

        title:
          "Compile and run Cinder C",

        description:
          "Place C source in the visible Cinder editor, " +
          "compile it in a background worker, execute it, " +
          "and return its output or diagnostic.",

        inputSchema: {
          type: "object",

          properties: {
            source: {
              type: "string",

              description:
                "C source code containing int main()."
            }
          },

          required: [
            "source"
          ],

          additionalProperties: false
        },

        annotations: {
          readOnlyHint: false,
          untrustedContentHint: true
        },

        async execute(input) {
          if (
            !input ||
            typeof input.source !== "string" ||
            !input.source.trim()
          ) {
            throw new Error(
              "source must be a non-empty string"
            );
          }

          if (!compilerWorkerReady) {
            throw new Error(
              "The compiler engine is not ready."
            );
          }

          codeEditor.value =
            input.source;

          updateLineNumbers();
          updateActiveLine();

          const report =
            await runCode();

          if (report.ok) {
            return {
              ok: true,
              stdout: report.stdout,
              exitCode: report.result,
              steps: report.steps
            };
          }

          return {
            ok: false,
            error: report.error,
            errorStart:
              report.errorStart,
            errorEnd:
              report.errorEnd
          };
        }
      })
    ).catch(
      (error) => {
        console.warn(
          "Browser compiler tool could not be registered.",
          error
        );
      }
    );
  } catch (error) {
    console.warn(
      "Browser compiler tool could not be registered.",
      error
    );
  }
}

/* Clean up background work when the page closes. */

window.addEventListener(
  "beforeunload",
  () => {
    destroyCompilerWorker();
  }
);

/* Application startup */

function startApplication() {
  loadExample(
    "fibonacci",
    false
  );

  openResultPanel("output");

  setExecutionState(
    "",
    "Ready",
    0,
    0,
    null
  );

  registerBrowserCompilerTool();
  createCompilerWorker();
}

startApplication();