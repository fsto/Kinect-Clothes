@echo off
cmd /C git add Clothes/*.cpp
cmd /C git add Clothes/*.hpp
cmd /C git add Clothes/*.h

set /p msg = Enter the commit message

cmd /C git commit -m "%msg%"
cmd /C git pull
cmd /C git push
