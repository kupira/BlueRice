CXX = g++
CXXFLAGS = -std=c++17 -pthread -Iinclude

SRC = src/main.cpp src/BluetoothUtils.cpp src/SettingsUtils.cpp
OUT = bluerice

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

