cmake_minimum_required(VERSION 3.15)
project(algorithmic_languages_project_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(algorithmic_languages_project_2
        main.cpp
        battleships/game.h
        battleships/game_configuration.h
        battleships/game_field.h
        battleships/simple_game_field.cpp
        battleships/simple_game_field.h
        battleships/console_printable.h
        util/cli_util.cpp
        util/cli_util.h
        battleships/rival_bot.h
        battleships/simple_rival_bot.cpp
        battleships/simple_rival_bot.h
        battleships/coordinate.h
        battleships/direction.h
        battleships/ship_position.h
        battleships/container_util.h battleships/simple_game.h
)