# YADA (Yet Another Diet Assistant)

YADA is a command-line diet management system that helps users track their food consumption, calorie intake, and maintain healthy eating habits.

## Features

- Food database management for basic and composite foods
- Daily food consumption logs
- Calorie tracking and target calculation
- User profile management with metrics like age, weight, height, and activity level
- Multiple calorie calculation methods (Harris-Benedict and Mifflin-St Jeor)
- Powerful food search capabilities
- Undo functionality for all operations
- Persistent storage in text files

## Compilation Instructions

### Requirements

- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 19.14+)
- Standard Template Library (STL)
- Filesystem support (C++17 feature)

### Building the Application

To compile the application, run the following commands in the project directory:

```bash
# For GCC
g++ -std=c++17 -o yada main.cpp model/*.cpp controller/*.cpp view/*.cpp util/*.cpp -lstdc++fs

# For Clang
clang++ -std=c++17 -o yada main.cpp model/*.cpp controller/*.cpp view/*.cpp util/*.cpp -lstdc++fs

# For MSVC
cl /std:c++17 /EHsc /Feyada.exe main.cpp model/*.cpp controller/*.cpp view/*.cpp util/*.cpp
