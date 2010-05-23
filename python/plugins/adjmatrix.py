import warnings
warnings.filterwarnings('ignore')

import numpy
import pylab

data = numpy.loadtxt('/tmp/input.txt')

pylab.figure(figsize=(6, 6))
pylab.xlabel('Destination Node')
pylab.ylabel('Source Node')
pylab.title('Adjacency Matrix')
pylab.pcolor(pylab.flipud(data), cmap=pylab.get_cmap('binary'))
pylab.xlim(0, len(data))
pylab.ylim(0, len(data))

pylab.savefig('/tmp/output.png')
