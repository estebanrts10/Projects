# search.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Jongdeog Lee (jlee700@illinois.edu) on 09/12/2018

"""
This file contains search functions.
"""
# Search should return the path and the number of states explored.
# The path should be a list of tuples in the form (alpha, beta, gamma) that correspond
# to the positions of the path taken by your search algorithm.
# Number of states explored should be a number.
# maze is a Maze object based on the maze from the file specified by input filename
# searchMethod is the search method specified by --method flag (bfs,astar)
# You may need to slight change your previous search functions in MP1 since this is 3-d maze

from collections import deque
from heapq import heappop, heappush
import queue

def search(maze, searchMethod):
    return {
        "bfs": bfs,
    }.get(searchMethod, [])(maze)

def bfs(maze):
    """
    Runs BFS for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    #took the bfs algorithm from mp1 and just added the closest point helper to find the closest point and use that as the objective
    frontier = queue.Queue()
    visited = []
    path = []
    ret = dict([])
    start = maze.getStart()
    objList = maze.getObjectives()
    objQueue = nearestDot(maze,objList,start)
    #print(objQueue)
    obj = objQueue.get()[1]
    print("obj:", obj)
    curr = start
    exploredStates = 0
    while (curr != obj):
        neighbors = maze.getNeighbors(curr[0],curr[1])
        while(len(neighbors) != 0):
            neighbor = neighbors.pop()
            if (neighbor not in visited):
                exploredStates += 1
                frontier.put(neighbor)
                ret[neighbor] = curr
                visited.append(neighbor)
                #print("AHAHA")
        if(curr not in visited):
            visited.append(curr)
        #print("AHAHA12312")
        if frontier.empty():
            if(not objQueue.empty()):
                obj = objQueue.get()[1]
                curr = start
                path = []
                visited = []
                ret = dict([])
                continue
            print("objNotFound")
            return None
        temp = frontier.get()
        #ret[temp] = curr     #key of dictionary is the return spot and value is the next
        curr = temp
    if(curr!=obj):
#        print("noobjective")
        return None
    while curr != start:
        #print('lul')
        path.append(curr)
        curr = ret[curr]
    path.append(curr)
    path.reverse()

#    print("path:",path)
#    print("exploredStates:",exploredStates)
    return path, exploredStates



def nearestDot(maze,objectives,start):
    objQueue = queue.PriorityQueue()


    for i in objectives:
        dist = abs(i[0]-start[0]) + abs(i[1]-start[1])
        objQueue.put((dist,(i)))
    return objQueue
