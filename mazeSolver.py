#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile

import mazeHelper
import time

SCREEN_HEIGHT = 127
SCREEN_WIDTH = 177
MAX_ROWS = 4
MAX_COLS = 6

class Cell:
    def __init__(self, northWall, eastWall, southWall, westWall):
        self.northWall = northWall
        self.eastWall = eastWall
        self.southWall = southWall
        self.westWall = westWall

grid = [[Cell(0, 0, 0, 0) for _ in range(MAX_ROWS)] for _ in range(MAX_COLS)]

robotDirection = 0 # 0=North, 1=East, 2=South, 3=West

startPosRow = 0
startPosCol = 0
currentPosRow = startPosRow
currentPosCol = startPosCol
targetPosRow = 3
targetPosCol = 0

hasTurned = 0 # 1 = has turned right
hasMoved = 0 # 1 = has moved

# Optimal path stack using list
optimalPathStack = []
# pop when cur direction is the reversal of the last direction
# append otherwise



# Using the direction of the robot,
# If the robot faces a wall
# 	- If the robot has turned, perform a geomagnetic reversal 
# 	- Else, turn right
# Else if there is no wall,
# 	- If the right side of the robot has no wall and the robot has moved, turn right
# 	- Else, move based on the direction
def move(wall, rightWall):
    if wall == 1:
        if hasTurned == 1:
            robotDirection = (robotDirection + 2) % 4
            hasTurned = 0
        else:
            robotDirection = (robotDirection + 1) % 4
            hasTurned = 1
        hasMoved = 0
    else:
        if rightWall == 0 and hasMoved == 1:
            robotDirection = (robotDirection + 1) % 4
            hasMoved = 0
        else:
            if robotDirection == 0:
                currentPosRow -= 1
            elif robotDirection == 1:
                currentPosCol += 1
            elif robotDirection == 2:
                currentPosRow += 1
            elif robotDirection == 3:
                currentPosCol -= 1

            # If last move was the opposite direction of the current direction, remove it
            if (optimalPathStack[-1] == (robotDirection + 2) % 4):
                optimalPathStack.pop()
            else:
                optimalPathStack.append(robotDirection)
            
            hasMoved = 1
        hasTurned = 0

def solver():
	# 0=North, 1=East, 2=South, 3=West
    match robotDirection:
        case 0:
            move(grid[currentPosRow][currentPosCol].northWall, grid[currentPosRow][currentPosCol].eastWall)
        case 1:
            move(grid[currentPosRow][currentPosCol].eastWall, grid[currentPosRow][currentPosCol].southWall)
        case 2:
            move(grid[currentPosRow][currentPosCol].southWall, grid[currentPosRow][currentPosCol].westWall)
        case 3:
            move(grid[currentPosRow][currentPosCol].westWall, grid[currentPosRow][currentPosCol].northWall)


# Main loop
while True:
    mazeHelper.gridInit(grid)
    mazeHelper.wallGen(grid)

    while (currentPosRow != targetPosRow) or (currentPosCol != targetPosCol):
        solver()
        mazeHelper.gridDraw(grid, SCREEN_WIDTH, SCREEN_HEIGHT)
        mazeHelper.displayStartAndEnd(startPosRow, startPosCol, targetPosRow, targetPosCol, SCREEN_WIDTH, SCREEN_HEIGHT)
        mazeHelper.drawBot(robotDirection, currentPosRow, currentPosCol, SCREEN_WIDTH, SCREEN_HEIGHT)
        time.sleep(1000)
        EV3Brick.screen.clear()


    # Reverse back to start
    break

for x in optimalPathStack:
    print(x)

while True:
    EV3Brick.screen.print("Maze Solved !!")
    time.sleep(500)
    EV3Brick.screen.clear()
    time.sleep(500)
