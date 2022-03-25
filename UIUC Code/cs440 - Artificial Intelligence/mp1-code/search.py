# search.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Michael Abir (abir2@illinois.edu) on 08/28/2018
import queue
"""
This is the main entry point for MP1. You should only modify code
within this file -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""
# Search should return the path.
# The path should be a list of tuples in the form (row, col) that correspond
# to the positions of the path taken by your search algorithm.
# maze is a Maze object based on the maze from the file specified by input filename
# searchMethod is the search method specified by --method flag (bfs,dfs,astar,astar_multi,fast)

def search(maze, searchMethod):
    return {
        "bfs": bfs,
        "astar": astar,
        "astar_corner": astar_corner,
        "astar_multi": astar_multi,
        "fast": fast,
    }.get(searchMethod)(maze)
"""
getStart()
getObjectives()
isValidMove(row, col)
isValidPath(path)
getNeighbors(row, col)
"""
#python mp1.py maps/new_map/single/tinyMaze.txt --method bfs
def bfs(maze):
    """
    Runs BFS for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    frontier = queue.Queue()
    visited = []
    path = []
    ret = dict([])

    obj = maze.getObjectives()
    obj = obj[0]
    start = maze.getStart()
    curr = start
    while (curr != obj):
        neighbors = maze.getNeighbors(curr[0],curr[1])
        while(len(neighbors) != 0):
            neighbor = neighbors.pop()
            if (neighbor not in visited):
                frontier.put(neighbor)
                ret[neighbor] = curr
                visited.append(neighbor)
        if(curr not in visited):
            visited.append(curr)
        temp = frontier.get()
        #ret[temp] = curr     #key of dictionary is the return spot and value is the next
        curr = temp
    while curr != start:
        #print('lul')
        path.append(curr)
        curr = ret[curr]
    path.append(curr)
    path.reverse()


    return path
#access dictionary spot by doing dictname[keys]
#adding something to a dictionary is dictname[key] = value

def astar(maze):
    """
    Runs A star for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    blocksMoved = dict([])
    ret = dict([])
    obj = maze.getObjectives()
    obj = obj[0]
    milage = 0
    visited = []
    path=[]
    start = maze.getStart()
    curr = start
    visited.append(start)
    toGoal = abs(obj[0]-start[0]) + abs(obj[1]-start[1])
    frontier = queue.PriorityQueue() #get and put
    frontier.put((toGoal, (start)))
    blocksMoved[start] = 0

    while (curr != obj) and (len(visited) != 0):
        if(curr not in visited):
            visited.append(curr)
        curr = frontier.get()[1]
         #removes item from queue and adds it to visited
        neighbors = maze.getNeighbors(curr[0],curr[1])
        while(len(neighbors) != 0):
            neighbor = neighbors.pop()
            if neighbor not in visited:
                milage = blocksMoved[curr] + 1
                temp = abs(obj[0]-neighbor[0]) + abs(obj[1]-neighbor[1]) + milage
                frontier.put((temp,(neighbor)))
                blocksMoved[neighbor] = milage+1
                ret[neighbor] = curr
                visited.append(neighbor)

        milage += 1

    while curr != start:
        path.append(curr)
        curr = ret[curr]
    path.append(curr)
    path.reverse()

    return path


"""
    distance estimate
    we gonna take manhattan estimate
    left/right updown from start to end

"""


def astar_corner(maze):
    """
    Runs A star for part 2 of the assignment in the case where there are four corner objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    start = maze.getStart()
    objectives = maze.getObjectives()
    totalPath = [start]
    path = []
    objDic = {}
    currObj = start
    while(len(objectives)!=0):
        obj = nearestDot(maze,objectives,start, path, objDic)
        objectives.remove(obj)if
        objDic[obj] = currObj
        currObj = obj

        blocksMoved = dict([])
        ret = dict([])
        #obj = obj[0]
        milage = 0
        visited = []
        path=[]
        curr = start
        visited.append(start)
        toGoal = abs(obj[0]-start[0]) + abs(obj[1]-start[1])
        frontier = queue.PriorityQueue() #get and put
        frontier.put((toGoal, (start)))
        blocksMoved[start] = 0


        while (curr != obj) and (len(visited) != 0):
            if(curr not in visited):
                visited.append(curr)
            curr = frontier.get()[1]
             #removes item from queue and adds it to visited
            neighbors = maze.getNeighbors(curr[0],curr[1])
            while(len(neighbors) != 0):
                neighbor = neighbors.pop()
                if neighbor not in visited:
                    milage = blocksMoved[curr] + 1
                    temp = abs(obj[0]-neighbor[0]) + abs(obj[1]-neighbor[1]) + milage
                    frontier.put((temp,(neighbor)))
                    blocksMoved[neighbor] = milage+1
                    ret[neighbor] = curr
                    visited.append(neighbor)

            milage += 1

        while curr != start:
            path.append(curr)
            curr = ret[curr]
        #path.append(curr)
        path.reverse()
        totalPath += path #there would be some overlap between start states and obj states
        start = obj

    #print(totalPath)
    return totalPath
def nearestDot(maze,objectives,start):
    objQueue = queue.PriorityQueue()


    for i in objectives:
        dist = abs(i[0]-start[0]) + abs(i[1]-start[1])
        objQueue.put((dist,(i)))
    return objQueue

def astar_multi(maze):
    """
    Runs A star for part 3 of the assignment in the case where there are
    multiple objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    blocksMoved = dict([])
    ret = dict([])
    objectives = maze.getObjectives()

    milage = 0
    visited = []
    path=[]
    totalPath = []
    pathDict = {}
    start = maze.getStart()
    visitedPath = []
    curr = start
    visited.append(start)
    toGoal = abs(obj[0]-start[0]) + abs(obj[1]-start[1])
    frontier = queue.PriorityQueue() #get and put
    frontier.put((toGoal, (start)))
    blocksMoved[start] = 0
    pathQueue = queue.PriorityQueue()

    for(i in objectives):
        objQueue = nearestDot(maze, objectives,start,)
        #obj = objQueue.get()[1]
        #ojectives.remove(nearest)
        totalPath = [start]
        while(objQueue.isEmpty()!=true):
            obj = objQueue.get()[1]


            while (curr != obj) and (len(visited) != 0):
                if(curr not in visited):
                    visited.append(curr)
                curr = frontier.get()[1]
                 #removes item from queue and adds it to visited
                neighbors = maze.getNeighbors(curr[0],curr[1])
                while(len(neighbors) != 0):
                    neighbor = neighbors.pop()
                    if neighbor not in visited:
                        milage = blocksMoved[curr] + 1
                        blocksMoved[neighbor] = blocksMoved[curr] + 1
                        temp = abs(obj[0]-neighbor[0]) + abs(obj[1]-neighbor[1]) + blocksMoved[neighbor]
                        frontier.put((temp,(neighbor)))
                        ret[neighbor] = curr
                        visited.append(neighbor)


                while curr != start:
                    path.append(curr)
                    curr = ret[curr]
                path.append(curr)
                path.reverse()
                totalPath += path
            pathDict[obj] = totalPath
            visitedPath.append(obj)
        return path


        #print(totalPath)
        return totalPath

def fast(maze):
    """
    Runs suboptimal search algorithm for part 4.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    # TODO: Write your code here
    return []
