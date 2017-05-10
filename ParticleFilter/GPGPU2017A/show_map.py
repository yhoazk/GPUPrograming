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

pf_x = []
pf_y = []
pf_t = []

gt_data = {'x':gt_x, 'y':gt_y, 'th':gt_t}
def main():
    with open("data/map_data.txt", "r") as data:
        for line in data:
            x,y, m_id = line.split("	")
            m_id = int(m_id)
            x = float(x)
            y = float(y)
            x_p.append(x)
            y_p.append(y)
            map_data[m_id] = [x,y]
          
    with open("pf_output.txt", 'r') as pf_file:
        for line in pf_file:
            x,y,t = line.split(" ")#map(float, line.split("	"))
            pf_x.append(x)
            pf_y.append(y)
            pf_t.append(t)
    sizes_pf = (np.random.randn(len(pf_x)) * 2) ** 2
    sizes_mk = 50 * np.ones(len(x_p))

    with open("data/gt_data.txt", 'r') as gt_file:
        for line in gt_file:
            x,y,t = line.split(" ")#map(float, line.split("	"))
            gt_x.append(x)
            gt_y.append(y)
            gt_t.append(t)
    sizes_gt = (np.random.randn(len(gt_x)) * 2) ** 2
    sizes_mk = 50 * np.ones(len(x_p))

    plt.scatter(x_p,y_p, s=sizes_mk)
    plt.scatter(pf_x,pf_y, s=sizes_pf)
    plt.scatter(gt_x,gt_y, s=sizes_gt)
    """
    for n in range(len(gt_x)-1):
        a = plt.arrow(gt_x[n], gt_y[n], gt_x[n+1], gt_y[n+1], width=0.1,zorder=1)
        a.set_facecolor('0.7')
        a.set_edgecolor('w')
    """
    plt.grid('on')
    plt.show()


if __name__ == "__main__":
    main()
