# Sorting Visualizer

A C++ application that visualizes various sorting algorithms using the SFML and TGUI libraries. This project demonstrates Bubble Sort, Insertion Sort, Merge Sort, Quick Sort, and Heap Sort with interactive controls for speed, data size, pausing, stepping, and aborting the sorting process.

## Features
- **Visualizations**: Real-time graphical representation of sorting algorithms with highlighted bars for comparisons and swaps.
- **Algorithms**: Implements Bubble Sort, Insertion Sort, Merge Sort, Quick Sort, and Heap Sort.
- **Interactive Controls**:
  - Adjust sorting speed via a slider (0.5x to 3x).
  - Modify the number of bars (10 to 200).
  - Pause/resume sorting.
  - Step through sorting operations manually.
  - Abort and reshuffle data.
- **Metrics**: Displays comparisons, swaps, and elapsed time for each sort.
- **Thread-Safe**: Uses mutexes and condition variables for safe data access during visualization.
- **FPS Counter**: Shows real-time frame rate.
- **Responsive Design**: Window resizing support with dynamic bar width adjustment.

## Prerequisites
- **C++ Compiler**: Compatible with C++11 or later (e.g., g++, MSVC).
- **SFML**: Simple and Fast Multimedia Library (version 2.5 or later).
- **TGUI**: GUI library for SFML (version 0.9 or later).
- **Arial Font**: Requires `arial.ttf` in the working directory for FPS and enrollment number display (optional; displays warning if missing).

## Installation
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/sorting-visualizer.git
   cd sorting-visualizer


Install SFML:

Ubuntu:sudo apt-get install libsfml-dev


macOS (using Homebrew):brew install sfml


Windows: Download and install SFML from SFML's official website or use a package manager like vcpkg.


Install TGUI:

Follow the instructions on the TGUI website to build and install TGUI.
Ensure TGUI is linked with the same SFML version used in your project.
Copy the TGUI/themes/Black.txt theme file to the working directory or adjust the path in the code.


Arial Font (Optional):

Place arial.ttf in the project directory or modify the font path in the code.
If the font is missing, the FPS and enrollment number display will be disabled with a console warning.


Build the Project:

Use CMake or your preferred build system. Example with CMake:mkdir build && cd build
cmake ..
make


Ensure SFML and TGUI libraries are linked correctly. Example linker flags:-lsfml-graphics -lsfml-window -lsfml-system -ltgui





Usage

Run the Application:
./sorting_visualizer


Controls:

Buttons:
Bubble Sort, Insertion Sort, Merge Sort, Quick Sort, Heap Sort: Start the respective sorting algorithm.
Reshuffle: Randomize the data.
Pause/Play: Toggle pausing the sort (disabled in step mode).
Abort: Stop the current sort and reshuffle data.
Step Mode: Toggle step-by-step mode (pauses after each operation).
Step: Advance one step in step mode.


Sliders:
Speed: Adjust sorting speed (left for slower, right for faster).
Count: Change the number of bars (affects data size).


Labels:
Status: Shows current state (e.g., "Bubble Sort Running", "Paused").
Comparisons/Swaps: Displays the number of comparisons and swaps.
Time: Shows the elapsed time for the completed sort.
Speed: Displays the current speed multiplier (e.g., "Speed: 1.00x").




Window:

Resize the window to adjust the visualization.
Bars are colored using a gradient (red-green-blue) based on their values.
Highlighted bars (white) indicate elements being compared or swapped.



Code Structure

Main File: main.cpp contains the entire application logic.
Key Functions:
drawBars: Renders the bars with gradient colors and highlights.
shuffleData: Randomizes the data using Mersenne Twister.
bubbleSortVisualized, insertionSortVisualized, mergeSort, quickSort, heapSortVisualized: Implement sorting algorithms with visualization support.
getGradientColor: Generates colors for bars based on their values.
pauseCheck: Handles pausing and stepping logic.


Threading:
Sorting runs in a separate thread to keep the GUI responsive.
Uses std::atomic, std::mutex, and std::condition_variable for thread safety.



Dependencies

SFML: Handles windowing, graphics, and event processing.
TGUI: Provides GUI elements (buttons, sliders, labels).
C++ Standard Library: Uses <vector>, <algorithm>, <random>, <thread>, <mutex>, <atomic>, <condition_variable> for data management and concurrency.

Notes

The application uses a fixed frame rate of 60 FPS for smooth rendering.
The enrollment number "202303103510046" is displayed in the top-right corner (requires arial.ttf).
Sorting can be aborted at any time, resetting the data to a new random state.
Step mode allows manual control over each comparison/swap operation.
The speed slider uses a non-linear scale (0.5x to 3x) for finer control at slower speeds.

Contributing
Contributions are welcome! Please follow these steps:

Fork the repository.
Create a feature branch (git checkout -b feature/your-feature).
Commit your changes (git commit -m "Add your feature").
Push to the branch (git push origin feature/your-feature).
Open a pull request.

License
This project is licensed under the MIT License. See the LICENSE file for details.
Acknowledgments

Built with SFML and TGUI.
Inspired by sorting algorithm visualization tutorials and projects.
