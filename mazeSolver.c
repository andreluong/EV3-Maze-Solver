const int ScreenHeight = 127;
const int ScreenWidth = 177;
typedef struct
{
    int NorthWall;
    int EastWall;
    int SouthWall;
    int WestWall;
} Cell;
Cell Grid[4][6];
// Start Facing North
int RobotDirection = 0; // 0=North, 1=East, 2=South, 3=West
// Start in the 0,0 Cell
int StartPosRow = 0; // Starting position
int StartPosCol = 0;
int CurrentPosRow = StartPosRow; // Starting position
int CurrentPosCol = StartPosCol;
int TargetPosRow = 3;
int TargetPosCol = 0;

int hasTurned = 0; // 1 = has turned right
int hasMoved = 0; // 1 = has moved
//int optimalPath[24]; // Must be reversed after target

void Solver();
void GridInit();
void WallGen();
void GridDraw();
void DrawBot();
void DisplayStartandEnd();

task main()
{
    GridInit();
    WallGen();

    while ((CurrentPosRow != TargetPosRow) || (CurrentPosCol != TargetPosCol))
    {
        Solver();
        GridDraw();
        DisplayStartandEnd();
        DrawBot();
        sleep(1000);
        eraseDisplay();
    }

    // TODO: Find the optimal path backwards
    
    
    
    while (true)
    {
        displayCenteredTextLine(5, "MAZE SOLVED !!");
        sleep(500);
        eraseDisplay();
        sleep(500);
    }
}

//=====================================================================
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
void Solver()
{
	/*
		Using the direction of the robot,
		If the robot faces a wall
			- If the robot has turned, perform a geomagnetic reversal 
			- Else, turn right
		Else if there is no wall,
			- If the right side of the robot has no wall and the robot has moved, turn right
			- Else, move in the specified direction
	*/
	// 0=North, 1=East, 2=South, 3=West
	switch (RobotDirection) {
		// North
		case 0: {
			if (Grid[CurrentPosRow][CurrentPosCol].NorthWall == 1) {
				if (hasTurned == 1) {
					RobotDirection = 2; // South
					hasTurned = 0;
				}	else {
					RobotDirection = 1; // East
					hasTurned = 1;
				}	
				hasMoved = 0;
			} else {
				if (Grid[CurrentPosRow][CurrentPosCol].EastWall == 0 && hasMoved == 1) {
					RobotDirection = 1; // East
					hasMoved = 0;
				} else {
					CurrentPosRow += 1; // Move up
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;
		}
		// East
		case 1: {
			if (Grid[CurrentPosRow][CurrentPosCol].EastWall == 1) {
				if (hasTurned == 1) {
					RobotDirection = 3; // West
					hasTurned = 0;
				}	else {
					RobotDirection = 2; // South
					hasTurned = 1;
				}	
				hasMoved = 0;
			} else {
				if (Grid[CurrentPosRow][CurrentPosCol].SouthWall == 0 && hasMoved == 1) {
					RobotDirection = 2; // South
					hasMoved = 0;
				} else {
					CurrentPosCol += 1; // Move right
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;		
		}
		// South
		case 2: {
			if (Grid[CurrentPosRow][CurrentPosCol].SouthWall == 1) {
				if (hasTurned == 1) {
					RobotDirection = 0; // North
					hasTurned = 0;
				}	else {
					RobotDirection = 3; // West
					hasTurned = 1;
				}	
				hasMoved = 0;
			} else {
				if (Grid[CurrentPosRow][CurrentPosCol].WestWall == 0 && hasMoved == 1) {
					RobotDirection = 3; // West
					hasMoved = 0;
				} else {
					CurrentPosRow -= 1; // Move down
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;		
		}
		// West
		case 3: {
			if (Grid[CurrentPosRow][CurrentPosCol].WestWall == 1) {
				if (hasTurned == 1) {
					RobotDirection = 1; // East
					hasTurned = 0;
				}	else {
					RobotDirection = 0; // North
					hasTurned = 1;
				}	
				hasMoved = 0;
			} else {
				if (Grid[CurrentPosRow][CurrentPosCol].NorthWall == 0 && hasMoved == 1) {
					RobotDirection = 0; // North
					hasMoved = 0;
				} else {
					CurrentPosCol -= 1; // Move left
					hasMoved = 1;
				}
				hasTurned = 0;
			}
			break;		
		}
	}
}

//=====================================================================
void DisplayStartandEnd()
{
    int XpixelPos = 0;
    int YpixelPos = 0;

    if (StartPosCol == 0)
    {
        XpixelPos = ScreenWidth / 12;
    } else
    {
        XpixelPos = (2 * StartPosCol + 1) * ScreenWidth / 12;
    }
       
    if (StartPosRow == 0)
    {
        YpixelPos = ScreenHeight / 8;
    } else
    {
        YpixelPos = (2 * StartPosRow + 1) * ScreenHeight / 8;
    }

    displayStringAt(XpixelPos, YpixelPos, "S");
    if (TargetPosCol == 0)
    {
        XpixelPos = ScreenWidth / 12;
    } else
    {
        XpixelPos = (2 * TargetPosCol + 1) * ScreenWidth / 12;
    }

    if (TargetPosRow == 0)
    {
        YpixelPos = ScreenHeight / 8;
    } else
    {
        YpixelPos = (2 * TargetPosRow + 1) * ScreenHeight / 8;
    }

    displayStringAt(XpixelPos, YpixelPos, "E");
}
