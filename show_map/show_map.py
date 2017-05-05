#!/usr/bin/env python


import matplotlib.pyplot as plt
import numpy as np
import os

map_data = {}
x_p = []
y_p = []

gt_x = []
gt_y = []
gt_t = []
gt_data = {'x':gt_x, 'y':gt_y, 'th':gt_t}
def main():
    with open("map_data.txt", "r") as data:
        for line in data:
            x,y, m_id = line.split("	")
            m_id = int(m_id)
            x = float(x)
            y = float(y)
            x_p.append(x)
            y_p.append(y)
            map_data[m_id] = [x,y]
          
    with open("gt_data.txt", 'r') as gt_file:
        for line in gt_file:
            x,y,t =map(float, line.split(" "))
            gt_x.append(x)
            gt_y.append(y)
            gt_t.append(t)
    
    plt.scatter(x_p,y_p)
    plt.scatter(gt_x,gt_y)
    """
    for n in range(len(gt_x)-1):
        a = plt.arrow(gt_x[n], gt_y[n], gt_x[n+1], gt_y[n+1], width=0.1,zorder=1)
        a.set_facecolor('0.7')
        a.set_edgecolor('w')
    """
    plt.grid('on')
    plt.show()
    print(map_data.keys())


if __name__ == "__main__":
    main()
