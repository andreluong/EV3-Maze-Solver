const int ScreenHeight = 127;
const int ScreenWidth = 177;
const int MAX_ROWS = 4;
const int MAX_COLS = 6;

const int START_POS_ROW = 0;
const int START_POS_COL = 0;
const int TARGET_POS_ROW = 3;
const int TARGET_POS_COL = 0;

typedef struct
{
    int NorthWall;
    int EastWall;
    int SouthWall;
    int WestWall;
} Cell;

Cell Grid[MAX_ROWS][MAX_COLS];

// Start Facing North
int RobotDirection = 0; // 0=North, 1=East, 2=South, 3=West
int CurrentPosRow = START_POS_ROW; // Starting position
int CurrentPosCol = START_POS_COL;

int hasTurned = 0; // 1 = has turned right
int hasMoved = 0; // 1 = has moved

int optimalPathStack[MAX_ROWS * MAX_COLS * 2];
int optimalPathStackIndex = -1;

// Function prototypes
void Solver();
void GridInit();
void WallGen();
void GridDraw();
void DrawBot();
void DisplayStartandEnd();
void reverseStack();
void addToPath();
void moveUsingStack();
void smoothDirections();

void testingDatalog()
{
    // Testing
    datalogAddShort(0, 9999);
    for (int i = optimalPathStackIndex; i >= 0; i--)
    {
    	int curCell = optimalPathStack[i];
    	datalogAddShort(0, curCell);

    	if (curCell == 0)
    	{
    			displayCenteredTextLine(5, "North");
    	} else if (curCell == 1)
    	{
    		displayCenteredTextLine(5, "East");
    	} else if (curCell == 2)
    	{
    		displayCenteredTextLine(5, "South");
    	} else if (curCell == 3)
    	{
    		displayCenteredTextLine(5, "West");
    	}
    	sleep(1000);
    	eraseDisplay();
    	sleep(500);
    }
}

task main()
{
    datalogFlush();
    datalogClose();

    if (!datalogOpen(1, 1, false)) {
        return;
    }

    GridInit();
    WallGen();

    while ((CurrentPosRow != TARGET_POS_ROW) || (CurrentPosCol != TARGET_POS_COL))
    {
        Solver();
        GridDraw();
        DisplayStartandEnd();
        DrawBot();
        sleep(50);
        eraseDisplay();
    }

    reverseStack();
    smoothDirections();
    
    // TODO: remove
    testingDatalog();

    sleep(500);
    moveUsingStack();

    datalogClose();
    while (true)
    {
        displayCenteredTextLine(5, "MAZE SOLVED !!");
        sleep(500);
        eraseDisplay();
        sleep(500);
    }
}

void GridInit()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            Grid[i][j].NorthWall = 0;
            Grid[i][j].EastWall = 0;
            Grid[i][j].WestWall = 0;
            Grid[i][j].SouthWall = 0;
        }
    }
}

//=====================================================================
void WallGen()
{
    int i = 0;
    int j = 0;

    for (i = 0; i < 4; i++)
    {
        Grid[i][0].WestWall = 1;
        Grid[i][5].EastWall = 1;
    }

    for (j = 0; j < 6; j++)
    {
        Grid[0][j].SouthWall = 1;
        Grid[3][j].NorthWall = 1;
    }

    Grid[0][0].NorthWall = 1;
    Grid[1][0].SouthWall = 1;
    Grid[0][1].NorthWall = 1;
    Grid[1][1].SouthWall = 1;
    Grid[0][3].EastWall = 1;
    Grid[0][4].WestWall = 1;
    Grid[1][2].EastWall = 1;
    Grid[1][3].WestWall = 1;
    Grid[1][3].EastWall = 1;
    Grid[1][4].WestWall = 1;
    Grid[1][4].EastWall = 1;
    Grid[1][5].WestWall = 1;
    Grid[1][5].NorthWall = 1;
    Grid[2][5].SouthWall = 1;
    Grid[3][0].EastWall = 1;
    Grid[3][1].WestWall = 1;
    Grid[3][4].SouthWall = 1;
    Grid[2][4].NorthWall = 1;

    for (j = 1; j < 4; j++)
    {
        Grid[2][j].NorthWall = 1;
        Grid[2][j].SouthWall = 1;
        Grid[3][j].SouthWall = 1;
        Grid[1][j].NorthWall = 1;
    }
}

//=====================================================================
void GridDraw()
{
    int XStart = 0;
    int YStart = 0;
    int XEnd = 0;
    int YEnd = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (Grid[i][j].NorthWall == 1)
            {
                XStart = j * ScreenWidth / 6;
                YStart = (i + 1) * ScreenHeight / 4;
                XEnd = (j + 1) * ScreenWidth / 6;
                YEnd = (i + 1) * ScreenHeight / 4;
                drawLine(XStart, YStart, XEnd, YEnd);
            }

            if (Grid[i][j].EastWall == 1)
            {
                XStart = (j + 1) * ScreenWidth / 6;
                YStart = (i)*ScreenHeight / 4;
                XEnd = (j + 1) * ScreenWidth / 6;
                YEnd = (i + 1) * ScreenHeight / 4;
                drawLine(XStart, YStart, XEnd, YEnd);
            }

            if (Grid[i][j].WestWall == 1)
            {
                XStart = j * ScreenWidth / 6;
                YStart = (i)*ScreenHeight / 4;
                XEnd = (j)*ScreenWidth / 6;
                YEnd = (i + 1) * ScreenHeight / 4;
                drawLine(XStart, YStart, XEnd, YEnd);
            }

            if (Grid[i][j].SouthWall == 1)
            {
                XStart = j * ScreenWidth / 6;
                YStart = (i)*ScreenHeight / 4;
                XEnd = (j + 1) * ScreenWidth / 6;
                YEnd = (i)*ScreenHeight / 4;
                drawLine(XStart, YStart, XEnd, YEnd);
            }
        }
    }
}

//=====================================================================
void DrawBot()
{
    int RobotXpixelPos = 0;
    int RobotYpixelPos = 0;

    if (CurrentPosCol == 0)
    {
        RobotXpixelPos = ScreenWidth / 12;
    } else
    {
        RobotXpixelPos = (2 * CurrentPosCol + 1) * ScreenWidth / 12;
    }

    if (CurrentPosRow == 0)
    {
        RobotYpixelPos = ScreenHeight / 8;
    } else
    {
        RobotYpixelPos = (2 * CurrentPosRow + 1) * ScreenHeight / 8;
    }

    switch (RobotDirection)
    {
        case 0:
            displayStringAt(RobotXpixelPos, RobotYpixelPos, "^");
            break; // Facing North
        case 1:
            displayStringAt(RobotXpixelPos, RobotYpixelPos, ">");
            break; // Facing East
        case 2:
            displayStringAt(RobotXpixelPos, RobotYpixelPos, "V");
            break; // Facing South
        case 3:
            displayStringAt(RobotXpixelPos, RobotYpixelPos, "<");
            break; // Facing West
        default:
            break;
    }
}

//=====================================================================
void DisplayStartandEnd()
{
    int XpixelPos = 0;
    int YpixelPos = 0;

    if (START_POS_COL == 0)
    {
        XpixelPos = ScreenWidth / 12;
    } else
    {
        XpixelPos = (2 * START_POS_COL + 1) * ScreenWidth / 12;
    }

    if (START_POS_ROW == 0)
    {
        YpixelPos = ScreenHeight / 8;
    } else
    {
        YpixelPos = (2 * START_POS_ROW + 1) * ScreenHeight / 8;
    }
    displayStringAt(XpixelPos, YpixelPos, "S");

    if (TARGET_POS_COL == 0)
    {
        XpixelPos = ScreenWidth / 12;
    } else
    {
        XpixelPos = (2 * TARGET_POS_COL + 1) * ScreenWidth / 12;
    }

    if (TARGET_POS_ROW == 0)
    {
        YpixelPos = ScreenHeight / 8;
    } else
    {
        YpixelPos = (2 * TARGET_POS_ROW + 1) * ScreenHeight / 8;
    }
    displayStringAt(XpixelPos, YpixelPos, "E");
}

// Adds the current direction to the optimal path stack
void addToPath()
{
	if (optimalPathStackIndex < 0) {
		optimalPathStack[0] = RobotDirection;
		optimalPathStackIndex = 0;
		datalogAddShort(0, RobotDirection); // TODO: remove
	} else if (optimalPathStack[optimalPathStackIndex] == (RobotDirection + 2) % 4) {
		optimalPathStack[optimalPathStackIndex] = -1;
		optimalPathStackIndex--;
		datalogAddShort(0, -1); // TODO: remove
	} else {
		optimalPathStackIndex++;
		optimalPathStack[optimalPathStackIndex] = RobotDirection;
		datalogAddShort(0, RobotDirection); // TODO: remove
	}
}

void Solver()
{
	switch (RobotDirection)
	{
		case 0:	{
			if (Grid[CurrentPosRow][CurrentPosCol].NorthWall == 1)
			{
				if (hasTurned == 1) {
					RobotDirection = 1;
					hasTurned = 0;
				} else {
					RobotDirection = 1;
					hasTurned = 1;
				}
				hasMoved = 0;
			}	else {
				if (Grid[CurrentPosRow][CurrentPosCol].EastWall == 0 && hasMoved == 1) {
					RobotDirection = 1;
					hasMoved = 0;
				} else {
					CurrentPosRow++;
					addToPath();
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;
		}

		case 1: {
			if (Grid[CurrentPosRow][CurrentPosCol].EastWall == 1) {
				if (hasTurned == 1)
				{
					RobotDirection = 3;
					hasTurned = 0;
				} else {
					RobotDirection = 2;
					hasTurned = 1;
				}
				hasMoved = 0;
			} else {
				if (Grid[CurrentPosRow][CurrentPosCol].SouthWall == 0 && hasMoved == 1) {
					RobotDirection = 2;
					hasMoved = 0;
				} else {
					CurrentPosCol++;
					addToPath();
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;
		}

			case 2: {
				if (Grid[CurrentPosRow][CurrentPosCol].SouthWall == 1) {
					if (hasTurned == 1)
					{
						RobotDirection = 0;
						hasTurned = 0;
					} else {
						RobotDirection = 3;
						hasTurned = 1;
					}
					hasMoved = 0;
				} else {
					if (Grid[CurrentPosRow][CurrentPosCol].WestWall == 0 && hasMoved == 1) {
						RobotDirection = 3;
						hasMoved = 0;
					} else {
						CurrentPosRow--;
						addToPath();
						hasMoved = 1;
					}
					hasTurned = 0;
				}
				break;
			}

			case 3: {
				if (Grid[CurrentPosRow][CurrentPosCol].WestWall == 1) {
					if (hasTurned == 1)
					{
						RobotDirection = 1;
						hasTurned = 0;
					} else {
						RobotDirection = 0;
						hasTurned = 1;
					}
					hasMoved = 0;
				} else {
					if (Grid[CurrentPosRow][CurrentPosCol].NorthWall == 0 && hasMoved == 1) {
						RobotDirection = 0;
						hasMoved = 0;
					} else {
						CurrentPosCol--;
						addToPath();
						hasMoved = 1;
					}
					hasTurned = 0;
				}
				break;
			}
	}
}

void reverseStack()
{
	// Reverse the directions
	for (int i = 0; i <= optimalPathStackIndex; i++)
	{
		optimalPathStack[i] = (optimalPathStack[i] + 2) % 4;
	}
}

// Add an extra direction for smoother movement
// Example: N, N, E, E, S, E --> N, N, N, E, E, E, S, S, E, E
void smoothDirections()
{
    int tempArr[MAX_ROWS * MAX_COLS];
    int index = 0;

    // Append an extra direction to each group of directions
    for (int i = 0; i < optimalPathStackIndex; i++) {
        int curDirection = optimalPathStack[i];

        if (index == 0) {
            tempArr[index] = curDirection;
            index++;
        } else {
            int nextDirection = optimalPathStack[i+1];
            
            if (curDirection == nextDirection) {
                tempArr[index] = curDirection;
                index++;
            } else {
                // Duplicate current direction
                tempArr[index] = curDirection;
                tempArr[index+1] = curDirection;
                index += 2;
            }
        }
    }

    optimalPathStack = tempArr;
    optimalPathStackIndex = index
}

void moveUsingStack()
{
	for (int i = optimalPathStackIndex; i >= 0; i--) {
		RobotDirection = optimalPathStack[i];

		if (RobotDirection == 0) {
			CurrentPosRow++;
		} else if (RobotDirection == 1) {
			CurrentPosCol++;
		} else if (RobotDirection == 2) {
			CurrentPosRow--;
		} else if (RobotDirection == 3) {
			CurrentPosCol--;
		}

        GridDraw();
        DisplayStartandEnd();
        DrawBot();
        sleep(1000);
        eraseDisplay();
    }
}