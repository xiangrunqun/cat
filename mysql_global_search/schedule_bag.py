#!/usr/local/bin/python3

'''
scheduling by system at midnigth...
# 0 0 * * * /usr/local/bin/python3 /home/bag/res/conf/schedule_bag.py
'''

import logging;logging.basicConfig(level=logging.INFO,filename='/home/bag/res/conf/cron.log')
from urllib import request
from datetime import datetime
import MySQLdb,os,os.path,shutil

#logging.info('schedule start :%s',datetime.now())

user,passwd,database,template_dir = 'user_db','passwd_db','db','/home/bag/bak/'

# scheduling task for updating disk space and testing time expired of corp....
def duang():
	f = request.urlopen('http://bag.89mc.com/basic/api/task/zero')
	data = f.read()
	#logging.info('request url data:%s',data)

def filter_dir(dir_name):
	should_filter = dir_name in 'conf html_template proxy_temp temporary'
	#print(should_filter)
	return should_filter


def test_res_in_db(dir_name):
	''' test resource in database...'''
	# connect db ....
	
	conn = MySQLdb.connect(user=user,passwd=passwd,db=database)
	#print('dir_name:', dir_name)
    # test res if dir is a file...
	if os.path.isfile(dir_name):
		c = conn.cursor()
		file_name = dir_name[3:]
		#print('file_name:', file_name)
		c.callproc('test_res_in_db',(file_name,))
		ret = c.fetchone()
		if not ret[0]:
			shutil.move(dir_name,template_dir+file_name)

		c.close()
	
	elif os.path.isdir(dir_name):
		parent_dir = dir_name
		list_dir = os.listdir(parent_dir)
		for dir in list_dir:
			dir_path = parent_dir +'/'+ dir
            #print('test res:',dir_name,dir)
			if os.path.isdir(dir_path):
				test_res_in_db(dir_path)
			elif os.path.isfile(dir_path):
				c = conn.cursor()
				c.callproc('test_res_in_db',(dir,))
				ret = c.fetchone()
				# move file to template dir if not exists...
				if not ret[0]:
					shutil.move(dir_path,template_dir+dir)

				c.close()
	
	# colse db conection....
	conn.close()

	  


def clear_res():
	# conf html_template proxy_temp temporary:cannot delete...
	list_root = os.listdir('..')

	for x in list_root:
		#print('dir', x)
		# test res if not in the filter list....
		if not filter_dir(x):
			test_res_in_db('../'+x)		

''' '''
		
if __name__ == '__main__':
	try:
		duang()
	except Exception as e:
		logging.error('duang...error', e)
	try:
		clear_res()
	except Exception as e:
		logging.error('clearing res error...', e)


	#filter_dir('confo')


''' '''

