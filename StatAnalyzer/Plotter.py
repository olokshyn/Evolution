import sys
import matplotlib.pyplot as plt


def BuildHistogram(occurrences):
    items = occurrences.items()
    items.sort(key=lambda x: x[0])
    if items and items[0][0] == 0:
        items.pop(0)
    plt.figure(1)
    plt.bar([x[0] for x in items], [x[1] for x in items], align="center")


def BuildPlot(data):
    data_list = list(data)
    plt.figure(2)
    plt.plot(data_list)


def ShowPlot():
    plt.show()
