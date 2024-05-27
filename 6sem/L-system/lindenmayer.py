import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

class Turtle3D:
    def __init__(self):
        self.position = np.array([0.0, 0.0, 0.0])
        self.heading = np.array([1.0, 0.0, 0.0])
        self.left = np.array([0.0, 1.0, 0.0])
        self.up = np.array([0.0, 0.0, 1.0])
        self.stack = []
        self.points = [self.position.copy()]

    def move_forward(self, distance):
        self.position += self.heading * distance
        self.points.append(self.position.copy())

    def rotate(self, axis, angle):
        angle = np.radians(angle)
        c, s = np.cos(angle), np.sin(angle)
        if axis == 'left':
            rotation_matrix = np.array([
                [1, 0, 0],
                [0, c, -s],
                [0, s, c]
            ])
            self.heading = np.dot(rotation_matrix, self.heading)
            self.up = np.dot(rotation_matrix, self.up)
        elif axis == 'up':
            rotation_matrix = np.array([
                [c, 0, s],
                [0, 1, 0],
                [-s, 0, c]
            ])
            self.heading = np.dot(rotation_matrix, self.heading)
            self.left = np.dot(rotation_matrix, self.left)
        elif axis == 'heading':
            rotation_matrix = np.array([
                [c, -s, 0],
                [s, c, 0],
                [0, 0, 1]
            ])
            self.left = np.dot(rotation_matrix, self.left)
            self.up = np.dot(rotation_matrix, self.up)

    def push_state(self):
        self.stack.append((self.position.copy(), self.heading.copy(), self.left.copy(), self.up.copy()))

    def pop_state(self):
        self.position, self.heading, self.left, self.up = self.stack.pop()

def l_system(axiom, rules, iterations):
    current = axiom
    for _ in range(iterations):
        next_string = ""
        for char in current:
            next_string += rules.get(char, char)
        current = next_string
    return current

def draw_l_system(turtle, instructions, length, angle):
    for command in instructions:
        if command == 'F':
            turtle.move_forward(length)
        elif command == '+':
            turtle.rotate('up', angle)
        elif command == '-':
            turtle.rotate('up', -angle)
        elif command == '&':
            turtle.rotate('left', angle)
        elif command == '^':
            turtle.rotate('left', -angle)
        elif command == '\\':
            turtle.rotate('heading', angle)
        elif command == '/':
            turtle.rotate('heading', -angle)
        elif command == '[':
            turtle.push_state()
        elif command == ']':
            turtle.pop_state()

# Пример 3D L-системы с дополнительными символами поворота
axiom = "X"
rules = {
    "X": "F-[[X]+X]&F[+FX]/X\\^X",
    "F": "FF"
}
iterations = 4
length = 1
angle = 25

instructions = l_system(axiom, rules, iterations)
turtle3d = Turtle3D()
draw_l_system(turtle3d, instructions, length, angle)

# Визуализация с использованием matplotlib
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
points = np.array(turtle3d.points)
ax.plot(points[:, 0], points[:, 1], points[:, 2], lw=1)
ax.set_xlim(-100, 100)
ax.set_ylim(-100, 100)
ax.set_zlim(-100, 100)

plt.show()
