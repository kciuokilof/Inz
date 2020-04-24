import os,sys,shutil
from random import randint

if len(sys.argv) != 2:
    print 'usage : create_sensor.py <number> \nYou must specify the path to the origin file as the first arg'
    sys.exit(1)
    
    
for x in xrange(1, int(sys.argv[1])):
		
	print 'Creating sensor %d parameters' % (x)
	path='sensor_'  +(str(x))
	if os.path.exists(path):
		shutil.rmtree(path)
	os.mkdir(path,0777)
	shutil.copyfile('/home/kamil/Desktop/inz/Inz-kciuokilof-patch-2/sensor/cli', path+'/cli')
	open(path+'/ininum.txt','w+')
	f1=open(path+'/sensor.conf','w+')
	f2=open('/home/kamil/Desktop/inz/Inz-kciuokilof-patch-2/CLOUD/passwd','a')
	rnd1=randint(0,2147483647)
	rnd2=randint(0,2147483647)
	rnd3=randint(0,2147483647)
	rnd4=randint(0,2147483647)
	buff=str(x)+','+str(rnd1)+','+str(rnd2)+','+str(rnd3)+','+str(rnd4)
	f1.write(buff)
	f2.write(buff+'\n')
