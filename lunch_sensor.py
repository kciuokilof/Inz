import subprocess,sys,os
from subprocess import call
#Or just:
#args = "bin/bar -c somefile.xml -d text.txt -r aString -f anotherString".split()


if len(sys.argv) != 2:
    print 'usage : lunch_sensors.py <number> \nYou must specify the path to the origin file as the first arg'
    sys.exit(1)
os.chdir("sensor_"+sys.argv[1])
popen = subprocess.call(["./cli",'::1','1'])
f1 = open("ininum.txt",'rb')
l=f1.read(1024)
ttl=l.split(',',1)[0]
print '\n'+ttl+'\n'

for x in xrange(1, int(ttl)):
	popen = subprocess.call(["./cli",'::1','0'])


