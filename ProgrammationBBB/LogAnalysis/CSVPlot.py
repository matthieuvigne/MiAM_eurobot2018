# Plots curves from input csv file
import sys
import numpy as np
import matplotlib.pyplot as plt
import csv
import inspect

def doPlot(source, plotNames, hasTime = False, commonY = False):
	data = np.genfromtxt(source, delimiter=',', skip_header = 2)
	fig, axs = plt.subplots(nrows=len(plotNames),sharex=True,sharey=commonY)

	if len(plotNames) == 1:
		axs = [axs]

	f = open(source, "rb")
	reader = csv.reader(f)
	headers = reader.next()
	headers = reader.next()
	f.close()

	for i in range(0,len(plotNames)):
		desiredData = plotNames[i].split(':')
		for j in range(0,len(desiredData)):
			for k in range(0,len(headers)):
				if(headers[k] == desiredData[j]):
					if hasTime:
						axs[i].plot(data[:,0],data[:,k], label=desiredData[j])
					else:
						axs[i].plot(data[:,k], label=desiredData[j])

	for ax in axs:
		ax.legend(bbox_to_anchor=(1.0, 1.0), loc = 9)
	if(hasTime):
		plt.xlabel(r'Time')
	else:
		plt.xlabel(r'Samples')
	fig.show()
	raw_input('Exit')

if __name__ == "__main__":
	source = sys.argv[1]
	data = sys.argv[2:len(sys.argv)]
	if data[0] == 'TIME':
		doPlot(source, data[1:], True)
	elif data[0] == 'TIME:Y':
		doPlot(source, data[1:], True,True)
	elif data[0] == 'Y':
		doPlot(source, data[1:], False,True)
	else:
		doPlot(source, data)
