
# transform.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Jongdeog Lee (jlee700@illinois.edu) on 09/12/2018

"""
This file contains the transform function that converts the robot arm map
to the maze.
"""
import copy
from arm import Arm
from maze import Maze
from search import *
from geometry import *
from const import *
from util import *

def transformToMaze(arm, goals, obstacles, window, granularity):
    """This function transforms the given 2D map to the maze in MP1.

        Args:
            arm (Arm): arm instance
            goals (list): [(x, y, r)] of goals
            obstacles (list): [(x, y, r)] of obstacles
            window (tuple): (width, height) of the window
            granularity (int): unit of increasing/decreasing degree for angles

        Return:
            Maze: the maze instance generated based on input arguments.

    """
    armAng = arm.getArmAngle()
    armLim = arm.getArmLimit()
    aLim = armLim[0]
    bLim = armLim[1]
    armPosDist = arm.getArmPosDist()
    idx = angleToIdx([aLim[1],bLim[1]],[aLim[0],bLim[0]],granularity)
    rows = idx[0] + 1
    cols = idx[1] + 1
    maze =[[' ' for x in range(cols)] for y in range(rows)]
    aMin = armAng[0]
    #print(granularity)
    bMin = armAng[1]
    xType = type(1)
    #print (aMin)
    #print(aLim)
    for u in range(rows):
        for v in range(cols):
            #taking index into tdxToAngle in order to get the angles necessary
            #to input into setArmAngle
            angle = idxToAngle([u,v],[aLim[0],bLim[0]],granularity)
            if(type(angle[0])==xType):
                arm.setArmAngle(angle)
            if ((not isArmWithinWindow(arm.getArmPos(),window)) or doesArmTouchObjects(arm.getArmPosDist(),obstacles)):
                maze[u][v] = WALL_CHAR
                #check if its wall or object in the way
            elif(doesArmTipTouchGoals(arm.getEnd(),goals)):
                maze[u][v] = OBJECTIVE_CHAR
                #if touches goal then its an objective P
            elif (doesArmTouchObjects(arm.getArmPosDist(),goals,True)):
                maze[u][v] = OBJECTIVE_CHAR
                #if it touches the objective but not with the tip then you use this function
            else:
                maze[u][v] = SPACE_CHAR
            #numRows = (n[1] - n[0])/granularity
    #print("grangran", granularity)
    startRow = math.floor((aMin-aLim[0])/granularity) +1 - int((granularity)**.25)
    startCol = math.floor((bMin-bLim[0])/granularity) +1 - int((granularity)**.25)
    maze[startRow][startCol] = START_CHAR

    return Maze(maze, [aLim[0],bLim[0]],granularity)
