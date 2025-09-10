### ➡️ BFS vs Dijkstra Pathfinding Visualizer
**Description:**  
This project is an interactive web-based visualizer that demonstrates and compares two popular pathfinding algorithms: Breadth-First Search (BFS) and Dijkstra’s Algorithm.
It allows users to create mazes by placing walls, adjust start and end points, and then watch the algorithms explore the grid in real-time. The main goal is to provide an educational and visual tool to understand how different algorithms search for the shortest path in a grid environment.

**Highlights:**  
- **Interactive Grid Editing:** Place and erase walls by clicking and dragging. Move start (green) and end (red) positions dynamically.
- **Real-Time Visualization:** BFS and Dijkstra run side by side on separate grids. Animations show visited cells (light blue) and final path (yellow). Execution time displayed in milliseconds for each algorithm.  

![Pathfinder Screenshot 1](./BFSvsDijkstras/screenshots/Pathfinder1.png)

![Pathfinder Screenshot 2](./BFSvsDijkstras/screenshots/Pathfinder2.png)

## How to Download & Run  
1. Clone the repository.
2. Open Pathfinder.html in any modern browser (Chrome, Firefox, Edge).
3. No external libraries are required

## Controls
- Place Walls
  - Left-click and drag on the grid to draw walls (black).
  - Click again on a wall to erase it.
- Move Start & End Points
  - Drag the green square to reposition the start.
	- Drag the red square to reposition the end.
- Buttons
  - Start → Runs both BFS and Dijkstra simultaneously, animating their exploration.
	- Reset → Clears all walls, visited nodes, and paths.
- Visualization Colors
  - Green = Start node
	- Red = End node
  - Black = Wall
	- Light Blue = Visited cells
  - Yellow = Shortest path found
