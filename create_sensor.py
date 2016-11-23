import os,sys,shutil
from random import randint

if len(sys.argv) != 2:
    print 'usage : create_sensor.py <number> \nYou must specify the path to the origin file as the first arg'
    sys.exit(1)
CentraUnitID=10  
pth=os.getcwd()
f2=open(pth+'/CLOUD/passwd/'+str(CentraUnitID),'w+')  
f2.write(str(int(sys.argv[1])-1)+'\n')
k=pth+'/CentralUnit/passwd'
f3=open(k,'w+')
f3.seek(0)
f3.truncate()
for x in xrange(1, int(sys.argv[1])):
	print 'Creating sensor %d parameters' % (x)
	path='sensor_'  +(str(x))
	 
	if os.path.exists(path):
		shutil.rmtree(path)
	os.mkdir(path,0777)
	shutil.copyfile(pth+'/sensor/cli', path+'/cli')
	os.chmod(path+"/cli",0o777)
	open(path+'/ininum.txt','w+')
	
	f1=open(path+'/sensor.conf','w+')
	rnd1=randint(0,2147483647)
	rnd2=randint(0,2147483647)
	rnd3=randint(0,2147483647)
	rnd4=randint(0,2147483647)
	buff=str(x)+','+str(rnd1)+','+str(rnd2)+','+str(rnd3)+','+str(rnd4)
	f1.write(buff)
	f2.write(buff+'\n')
