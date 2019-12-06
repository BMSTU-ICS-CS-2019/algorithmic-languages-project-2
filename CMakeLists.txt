cmake_minimum_required(VERSION 3.15)
project(algorithmic_languages_project_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(algorithmic_languages_project_2 main.cpp game/game.h game/game_configuration.h game/game_field.h game/simple_game_field.cpp game/simple_game_field.h game/console_printable.h)