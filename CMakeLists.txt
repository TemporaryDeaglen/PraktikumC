cmake_minimum_required(VERSION 3.10)

# Устанавливаем имя проекта
project(MyProject)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем поддиректории
add_subdirectory(lib)
add_subdirectory(program1)
add_subdirectory(program2)
