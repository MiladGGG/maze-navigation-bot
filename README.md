# Maze Navigation Bot | C++ and Arduino

This repository contains some of the C++ logic used in a maze navigation bot. Worked on as a group project for a univeristy task.
Bot uses Arduino hardware. This includes 2 servos(used as wheels) and 3 IR sensors(front, left and right). The bot's name was agreed to be Cornelius.
The task was to write code in order for the robot traverse a maze as fast as possible. The bot was NOT allowed to make contact with any walls and had to detect when it reached the end.

Placed 2nd place out of 200+ teams in university competition.


## Bot in Action
![Demo](demo1.gif)



## Unique Functionality
- **Smooth steering**: Does smooth turns rather than 90° turns, resulting in a large advantage
- **Straight within hallways**: Bot constantly tries to remain parallel with walls in long hallways
- **Zooms through corners**: Delays and flags within the code enable the bot to efficently traverse corners

