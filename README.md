# Sorting Visualizer

A C++ application that visualizes various sorting algorithms using the **SFML** and **TGUI** libraries. This project demonstrates **Bubble Sort**, **Insertion Sort**, **Merge Sort**, **Quick Sort**, and **Heap Sort**, with interactive controls for speed, data size, pausing, stepping, and aborting the sorting process.

---

## Features

- **Real-Time Visualization**: Animated sorting with color-coded bars.
- **Sorting Algorithms**: 
  - Bubble Sort
  - Insertion Sort
  - Merge Sort
  - Quick Sort
  - Heap Sort
- **Interactive Controls**:
  - Adjust speed (0.5x to 3x)
  - Modify data size (10 to 200 bars)
  - Pause/Resume
  - Step through operations
  - Abort and reshuffle
- **Performance Metrics**:
  - Number of comparisons
  - Number of swaps
  - Elapsed time
- **Thread-Safe Execution**: Smooth GUI performance using multithreading.
- **FPS Counter**: Displays real-time frame rate.
- **Responsive UI**: Adjusts to window resizing dynamically.

---

## 🛠️ Prerequisites

- **C++ Compiler**: Supports C++11 or later (e.g., `g++`, MSVC)
- **SFML** (≥ v2.5)
- **TGUI** (≥ v0.9)
- **Arial Font** (`arial.ttf` in working directory for full UI support)

---

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/sorting-visualizer.git
cd sorting-visualizer
```

### 2. Install SFML

#### Ubuntu:

```bash
sudo apt-get install libsfml-dev
```

#### macOS (Homebrew):

```bash
brew install sfml
```

#### Windows:

Download from [SFML's official site](https://www.sfml-dev.org/) or use [vcpkg](https://github.com/microsoft/vcpkg).

### 3. Install TGUI

Follow instructions from the [TGUI website](https://tgui.eu/). Ensure it's built with the **same SFML version** as your project.

- Copy `TGUI/themes/Black.txt` to the project directory (or update the path in code).

### 4. Add Arial Font (Optional but Recommended)

Place `arial.ttf` in the working directory.

> If missing, the FPS and enrollment number won't display. A warning will appear in the console.

### 5. Build the Project

Using CMake:

```bash
mkdir build && cd build
cmake ..
make
```

Ensure you link the required libraries:

```bash
-lsfml-graphics -lsfml-window -lsfml-system -ltgui
```

---

## Usage

### Run the Application

```bash
./sorting_visualizer
```

### Controls

#### Buttons

- **Bubble Sort, Insertion Sort, Merge Sort, Quick Sort, Heap Sort**: Start sorting
- **Reshuffle**: Randomize data
- **Pause/Play**: Toggle pause (disabled in step mode)
- **Abort**: Stop and reshuffle
- **Step Mode**: Enable manual stepping
- **Step**: Perform one operation in step mode

#### 🎚️ Sliders

- **Speed**: 0.5x – 3x
- **Count**: Number of bars (10–200)

#### Labels

- **Status**: E.g., "Quick Sort Running", "Paused"
- **Comparisons / Swaps**: Operation counters
- **Time**: Sort duration
- **Speed**: Current speed multiplier

### 📐 Window

- Dynamic bar resizing on window change
- Bar colors follow a red-green-blue gradient
- Active comparisons/swaps shown in white

---

## Code Structure

- **main.cpp**: Core application logic

### Key Functions

- `drawBars()`: Render bars
- `shuffleData()`: Randomize data with Mersenne Twister
- `bubbleSortVisualized()`, etc.: Sorting algorithms with visual support
- `getGradientColor()`: Assign bar colors
- `pauseCheck()`: Manage pause/step logic

### Threading

- Sorting runs in a separate thread
- Uses `std::atomic`, `std::mutex`, `std::condition_variable`

---

## Dependencies

- **SFML**: Graphics, windowing, events
- **TGUI**: GUI elements (buttons, sliders)
- **C++ STL**: `vector`, `algorithm`, `thread`, `mutex`, etc.

---

## Notes

- Fixed 60 FPS for smooth visuals
- Displays enrollment number `202303103510046` (requires `arial.ttf`)
- Abort resets sorting with new random data
- Step mode enables manual control
- Speed slider uses non-linear scaling

---

## Contributing

Contributions are welcome!

1. Fork the repo
2. Create a feature branch  
   ```bash
   git checkout -b feature/your-feature
   ```
3. Commit your changes  
   ```bash
   git commit -m "Add your feature"
   ```
4. Push the branch  
   ```bash
   git push origin feature/your-feature
   ```
5. Open a pull request

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Acknowledgments

- Built with **SFML** and **TGUI**
- Inspired by visual algorithm tutorials and open-source projects
