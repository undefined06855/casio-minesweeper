@echo off
..\..\bin\make.exe %*

REM change this if different for your system, might consider a better build system in the future but there is too sparse
REM documentation on this... does everyone have their prizm make two folders up? it's pretty annoying
copy minesweeper.g3a D:\minesweeper.g3a
