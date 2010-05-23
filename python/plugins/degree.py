import warnings
warnings.filterwarnings('ignore')

import numpy
import pylab

data = numpy.loadtxt('/tmp/input.txt')

pylab.figure(figsize=(6, 6))
pylab.xlabel('Degree')
pylab.ylabel('Number of Nodes')
pylab.title('Node Degree Distribution')
pylab.hist(data, len(data))

pylab.savefig('/tmp/output.png')
