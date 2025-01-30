#!/bin/bash

# Установите необходимые пакеты
# sudo pacman -S --needed cmake gcc make

# Создаем директорию для сборки
mkdir -p build
cd build

# Запускаем CMake
cmake ..

# Сборка проекта
make

# Запускаем программы в фоновом режиме
# ./program1/program1 &
# ./program2/program2 &

# Ожидаем завершения всех фоновых процессов
# wait