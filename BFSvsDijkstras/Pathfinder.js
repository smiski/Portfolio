const ROWS = 20;
const COLS = 30;
const TILE_SIZE = 25;

let bGrid = [];
let dGrid = [];
let start = { row: ROWS - 1, col: 0 };
let end = { row: 0, col: COLS - 1 };

let isMouseDown = false;
let placingStart = false;
let placingEnd = false;
let isAnimating = false;

let bfsInterval = null;
let dijkstraInterval = null;

const bCanvas = document.getElementById("bfsCanvas");
const dCanvas = document.getElementById("dijkstraCanvas");
const bCtx = bCanvas.getContext("2d");
const dCtx = dCanvas.getContext("2d");

bCanvas.width = dCanvas.width = COLS * TILE_SIZE;
bCanvas.height = dCanvas.height = ROWS * TILE_SIZE;

function makeGrid() {
  return Array.from({ length: ROWS }, () =>
    Array.from({ length: COLS }, () => ({
      wall: false,
      visited: false,
      parent: null,
      distance: Infinity,
      path: false,
    }))
  );
}

function drawGrid(ctx, grid, showPath = true) {
  ctx.clearRect(0, 0, COLS * TILE_SIZE, ROWS * TILE_SIZE);
  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      const cell = grid[r][c];
      if (cell.wall) {
        ctx.fillStyle = "black";
      } else if (cell.path && showPath) {
        ctx.fillStyle = "yellow";
      } else if (cell.visited) {
        ctx.fillStyle = "lightblue";
      } else {
        ctx.fillStyle = "white";
      }
      ctx.fillRect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      ctx.strokeRect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }
  }
  // Draw start and end
  ctx.fillStyle = "green";
  ctx.fillRect(start.col * TILE_SIZE, start.row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
  ctx.fillStyle = "red";
  ctx.fillRect(end.col * TILE_SIZE, end.row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
}

function neighbors(r, c) {
  const result = [];
  [[-1, 0],[1, 0],[0, -1],[0, 1]].forEach(([dr, dc]) => {
    const nr = r + dr, nc = c + dc;
    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
      result.push([nr, nc]);
    }
  });
  return result;
}

function resetGrid(grid) {
  for (let row of grid) {
    for (let cell of row) {
      cell.visited = false;
      cell.parent = null;
      cell.path = false;
      cell.distance = Infinity;
    }
  }
}

function reconstructPath(grid) {
  let [r, c] = [end.row, end.col];
  while (grid[r][c].parent) {
    [r, c] = grid[r][c].parent;
    grid[r][c].path = true;
  }
}

function bfs() {
  resetGrid(bGrid);
  const queue = [[start.row, start.col]];
  bGrid[start.row][start.col].visited = true;

  const startTime = performance.now();
  const timeDiv = document.getElementById("bfsTime");

  clearInterval(bfsInterval);
  bfsInterval = setInterval(() => {
    const now = performance.now();
    timeDiv.textContent = `${(now - startTime).toFixed(2)} ms`;
  }, 30);

  function step() {
    if (queue.length === 0) {
      reconstructPath(bGrid);
      drawGrid(bCtx, bGrid);
      clearInterval(bfsInterval);
      const endTime = performance.now();
      timeDiv.textContent = `${(endTime - startTime).toFixed(2)} ms`;
      isAnimating = false;
      return;
    }

    const [r, c] = queue.shift();

    if (r === end.row && c === end.col) {
      reconstructPath(bGrid);
      drawGrid(bCtx, bGrid);
      clearInterval(bfsInterval);
      const endTime = performance.now();
      timeDiv.textContent = `${(endTime - startTime).toFixed(2)} ms`;
      isAnimating = false;
      return;
    }

    for (const [nr, nc] of neighbors(r, c)) {
      const cell = bGrid[nr][nc];
      if (!cell.visited && !cell.wall) {
        cell.visited = true;
        cell.parent = [r, c];
        queue.push([nr, nc]);
      }
    }

    drawGrid(bCtx, bGrid, false);
    requestAnimationFrame(step);
  }

  step();
}

class MinHeap {
  constructor() {
    this.heap = [];
  }

  push(node) {
    this.heap.push(node);
    this.bubbleUp();
  }

  pop() {
    const min = this.heap[0];
    const end = this.heap.pop();
    if (this.heap.length > 0) {
      this.heap[0] = end;
      this.sinkDown();
    }
    return min;
  }

  isEmpty() {
    return this.heap.length === 0;
  }

  bubbleUp() {
    let idx = this.heap.length - 1;
    const el = this.heap[idx];
    while (idx > 0) {
      let parentIdx = Math.floor((idx - 1) / 2);
      let parent = this.heap[parentIdx];
      if (el.dist >= parent.dist) break;
      this.heap[idx] = parent;
      this.heap[parentIdx] = el;
      idx = parentIdx;
    }
  }

  sinkDown() {
    let idx = 0;
    const length = this.heap.length;
    const el = this.heap[0];
    while (true) {
      let leftIdx = 2 * idx + 1;
      let rightIdx = 2 * idx + 2;
      let swap = null;
      if (leftIdx < length) {
        if (this.heap[leftIdx].dist < el.dist) {
          swap = leftIdx;
        }
      }
      if (rightIdx < length) {
        if (
          (swap === null && this.heap[rightIdx].dist < el.dist) ||
          (swap !== null && this.heap[rightIdx].dist < this.heap[leftIdx].dist)
        ) {
          swap = rightIdx;
        }
      }
      if (swap === null) break;
      this.heap[idx] = this.heap[swap];
      this.heap[swap] = el;
      idx = swap;
    }
  }
}

function animateDijkstra() {
  resetGrid(dGrid);
  const heap = new MinHeap();

  dGrid[start.row][start.col].distance = 0;
  heap.push({ row: start.row, col: start.col, dist: 0 });

  const startTime = performance.now();
  const timeDiv = document.getElementById("dijkstraTime");

  clearInterval(dijkstraInterval);
  dijkstraInterval = setInterval(() => {
    const now = performance.now();
    timeDiv.textContent = `${(now - startTime).toFixed(2)} ms`;
  }, 30);

  function step() {
    if (heap.isEmpty()) {
      reconstructPath(dGrid);
      drawGrid(dCtx, dGrid);
      clearInterval(dijkstraInterval);
      const endTime = performance.now();
      timeDiv.textContent = `${(endTime - startTime).toFixed(2)} ms`;
      isAnimating = false;
      return;
    }

    const { row: r, col: c } = heap.pop();
    const cell = dGrid[r][c];

    if (cell.visited) {
      return requestAnimationFrame(step);
    }
    cell.visited = true;

    if (r === end.row && c === end.col) {
      reconstructPath(dGrid);
      drawGrid(dCtx, dGrid);
      clearInterval(dijkstraInterval);
      const endTime = performance.now();
      timeDiv.textContent = `${(endTime - startTime).toFixed(2)} ms`;
      isAnimating = false;
      return;
    }

    for (const [nr, nc] of neighbors(r, c)) {
      const neighbor = dGrid[nr][nc];
      if (!neighbor.wall) {
        const newDist = cell.distance + 1;
        if (newDist < neighbor.distance) {
          neighbor.distance = newDist;
          neighbor.parent = [r, c];
          heap.push({ row: nr, col: nc, dist: newDist });
        }
      }
    }

    drawGrid(dCtx, dGrid, false);
    requestAnimationFrame(step);
  }

  step();
}

function getCell(event, canvas) {
  const rect = canvas.getBoundingClientRect();
  const x = event.clientX - rect.left;
  const y = event.clientY - rect.top;
  return {
    row: Math.floor(y / TILE_SIZE),
    col: Math.floor(x / TILE_SIZE),
  };
}

function handleMouseDown(e) {
  if (isAnimating) return;
  isMouseDown = true;
  const { row, col } = getCell(e, bCanvas);
  if (row === start.row && col === start.col) {
    placingStart = true;
  } else if (row === end.row && col === end.col) {
    placingEnd = true;
  } else {
    if (bGrid[row][col].wall) {
      eraseWall(row, col);
    } else {
      drawWall(row, col);
    }
  }
}

function handleMouseMove(e) {
  if (!isMouseDown || isAnimating) return;
  const { row, col } = getCell(e, bCanvas);
  if (placingStart && !bGrid[row][col].wall && !(row === end.row && col === end.col)) {
    start = { row, col };
    redrawAll();
  } else if (placingEnd && !bGrid[row][col].wall && !(row === start.row && col === start.col)) {
    end = { row, col };
    redrawAll();
  } else if (!placingStart && !placingEnd) {
    if (!bGrid[row][col].wall && !(row === start.row && col === start.col) && !(row === end.row && col === end.col)) {
      drawWall(row, col);
    }
  }
}

function handleMouseUp() {
  isMouseDown = placingStart = placingEnd = false;
}

function drawWall(r, c) {
  if ((r === start.row && c === start.col) || (r === end.row && c === end.col)) return;
  bGrid[r][c].wall = true;
  dGrid[r][c].wall = true; // mirror to dijkstra grid
  redrawAll();
}

function eraseWall(r, c) {
  bGrid[r][c].wall = false;
  dGrid[r][c].wall = false; // mirror to dijkstra grid
  redrawAll();
}

function redrawAll() {
  drawGrid(bCtx, bGrid);
  drawGrid(dCtx, dGrid);
}

// Button actions
document.getElementById("startBtn").onclick = () => {
  if (isAnimating) return;
  isAnimating = true;
  bfs();
  animateDijkstra();
};

document.getElementById("resetBtn").onclick = () => {
  if (isAnimating) return;
  clearInterval(bfsInterval);
  clearInterval(dijkstraInterval);
  document.getElementById("bfsTime").textContent = "0.00 ms";
  document.getElementById("dijkstraTime").textContent = "0.00 ms";
  bGrid = makeGrid();
  dGrid = makeGrid();
  redrawAll();
};


// Setup initial grids and draw
bGrid = makeGrid();
dGrid = makeGrid();
redrawAll();

bCanvas.addEventListener("mousedown", handleMouseDown);
bCanvas.addEventListener("mousemove", handleMouseMove);
window.addEventListener("mouseup", handleMouseUp);