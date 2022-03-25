# geometry.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Jongdeog Lee (jlee700@illinois.edu) on 09/12/2018

"""
This file contains geometry functions that relate with Part1 in MP2.
"""

import math
import numpy as np
from const import *

def computeCoordinate(start, length, angle):
    """Compute the end cooridinate based on the given start position, length and angle.

        Args:
            start (tuple): base of the arm link. (x-coordinate, y-coordinate)
            length (int): length of the arm link
            angle (int): degree of the arm link from x-axis to couter-clockwise

        Return:
            End position (int,int):of the arm link, (x-coordinate, y-coordinate)
    """
    xStart = start[0]
    yStart = start[1]
    angleRad = math.radians(angle)
    xEnd = math.cos(angleRad)*length + xStart
    yEnd = yStart - math.sin(angleRad)*length
    return xEnd,yEnd

#math.floor()
#math.sin()
#math.tan()
#math.cos()
"""def closestPoint(currArm,obj):
    xStart = currArm[0][0]
    yStart = currArm[0][1]
    endx = currArm[1][0]
    endy = currArm[1][1]
    objx = obj[0][0]
    objy = obj[0][1]
    #padding = currArm[2]
    x = endx - xStart
    y = endy - yStart
    temp = (y*(objy-yStart)+x*(objx-xStart))/(y**2+x**2)
    newx = xStart+temp*x
    newy = yStart+temp*y
    return newx,newy"""

def doesArmTouchObjects(armPosDist, objects, isGoal=False):
    """Determine whether the given arm links touch any obstacle or goal

        Args:
            armPosDist (list): start and end position and padding distance of all arm links [(start, end, distance)]
            objects (list): x-, y- coordinate and radius of object (obstacles or goals) [(x, y, r)]
            isGoal (bool): True if the object is a goal and False if the object is an obstacle.
                           When the object is an obstacle, consider padding distance.
                           When the object is a goal, no need to consider padding distance.
        Return:
            True if touched. False if not.
    """

    for currArm in armPosDist:
        for obj in objects:
            xStart = currArm[0][0]
            yStart = currArm[0][1]
            endx = currArm[1][0]
            endy = currArm[1][1]
            objx = obj[0]
            objy = obj[1]
            tempx = endx - xStart
            tempy = endy - yStart
            #top part is just initializing variables and getting the x y coord of the main arm
            dot = tempx**2 + tempy**2
            u = (((objx-xStart)*tempx + (objy - yStart)*tempy)/float(dot))
            if(u>1):
                u=1
            elif(u<0):
                u = 0
            x = xStart + u*tempx
            y = yStart + u*tempy
            xx = x - objx
            yy = y - objy
            #taking distance between closest point on line and the object point
            dist = xx*xx + yy*yy
            dist = dist**.5
            r = obj[2]
            #if its not the goal then the radius r is the padding+radius
            #if it is the goal then the radius is simply the radius
            if(not isGoal):
                r+=currArm[2]
            if dist<= r:
                #print('True')
                return True


    #print('False')
    return False



def doesArmTipTouchGoals(armEnd, goals):
    """Determine whether the given arm tip touch goals

        Args:
            armEnd (tuple): the arm tip position, (x-coordinate, y-coordinate)
            goals (list): x-, y- coordinate and radius of goals [(x, y, r)]. There can be more than one goal.
        Return:
            True if arm tip touches any goal. False if not.
    """
    xArm = armEnd[0]
    yArm = armEnd[1]
    for n in goals:
        goalx = n[0]
        goaly = n[1]
        goalr = n[2]
        dist = math.hypot((goalx-xArm), (goaly-yArm) )
        dist = math.floor(dist)
        if dist <= goalr:
            return True
    return False


def isArmWithinWindow(armPos, window):
    """Determine whether the given arm stays in the window

        Args:
            armPos (list): start and end positions of all arm links [(start, end)]
            window (tuple): (width, height) of the window

        Return:
            True if all parts are in the window. False if not.
    """
    for n in armPos:
        xStart = n[0][0]
        yStart = n[0][1]
        endx = n[1][0]
        endy = n[1][1]
        xWin = window[0]
        yWin = window[1]
        if(xStart<0 or xStart>xWin) or (yStart<0 or yStart>yWin) or (endx<0 or endx>xWin) or (endy<0 or endy>yWin):
            return False
    return True


if __name__ == '__main__':
    computeCoordinateParameters = [((150, 190),100,20), ((150, 190),100,40), ((150, 190),100,60), ((150, 190),100,160)]
    resultComputeCoordinate = [(243, 156), (226, 126), (200, 104), (57, 156)]
    testRestuls = [computeCoordinate(start, length, angle) for start, length, angle in computeCoordinateParameters]
    assert testRestuls == resultComputeCoordinate

    testArmPosDists = [((100,100), (135, 110), 4), ((135, 110), (150, 150), 5)]
    testObstacles = [[(120, 100, 5)], [(110, 110, 20)], [(160, 160, 5)], [(130, 105, 10)]]
    resultDoesArmTouchObjects = [
        True, True, False, True, False, True, False, True,
        False, True, False, True, False, False, False, True
    ]

    testResults = []
    for testArmPosDist in testArmPosDists:
        for testObstacle in testObstacles:
            testResults.append(doesArmTouchObjects([testArmPosDist], testObstacle))
            # print(testArmPosDist)
            # print(doesArmTouchObjects([testArmPosDist], testObstacle))

    print("\n")
    for testArmPosDist in testArmPosDists:
        for testObstacle in testObstacles:
            testResults.append(doesArmTouchObjects([testArmPosDist], testObstacle, isGoal=True))
            # print(testArmPosDist)
            # print(doesArmTouchObjects([testArmPosDist], testObstacle, isGoal=True))

    assert resultDoesArmTouchObjects == testResults

    testArmEnds = [(100, 100), (95, 95), (90, 90)]
    testGoal = [(100, 100, 10)]
    resultDoesArmTouchGoals = [True, True, False]

    testResults = [doesArmTipTouchGoals(testArmEnd, testGoal) for testArmEnd in testArmEnds]
    assert resultDoesArmTouchGoals == testResults

    testArmPoss = [((100,100), (135, 110)), ((135, 110), (150, 150))]
    testWindows = [(160, 130), (130, 170), (200, 200)]
    resultIsArmWithinWindow = [True, False, True, False, False, True]
    testResults = []
    for testArmPos in testArmPoss:
        for testWindow in testWindows:
            testResults.append(isArmWithinWindow([testArmPos], testWindow))
    assert resultIsArmWithinWindow == testResults

    print("Test passed\n")
