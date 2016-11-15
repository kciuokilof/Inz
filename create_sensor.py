import os,sys,shutil

if len(sys.argv) != 2:
    print 'usage : create_sensor.py <number> \nYou must specify the path to the origin file as the first arg'
    sys.exit(1)
    
    
for x in xrange(1, int(sys.argv[1])):
		
	print 'Creating sensor %d parameters' % (x)
	path='sensor_'  +(str(x))
	shutil.rmtree(path)
	os.mkdir(path,0777)
	shutil.copyfile('/home/kamil/Desktop/inz/Inz-kciuokilof-patch-2/sensor/cli', path+'/cli')
	


        
