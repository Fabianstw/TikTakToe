cmake_minimum_required(VERSION 3.27)
project(TikTakToe)

set(CMAKE_CXX_STANDARD 23)

add_executable(TikTakToe main.cpp
        GameDict/TicTacToeGame.cpp
        GameDict/TicTacToeGame.h)
