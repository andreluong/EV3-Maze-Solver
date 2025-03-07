#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile

import mazeHelper
import time # sleep function

ev3 = EV3Brick()

SCREEN_HEIGHT = 127
SCREEN_WIDTH = 177
MAX_ROWS = 4
MAX_COLS = 6

START_POS_ROW = 0
START_POS_COL = 0
TARGET_POS_ROW = 3
TARGET_POS_COL = 0

class Cell:
    def __init__(self, northWall, eastWall, southWall, westWall):
        self.northWall = northWall
        self.eastWall = eastWall
        self.southWall = southWall
        self.westWall = westWall

grid = [[Cell(0, 0, 0, 0) for _ in range(MAX_ROWS)] for _ in range(MAX_COLS)]

robotDirection = 0 # 0=North, 1=East, 2=South, 3=West

currentPosRow = START_POS_ROW
currentPosCol = START_POS_COL

hasTurned = False
hasMoved = False

# Optimal path stack using list
optimalPathStack = []
# pop when cur direction is the reversal of the last direction
# append otherwise



# If the robot faces a wall
# 	- If the robot has turned, perform a geomagnetic reversal 
# 	- Else, turn right
# Else if there is no wall,
# 	- If the right side of the robot has no wall and the robot has moved, turn right
# 	- Else, move based on the direction
def move(wall, rightWall):
    if wall == 1:
        if hasTurned == True:
            robotDirection = (robotDirection + 2) % 4
            hasTurned = False
        else:
            robotDirection = (robotDirection + 1) % 4
            hasTurned = True
        hasMoved = False
    else:
        if rightWall == 0 and hasMoved == True:
            robotDirection = (robotDirection + 1) % 4
            hasMoved = False
        else:
            if robotDirection == 0:
                currentPosRow -= 1
            elif robotDirection == 1:
                currentPosCol += 1
            elif robotDirection == 2:
                currentPosRow += 1
            elif robotDirection == 3:
                currentPosCol -= 1

            # If last move is the opposite direction of the current direction, remove it
            if (optimalPathStack[-1] == (robotDirection + 2) % 4):
                optimalPathStack.pop()
            else:
                optimalPathStack.append(robotDirection)
            
            hasMoved = True
        hasTurned = False

def pathFinder():
	# 0=North, 1=East, 2=South, 3=West
    gridCell = grid[currentPosRow][currentPosCol]
    match robotDirection:
        case 0:
            move(gridCell.northWall, gridCell.eastWall)
        case 1:
            move(gridCell.eastWall, gridCell.southWall)
        case 2:
            move(gridCell.southWall, gridCell.westWall)
        case 3:
            move(gridCell.westWall, gridCell.northWall)

# Draws the robot and maze on the screen
def drawOnScreen():
    mazeHelper.gridDraw(grid, SCREEN_WIDTH, SCREEN_HEIGHT)
    mazeHelper.displayStartAndEnd(START_POS_ROW, START_POS_COL, TARGET_POS_ROW, TARGET_POS_COL, SCREEN_WIDTH, SCREEN_HEIGHT)
    mazeHelper.drawBot(robotDirection, currentPosRow, currentPosCol, SCREEN_WIDTH, SCREEN_HEIGHT)
    time.sleep(1000)
    ev3.screen.clear()

# Reverses the directions in the optimal path stack
def reverseStack():
    for i in range(len(optimalPathStack)):
        optimalPathStack[i] = (optimalPathStack[i] + 2) % 4

# Fix directions
def moveUsingStack():
    for path in optimalPathStack:
        match path:
            case 0:
                currentPosRow -= 1
            case 1:
                currentPosCol += 1
            case 2:
                currentPosRow += 1
            case 3:
                currentPosCol -= 1
        drawOnScreen()
    
    optimalPathStack.clear()



# Main loop
while True:
    mazeHelper.gridInit(grid)
    mazeHelper.wallGen(grid)

    while (currentPosRow != TARGET_POS_ROW) or (currentPosCol != TARGET_POS_COL):
        pathFinder()
        drawOnScreen()

    break

# Print the optimal path stack
for x in optimalPathStack:
    match x:
        case 0:
            print("North")
        case 1:
            print("East")
        case 2:
            print("South")
        case 3:
            print("West")

# Reverse back to start
reverseStack()
moveUsingStack()

# Ending message
while True:
    ev3.screen.print("Maze Solved !!")
    time.sleep(500)
    ev3.screen.clear()
    time.sleep(500)
