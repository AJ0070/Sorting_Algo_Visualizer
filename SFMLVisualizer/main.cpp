#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <functional>
#include <string>
#include <cmath>
#include <atomic>
#include <mutex>
#include <condition_variable>

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 700;
int NUM_BARS = 100;
float BASE_DELAY = 0.01f;
float BAR_WIDTH = static_cast<float>(WINDOW_WIDTH) / NUM_BARS;
std::atomic<bool> isPaused(false);
std::atomic<bool> isAborted(false);
std::atomic<bool> stepMode(false);
std::mutex dataMutex;
std::condition_variable pauseCV;
std::condition_variable stepCV;

void pauseCheck() {
    if (stepMode) {
        std::unique_lock<std::mutex> lock(dataMutex);
        stepCV.wait(lock);
    }
    else {
        std::unique_lock<std::mutex> lock(dataMutex);
        pauseCV.wait(lock, [] { return !isPaused; });
    }
}

sf::Color getGradientColor(int value, int minVal, int maxVal) {
    if (minVal == maxVal) return sf::Color::White; // Avoid division by zero
    float t = static_cast<float>(value - minVal) / (maxVal - minVal);
    sf::Uint8 r = static_cast<sf::Uint8>(255 * t); // Red increases with value
    sf::Uint8 g = static_cast<sf::Uint8>(255 * (1 - t)); // Green decreases with value
    sf::Uint8 b = static_cast<sf::Uint8>(128 + 127 * std::sin(t * 3.14)); // Blue oscillates for variety
    return sf::Color(r, g, b);
}

void drawBars(sf::RenderWindow& window, const std::vector<int>& data, const std::vector<int>& highlightedIndices = {}) {
    window.clear(sf::Color::Black);
    int minVal = *std::min_element(data.begin(), data.end());
    int maxVal = *std::max_element(data.begin(), data.end());

    for (size_t i = 0; i < data.size(); ++i) {
        sf::RectangleShape bar;
        float width = std::max(1.0f, BAR_WIDTH - 1.0f);
        bar.setSize(sf::Vector2f(width, static_cast<float>(data[i])));
        bar.setPosition(i * BAR_WIDTH, static_cast<float>(WINDOW_HEIGHT - data[i]));

        bool isHighlighted = std::find(highlightedIndices.begin(), highlightedIndices.end(), static_cast<int>(i)) != highlightedIndices.end();
        bar.setFillColor(isHighlighted ? sf::Color::White : getGradientColor(data[i], minVal, maxVal));

        window.draw(bar);
    }
}

// Used Mersenne Twister engine(mt19937) for randomization (pseudorandom number generator (PRNG))
void shuffleData(std::vector<int>& data) {
    std::random_device rd;
    std::mt19937 g(rd());
    for (size_t i = 0; i < data.size(); ++i)
        data[i] = 50 + std::uniform_int_distribution<>(0, static_cast<int>(WINDOW_HEIGHT - 150))(g);
    std::shuffle(data.begin(), data.end(), g);
}

void bubbleSortVisualized(std::vector<int>& data, float delay, std::vector<int>& highlightedIndices,
    std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps, std::atomic<bool>& sorting) {
    size_t n = data.size();

    for (size_t i = 0; i < n - 1 && !isAborted; ++i) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1 && !isAborted; ++j) {
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                highlightedIndices = { static_cast<int>(j), static_cast<int>(j + 1) };
            }
            comparisons++;
            pauseCheck();
            std::this_thread::sleep_for(std::chrono::duration<float>(delay));

            if (data[j] > data[j + 1]) {
                std::lock_guard<std::mutex> lock(dataMutex);
                std::swap(data[j], data[j + 1]);
                swaps++;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    if (!isAborted) {
        std::lock_guard<std::mutex> lock(dataMutex);
        sorting = false;
        highlightedIndices.clear();
    }
}

void insertionSortVisualized(std::vector<int>& data, float delay, std::vector<int>& highlightedIndices,
    std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps, std::atomic<bool>& sorting) {
    size_t n = data.size();

    for (size_t i = 1; i < n && !isAborted; ++i) {
        int key = data[i];
        int j = static_cast<int>(i) - 1;

        while (j >= 0 && data[j] > key && !isAborted) {
            comparisons++;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                data[j + 1] = data[j];
                highlightedIndices = { j, j + 1 };
            }
            pauseCheck();
            swaps++;
            std::this_thread::sleep_for(std::chrono::duration<float>(delay));
            j--;
        }
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            data[j + 1] = key;
            highlightedIndices.clear();
        }
    }
    if (!isAborted) {
        std::lock_guard<std::mutex> lock(dataMutex);
        sorting = false;
        highlightedIndices.clear();
    }
}

void merge(std::vector<int>& data, int left, int mid, int right, float delay,
    std::vector<int>& highlightedIndices, std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps) {
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right && !isAborted) {
        comparisons++;
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            highlightedIndices = { i, j };
        }
        pauseCheck();
        std::this_thread::sleep_for(std::chrono::duration<float>(delay));
        if (data[i] <= data[j]) {
            temp[k++] = data[i++];
        }
        else {
            temp[k++] = data[j++];
            swaps++;
        }
    }
    while (i <= mid && !isAborted) temp[k++] = data[i++];
    while (j <= right && !isAborted) temp[k++] = data[j++];

    for (i = left, k = 0; i <= right && !isAborted; ++i, ++k) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            data[i] = temp[k];
            highlightedIndices = { i };
        }
        pauseCheck();
        std::this_thread::sleep_for(std::chrono::duration<float>(delay));
    }
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        highlightedIndices.clear();
    }
}

void mergeSort(std::vector<int>& data, int left, int right, float delay,
    std::vector<int>& highlightedIndices, std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps,
    std::atomic<bool>& sorting) {
    if (left < right && !isAborted) {
        int mid = left + (right - left) / 2;
        mergeSort(data, left, mid, delay, highlightedIndices, comparisons, swaps, sorting);
        mergeSort(data, mid + 1, right, delay, highlightedIndices, comparisons, swaps, sorting);
        merge(data, left, mid, right, delay, highlightedIndices, comparisons, swaps);
    }
    if (!isAborted && left == 0 && right == static_cast<int>(data.size()) - 1) {
        std::lock_guard<std::mutex> lock(dataMutex);
        sorting = false;
        highlightedIndices.clear();
    }
}

int partition(std::vector<int>& data, int low, int high, float delay,
    std::vector<int>& highlightedIndices, std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps) {
    int pivot = data[high];
    int i = low - 1;

    for (int j = low; j < high && !isAborted; ++j) {
        comparisons++;
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            highlightedIndices = { j, high };
        }
        pauseCheck();
        std::this_thread::sleep_for(std::chrono::duration<float>(delay));

        if (data[j] < pivot) {
            i++;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                std::swap(data[i], data[j]);
                swaps++;
                highlightedIndices = { i, j };
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        std::swap(data[i + 1], data[high]);
        swaps++;
        highlightedIndices = { i + 1, high };
    }
    return i + 1;
}

void quickSort(std::vector<int>& data, int low, int high, float delay,
    std::vector<int>& highlightedIndices, std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps,
    std::atomic<bool>& sorting) {
    if (low < high && !isAborted) {
        int pi = partition(data, low, high, delay, highlightedIndices, comparisons, swaps);
        quickSort(data, low, pi - 1, delay, highlightedIndices, comparisons, swaps, sorting);
        quickSort(data, pi + 1, high, delay, highlightedIndices, comparisons, swaps, sorting);
    }
    if (!isAborted && low == 0 && high == static_cast<int>(data.size()) - 1) {
        std::lock_guard<std::mutex> lock(dataMutex);
        sorting = false;
        highlightedIndices.clear();
    }
}

void heapify(std::vector<int>& data, int n, int i, float delay,
    std::vector<int>& highlightedIndices, std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n) comparisons++;
    if (l < n && data[l] > data[largest]) largest = l;

    if (r < n) comparisons++;
    if (r < n && data[r] > data[largest]) largest = r;

    if (largest != i && !isAborted) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            std::swap(data[i], data[largest]);
            swaps++;
            highlightedIndices = { i, largest };
        }
        pauseCheck();
        std::this_thread::sleep_for(std::chrono::duration<float>(delay));
        heapify(data, n, largest, delay, highlightedIndices, comparisons, swaps);
    }
}

void heapSortVisualized(std::vector<int>& data, float delay, std::vector<int>& highlightedIndices,
    std::atomic<size_t>& comparisons, std::atomic<size_t>& swaps, std::atomic<bool>& sorting) {
    int n = static_cast<int>(data.size());
    for (int i = n / 2 - 1; i >= 0 && !isAborted; --i)
        heapify(data, n, i, delay, highlightedIndices, comparisons, swaps);
    for (int i = n - 1; i > 0 && !isAborted; --i) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            std::swap(data[0], data[i]);
            swaps++;
            highlightedIndices = { 0, i };
        }
        pauseCheck();
        std::this_thread::sleep_for(std::chrono::duration<float>(delay));
        heapify(data, i, 0, delay, highlightedIndices, comparisons, swaps);
    }
    if (!isAborted) {
        std::lock_guard<std::mutex> lock(dataMutex);
        sorting = false;
        highlightedIndices.clear();
    }
}

int main() {
    try {
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sorting Visualizer (TGUI)", sf::Style::Resize | sf::Style::Close);
        window.setFramerateLimit(60);
        tgui::Gui gui(window);
        tgui::Theme::setDefault("TGUI/themes/Black.txt");

        // GUI Elements (unchanged)
        auto bubbleBtn = tgui::Button::create("Bubble Sort");
        auto insertBtn = tgui::Button::create("Insertion Sort");
        auto mergeBtn = tgui::Button::create("Merge Sort");
        auto quickBtn = tgui::Button::create("Quick Sort");
        auto heapBtn = tgui::Button::create("Heap Sort");
        auto shuffleBtn = tgui::Button::create("Reshuffle");
        auto pauseBtn = tgui::Button::create("Pause");
        auto abortBtn = tgui::Button::create("Abort");
        auto stepBtn = tgui::Button::create("Step Mode");
        auto stepNextBtn = tgui::Button::create("Step");

        bubbleBtn->setPosition(20, 20);
        insertBtn->setPosition(150, 20);
        mergeBtn->setPosition(280, 20);
        quickBtn->setPosition(410, 20);
        heapBtn->setPosition(540, 20);
        shuffleBtn->setPosition(670, 20);
        pauseBtn->setPosition(20, 70);
        abortBtn->setPosition(150, 70);
        stepBtn->setPosition(280, 70);
        stepNextBtn->setPosition(410, 70);

        bubbleBtn->setSize({ 120, 40 });
        insertBtn->setSize({ 120, 40 });
        mergeBtn->setSize({ 120, 40 });
        quickBtn->setSize({ 120, 40 });
        heapBtn->setSize({ 120, 40 });
        shuffleBtn->setSize({ 120, 40 });
        pauseBtn->setSize({ 120, 40 });
        abortBtn->setSize({ 120, 40 });
        stepBtn->setSize({ 120, 40 });
        stepNextBtn->setSize({ 120, 40 });

        auto speedSlider = tgui::Slider::create(0, 100);
        speedSlider->setValue(50);
        speedSlider->setPosition(670, 70);
        speedSlider->setSize({ 120, 20 });

        auto speedLabel = tgui::Label::create("Speed: 1.00x");
        speedLabel->setPosition(670, 120);
        speedLabel->setTextSize(16);

        auto countSlider = tgui::Slider::create(10, 200);
        countSlider->setValue(NUM_BARS);
        countSlider->setPosition(670, 100);
        countSlider->setSize({ 120, 20 });

        auto statusLabel = tgui::Label::create("Idle");
        statusLabel->setPosition(20, 120);
        statusLabel->setTextSize(16);

        auto comparisonLabel = tgui::Label::create("Comparisons: 0");
        comparisonLabel->setPosition(20, 150);
        comparisonLabel->setTextSize(16);

        auto swapLabel = tgui::Label::create("Swaps: 0");
        swapLabel->setPosition(20, 180);
        swapLabel->setTextSize(16);

        auto timeLabel = tgui::Label::create("Time: 0.0s");
        timeLabel->setPosition(20, 210);
        timeLabel->setTextSize(16);

        gui.add(bubbleBtn);
        gui.add(insertBtn);
        gui.add(mergeBtn);
        gui.add(quickBtn);
        gui.add(heapBtn);
        gui.add(shuffleBtn);
        gui.add(pauseBtn);
        gui.add(abortBtn);
        gui.add(stepBtn);
        gui.add(stepNextBtn);
        gui.add(speedSlider);
        gui.add(speedLabel);
        gui.add(countSlider);
        gui.add(statusLabel);
        gui.add(comparisonLabel);
        gui.add(swapLabel);
        gui.add(timeLabel);

        // Font for FPS and Enrollment Number
        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");
        sf::Text fpsText("", font, 16);
        fpsText.setFillColor(sf::Color::Green);
        fpsText.setPosition(10, WINDOW_HEIGHT - 25);
        if (!fontLoaded) {
            std::cerr << "Failed to load arial.ttf, FPS and enrollment display disabled" << std::endl;
        }

        // Enrollment Number Text
        sf::Text enrollmentText;
        if (fontLoaded) {
            enrollmentText.setFont(font);
            enrollmentText.setString("202303103510046");
            enrollmentText.setCharacterSize(16);
            enrollmentText.setFillColor(sf::Color::White);
            // Position in top right corner, adjust based on text bounds
            sf::FloatRect textBounds = enrollmentText.getLocalBounds();
            enrollmentText.setPosition(WINDOW_WIDTH - textBounds.width - 10, 10);
        }

        // Data and State (unchanged)
        std::vector<int> data(NUM_BARS);
        std::vector<int> renderData(NUM_BARS);
        std::vector<int> highlightedIndices;
        shuffleData(data);
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            renderData = data;
        }
        std::atomic<bool> sorting(false);
        std::atomic<size_t> comparisons(0);
        std::atomic<size_t> swaps(0);
        std::function<void()> sortFunc = nullptr;
        std::thread sortThread;
        sf::Clock sortClock;

        // Helper Functions (unchanged)
        auto getSpeedMultiplier = [&] {
            float sliderValue = speedSlider->getValue();
            float t = sliderValue / 100.0f; // [0, 1]
            float multiplier;
            if (t < 0.5f) {
                multiplier = 0.5f + (t / 0.5f) * (1.0f - 0.5f);
            }
            else {
                multiplier = 1.0f + ((t - 0.5f) / 0.5f) * (3.0f - 1.0f);
            }
            return multiplier;
            };

        auto enableButtons = [&] {
            bubbleBtn->setEnabled(true);
            insertBtn->setEnabled(true);
            mergeBtn->setEnabled(true);
            quickBtn->setEnabled(true);
            heapBtn->setEnabled(true);
            shuffleBtn->setEnabled(true);
            pauseBtn->setEnabled(!stepMode);
            abortBtn->setEnabled(true);
            stepBtn->setEnabled(true);
            stepNextBtn->setEnabled(stepMode && sorting);
            };

        auto disableButtons = [&] {
            bubbleBtn->setEnabled(false);
            insertBtn->setEnabled(false);
            mergeBtn->setEnabled(false);
            quickBtn->setEnabled(false);
            heapBtn->setEnabled(false);
            shuffleBtn->setEnabled(false);
            pauseBtn->setEnabled(!stepMode);
            abortBtn->setEnabled(true);
            stepBtn->setEnabled(false);
            stepNextBtn->setEnabled(stepMode && sorting);
            };

        auto resetCounters = [&] {
            comparisons = 0;
            swaps = 0;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                highlightedIndices.clear();
            }
            comparisonLabel->setText("Comparisons: 0");
            swapLabel->setText("Swaps: 0");
            timeLabel->setText("Time: 0.0s");
            };

        auto regenerate = [&] {
            NUM_BARS = static_cast<int>(countSlider->getValue());
            BAR_WIDTH = static_cast<float>(WINDOW_WIDTH) / NUM_BARS;
            data.resize(NUM_BARS);
            renderData.resize(NUM_BARS);
            shuffleData(data);
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                renderData = data;
                highlightedIndices.clear();
            }
            };

        // Button Handlers (unchanged)
        bubbleBtn->onPress([&] {
            if (!sorting) {
                sorting = true;
                isAborted = false;
                resetCounters();
                statusLabel->setText("Bubble Sort Running");
                sortClock.restart();
                disableButtons();
                sortFunc = [&] {
                    bubbleSortVisualized(data, BASE_DELAY / getSpeedMultiplier(), highlightedIndices,
                        comparisons, swaps, sorting);
                    if (!isAborted) {
                        statusLabel->setText("Bubble Sort Complete");
                        timeLabel->setText("Time: " + std::to_string(sortClock.getElapsedTime().asSeconds()) + "s");
                        {
                            std::lock_guard<std::mutex> lock(dataMutex);
                            renderData = data;
                        }
                    }
                    enableButtons();
                    };
                sortThread = std::thread(sortFunc);
                sortThread.detach();
            }
            });

        insertBtn->onPress([&] {
            if (!sorting) {
                sorting = true;
                isAborted = false;
                resetCounters();
                statusLabel->setText("Insertion Sort Running");
                sortClock.restart();
                disableButtons();
                sortFunc = [&] {
                    insertionSortVisualized(data, BASE_DELAY / getSpeedMultiplier(), highlightedIndices,
                        comparisons, swaps, sorting);
                    if (!isAborted) {
                        statusLabel->setText("Insertion Sort Complete");
                        timeLabel->setText("Time: " + std::to_string(sortClock.getElapsedTime().asSeconds()) + "s");
                        {
                            std::lock_guard<std::mutex> lock(dataMutex);
                            renderData = data;
                        }
                    }
                    enableButtons();
                    };
                sortThread = std::thread(sortFunc);
                sortThread.detach();
            }
            });

        mergeBtn->onPress([&] {
            if (!sorting) {
                sorting = true;
                isAborted = false;
                resetCounters();
                statusLabel->setText("Merge Sort Running");
                sortClock.restart();
                disableButtons();
                sortFunc = [&] {
                    mergeSort(data, 0, static_cast<int>(data.size()) - 1, BASE_DELAY / getSpeedMultiplier(),
                        highlightedIndices, comparisons, swaps, sorting);
                    if (!isAborted) {
                        statusLabel->setText("Merge Sort Complete");
                        timeLabel->setText("Time: " + std::to_string(sortClock.getElapsedTime().asSeconds()) + "s");
                        {
                            std::lock_guard<std::mutex> lock(dataMutex);
                            renderData = data;
                        }
                    }
                    enableButtons();
                    };
                sortThread = std::thread(sortFunc);
                sortThread.detach();
            }
            });

        quickBtn->onPress([&] {
            if (!sorting) {
                sorting = true;
                isAborted = false;
                resetCounters();
                statusLabel->setText("Quick Sort Running");
                sortClock.restart();
                disableButtons();
                sortFunc = [&] {
                    quickSort(data, 0, static_cast<int>(data.size()) - 1, BASE_DELAY / getSpeedMultiplier(),
                        highlightedIndices, comparisons, swaps, sorting);
                    if (!isAborted) {
                        statusLabel->setText("Quick Sort Complete");
                        timeLabel->setText("Time: " + std::to_string(sortClock.getElapsedTime().asSeconds()) + "s");
                        {
                            std::lock_guard<std::mutex> lock(dataMutex);
                            renderData = data;
                        }
                    }
                    enableButtons();
                    };
                sortThread = std::thread(sortFunc);
                sortThread.detach();
            }
            });

        heapBtn->onPress([&] {
            if (!sorting) {
                sorting = true;
                isAborted = false;
                resetCounters();
                statusLabel->setText("Heap Sort Running");
                sortClock.restart();
                disableButtons();
                sortFunc = [&] {
                    heapSortVisualized(data, BASE_DELAY / getSpeedMultiplier(), highlightedIndices,
                        comparisons, swaps, sorting);
                    if (!isAborted) {
                        statusLabel->setText("Heap Sort Complete");
                        timeLabel->setText("Time: " + std::to_string(sortClock.getElapsedTime().asSeconds()) + "s");
                        {
                            std::lock_guard<std::mutex> lock(dataMutex);
                            renderData = data;
                        }
                    }
                    enableButtons();
                    };
                sortThread = std::thread(sortFunc);
                sortThread.detach();
            }
            });

        shuffleBtn->onPress([&] {
            if (!sorting) {
                regenerate();
                statusLabel->setText("Idle");
                resetCounters();
            }
            });

        pauseBtn->onPress([&] {
            if (!stepMode) {
                isPaused = !isPaused;
                pauseBtn->setText(isPaused ? "Play" : "Pause");
                statusLabel->setText(isPaused ? "Paused" : (sorting ? statusLabel->getText() : "Idle"));
                if (!isPaused) pauseCV.notify_all();
            }
            });

        abortBtn->onPress([&] {
            if (sorting) {
                isAborted = true;
                isPaused = false;
                pauseCV.notify_all();
                stepCV.notify_all();
                sorting = false;
                regenerate();
                statusLabel->setText("Sort Aborted");
                resetCounters();
                enableButtons();
            }
            });

        stepBtn->onPress([&] {
            stepMode = !stepMode;
            stepBtn->setText(stepMode ? "Auto Mode" : "Step Mode");
            pauseBtn->setEnabled(!stepMode);
            stepNextBtn->setEnabled(stepMode && sorting);
            if (!stepMode && isPaused) {
                isPaused = false;
                pauseCV.notify_all();
            }
            statusLabel->setText(stepMode ? "Step Mode" : (sorting ? statusLabel->getText() : "Idle"));
            });

        stepNextBtn->onPress([&] {
            if (stepMode && sorting) {
                stepCV.notify_one();
            }
            });

        countSlider->onValueChange([&](float) {
            if (!sorting) {
                regenerate();
                statusLabel->setText("Idle");
            }
            });

        // Main Loop
        sf::Clock fpsClock;
        int frames = 0;
        sf::Clock redrawClock;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                gui.handleEvent(event);
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized) {
                    WINDOW_WIDTH = event.size.width;
                    WINDOW_HEIGHT = event.size.height;
                    BAR_WIDTH = static_cast<float>(WINDOW_WIDTH) / NUM_BARS;
                    window.setView(sf::View(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)));
                    fpsText.setPosition(10, WINDOW_HEIGHT - 25);
                    // Update enrollment text position
                    if (fontLoaded) {
                        sf::FloatRect textBounds = enrollmentText.getLocalBounds();
                        enrollmentText.setPosition(WINDOW_WIDTH - textBounds.width - 10, 10);
                    }
                }
            }

            // Update renderData and highlightedIndices
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                renderData = data;
            }

            // Render at controlled rate
            if (redrawClock.getElapsedTime().asSeconds() >= 1.0f / 60.0f) {
                drawBars(window, renderData, highlightedIndices);
                gui.draw();
                if (fontLoaded) {
                    window.draw(fpsText);
                    window.draw(enrollmentText); // Draw enrollment number
                }
                window.display();
                redrawClock.restart();
            }

            // Update GUI elements
            comparisonLabel->setText("Comparisons: " + std::to_string(comparisons));
            swapLabel->setText("Swaps: " + std::to_string(swaps));
            speedLabel->setText("Speed: " + std::to_string(getSpeedMultiplier()).substr(0, 4) + "x");

            // FPS Counter
            frames++;
            if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
                if (fontLoaded) {
                    fpsText.setString("FPS: " + std::to_string(frames));
                }
                frames = 0;
                fpsClock.restart();
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::cin.get();
    }
    return 0;
}