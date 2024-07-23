#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile

def gridInit(grid):
    for i in range(4):
        for j in range(6):
            grid[i][j].NorthWall = 0
            grid[i][j].EastWall = 0
            grid[i][j].WestWall = 0
            grid[i][j].SouthWall = 0

def wallGen(grid):
    for i in range(4):
        grid[i][0].WestWall = 1
        grid[i][5].EastWall = 1

    for j in range(6):
        grid[0][j].SouthWall = 1
        grid[3][j].NorthWall = 1

    grid[0][0].NorthWall = 1
    grid[1][0].SouthWall = 1
    grid[0][1].NorthWall = 1
    grid[1][1].SouthWall = 1
    grid[0][3].EastWall = 1
    grid[0][4].WestWall = 1
    grid[1][2].EastWall = 1
    grid[1][3].WestWall = 1
    grid[1][3].EastWall = 1
    grid[1][4].WestWall = 1
    grid[1][4].EastWall = 1
    grid[1][5].WestWall = 1
    grid[1][5].NorthWall = 1
    grid[2][5].SouthWall = 1
    grid[3][0].EastWall = 1
    grid[3][1].WestWall = 1
    grid[3][4].SouthWall = 1
    grid[2][4].NorthWall = 1

    for j in range(1, 4):
        grid[2][j].NorthWall = 1
        grid[2][j].SouthWall = 1
        grid[3][j].SouthWall = 1
        grid[1][j].NorthWall = 1

def gridDraw(grid, screen_width, screen_height):
    for i in range(4):
        for j in range(6):
            if grid[i][j].NorthWall:
                x_start = j * screen_width # 6
                y_start = (i + 1) * screen_height # 4
                x_end = (j + 1) * screen_width # 6
                y_end = (i + 1) * screen_height # 4
                EV3Brick.screen.draw_line(x_start, y_start, x_end, y_end)

            if grid[i][j].EastWall:
                x_start = (j + 1) * screen_width # 6
                y_start = i * screen_height # 4
                x_end = (j + 1) * screen_width # 6
                y_end = (i + 1) * screen_height # 4
                EV3Brick.screen.draw_line(x_start, y_start, x_end, y_end)

            if grid[i][j].WestWall:
                x_start = j * screen_width # 6
                y_start = i * screen_height # 4
                x_end = j * screen_width # 6
                y_end = (i + 1) * screen_height # 4
                EV3Brick.screen.draw_line(x_start, y_start, x_end, y_end)

            if grid[i][j].SouthWall:
                x_start = j * screen_width # 6
                y_start = i * screen_height # 4
                x_end = (j + 1) * screen_width # 6
                y_end = i * screen_height # 4
                EV3Brick.screen.draw_line(x_start, y_start, x_end, y_end)

def drawBot(robotDirection, currentPosRow, currentPosCol, screen_width, screen_height):
    robotXpixelPos = 0
    robotYpixelPos = 0

    if currentPosCol == 0:
        robotXpixelPos = screen_width / 12
    else:
        robotXpixelPos = (2 * currentPosCol + 1) * screen_width / 12
    
    if currentPosRow == 0:
        robotYpixelPos = screen_height / 8
    else:
        robotYpixelPos = (2 * currentPosRow + 1) * screen_height / 8
    
    displayText = ""

    match robotDirection:
        case 0:
            displayText = "^"
        case 1:
            displayText = ">"
        case 2:
            displayText = "v"
        case 3:
            displayText = "<"
        case _:
            displayText = "X"

    EV3Brick.screen.draw_text(robotXpixelPos, robotYpixelPos, displayText)

def displayStartAndEnd(startPosRow, startPosCol, targetPosRow, targetPosCol, screen_width, screen_height):
    XpixelPos = 0
    YpixelPos = 0

    # Draw start position
    if startPosCol == 0:
        XpixelPos = screen_width / 12
    else:
        XpixelPos = (2 * startPosCol + 1) * screen_width / 12

    if startPosRow == 0:
        YpixelPos = screen_height / 8
    else:
        YpixelPos = (2 * startPosRow + 1) * screen_height / 8

    EV3Brick.screen.draw_text(XpixelPos, YpixelPos, "S")

    # Draw end position
    if targetPosCol == 0:
        XpixelPos = screen_width / 12
    else:
        XpixelPos = (2 * targetPosCol + 1) * screen_width / 12

    if targetPosRow == 0:
        YpixelPos = screen_height / 8
    else:
        YpixelPos = (2 * targetPosRow + 1) * screen_height / 8
    
    EV3Brick.screen.draw_text(XpixelPos, YpixelPos, "E")