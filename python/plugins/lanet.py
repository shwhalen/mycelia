import os

lanetDir = '/Users/sean/Code/mycelia/lanet-vi'
povrayDir = '/opt/local/bin'
width = 1024
height = 1024

os.chdir('/tmp')
os.environ['PATH'] = '%s:%s' % (lanetDir, povrayDir)
os.system('lanet -input input.txt -W %s -H %s' % (width, height))

os.remove('input_col_b_%sx%s.pov' % (width, height))
os.rename('input_col_b_%sx%sPOV.png' % (width, height), 'output.png')
