import numpy as np
import utils
import random
import copy

class Agent:

    def __init__(self, actions, Ne, C, gamma):
        self.actions = actions
        self.Ne = Ne # used in exploration function
        self.C = C
        self.gamma = gamma

        # Create the Q and N Table to work with
        self.Q = utils.create_q_table()
        self.N = utils.create_q_table()
        self.points = 0
        self.s = None
        self.a = None
        #initialize s a and points so that they can be used

    def train(self):
        self._train = True

    def eval(self):
        self._train = False

    # At the end of training save the trained model
    def save_model(self,model_path):
        utils.save(model_path, self.Q)
        utils.save(model_path.replace('.npy', '_N.npy'), self.N)

    # Load the trained model for evaluation
    def load_model(self,model_path):
        self.Q = utils.load(model_path)

    def reset(self):
        self.points = 0
        self.s = None
        self.a = None

    def discretize(self, state):
        snake_head_x, snake_head_y, snake_body, food_x, food_y = state
        #initialize the state variables so that they can be used later
        c = 0
        if food_x == snake_head_x: # if food_x is the same as snakeheadx then that means that its in the same horiz line
            food_dir_x = 0
        else:
            if food_x > snake_head_x: #if foodx is greater then its on the right
                food_dir_x = 2
            else:
                if food_x < snake_head_x:
                    food_dir_x = 1  #if its less then its on the left
        if food_y == snake_head_y:
            food_dir_y = 0
        else:
            if food_y < snake_head_y:
                food_dir_y = 1
            else:
                if food_y > snake_head_y:
                    food_dir_y = 2
        if snake_head_x >= 40 and snake_head_y >= 40 and snake_head_x <= 520 and snake_head_y <= 520:
            if snake_head_x == 480:
                adjoining_wall_x = 2
            if snake_head_x == 40:
                adjoining_wall_x = 1
            if snake_head_x != 40 and snake_head_x != 480:
                adjoining_wall_x = 0
            #top checks if there is a wall adjacent to the left or right of the snake head
            #bot checks if ther is a wall on the adjacent below or above the snake head
            if snake_head_y == 480:
                adjoining_wall_y = 2
            if snake_head_y == 40:
                adjoining_wall_y = 1
            if snake_head_y != 40 and snake_head_y != 480:
                adjoining_wall_y = 0
        else:
            #if there is not any adjoining walls then both are 0s
            adjoining_wall_x = 0
            adjoining_wall_y = 0
        #this next for loop checks to see if any of the snake body parts are adjacent to the head
        top,bot,left,right = [0,0,0,0]
        for bodylicious in snake_body:
            x = bodylicious[0]
            y = bodylicious[1]
            #taking the x and y coordinates of the current body part
            if x == snake_head_x:
                if y == 40 + snake_head_y:
                    bot = 1
                else:
                    if y + 40 == snake_head_y:
                        top = 1

            if y == snake_head_y:
                if x - 40 == snake_head_x:
                    right = 1
                else:
                    if x + 40 == snake_head_x:
                        left = 1
        #return all the variables that were initialized for next s
        return [adjoining_wall_x,adjoining_wall_y,food_dir_x,food_dir_y,top,bot,left,right]

    def act(self, state, points, dead):
        '''
        :param state: a list of [snake_head_x, snake_head_y, snake_body, food_x, food_y] from environment.
        :param points: float, the current points from environment
        :param dead: boolean, if the snake is dead
        :return: the index of action. 0,1,2,3 indicates up,down,left,right separately

        TODO: write your function here.
        Return the index of action the snake needs to take, according to the state and points known from environment.
        Tips: you need to discretize the state to the state space defined on the webpage first.
        (Note that [snake_head_x=0, snake_head_y=0] is also the case when snake runs out of the 480x480 board)

        '''
        dirs = [3,2,1,0]
        nextS = self.discretize(state)
        #@@@check that discretize is returning the correct stato
        #print(nextS)
        if self.s is not None:
            #in order to start i have to check if there is a given state or action otherwise i cant discretize self.s
            vars = tuple(self.discretize(self.s) + [self.a])
            tempVar = self.C + self.N[vars]
            aa = self.C / tempVar
            #setting up the alpha value that is used for the bellman equation
            temp1 = points - self.points
            stuff = -.1
            if temp1 > 0:
                stuff = 1
            else:
                if dead:
                    stuff = -1
            #@@@check
            bigQ = -2
            for i in dirs:
                pair = tuple(nextS + [i])
                if self.Q[pair] > bigQ:
                    bigQ = self.Q[pair]
                #print(self.Q[pair])
            #update the Q table with the value outputted by the bellman equation
            self.Q[vars] += self.C/(self.C + self.N[vars]) * (stuff + self.gamma * bigQ - self.Q[vars])
        #if its dead then simply reset and byebye
        if dead:
            self.reset()
            return 0
        #@@@make sure that dead check is in the right position.
        self.s = tuple(state)
        self.points = points
        #setting the "next" state points and state to be used in next run
        estado,valor = 0,0
        #estado is the action and valor is its value
        for aa in dirs:
            tempVars = tuple(nextS+[aa])
            #creating a copy of nextS that has the additional action at the end of the list/tuple
            if estado == 0:
                estado = aa
                if self.Ne > self.N[tempVars]:
                    valor = 1
                else:
                    valor = self.Q[tempVars]
            #setting the binbin variable to compare with valor
            if self._train and self.Ne > self.N[tempVars]:
                binbin= 1
            else:
                binbin = self.Q[tempVars]
            #comparison between binbin and valor to find the biggest Q with the 4 possible directions
            if binbin > valor:
                estado = aa
                valor = binbin
        #print(points)
        nextS.append(estado)
        newPair = tuple(nextS)
        self.N[newPair] += 1
        #setting the next state action pair so that it can be used in the next run.
        self.a = estado
        #saving
        return self.actions[estado]
        #odio esta clase D:
