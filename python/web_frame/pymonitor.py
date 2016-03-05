#!/usr/bin/env python3
# -*- coding:utf-8 -*-

''' my web index handler...'''

__author__ = 'chan'

import os,sys,time,subprocess

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

def log(s):
	print('[Monitor] %s'%s)

class MyFileSystemEvventHandler(FileSystemEventHandler):

	def __init__(self,fn):
		super(MyFileSystemEvventHandler,self).__init__()
		self.restart = fn

	def on_any_event(self,event):
		if event.src_path.endswith('.py'):
			log('Python source file change:%s' % event.src_path)
			self.restart()


command = ['echo','ok']

process = None

def kill_process():
	global process
	if process:
		log('Kill process [%s]...' % process.pid)
		process.kill()
		process.wait()
		log('Porcess ended with code %s.'%process.returncode)
		process = None

def start_porcess():
	global process,command
	log('Start process %s...' % ' '.join(command))
	process = subprocess.Popen(command,stdin=sys.stdin,stdout=sys.stdout,
		stderr=sys.stderr)

def restart_process():
	kill_process()
	start_porcess()


def start_watch(path,callback):
	observer = Observer()
	observer.schedule(MyFileSystemEvventHandler(restart_process),path,
		recursive=True)
	observer.start()
	log('Watching directory %s...' % path)
	start_porcess()
	try:
		while 1:
			time.sleep(0.5)
	except KeyboardInterrupt:
		observer.stop()
	observer.join()

if __name__ == '__main__':
	argv = sys.argv[1:]
	if not argv:
		print('Usage: ./pymonitor your-script.py')
		exit(0)
	if argv[0] != 'python3':
		argv.insert(0,'python3')
	command = argv
	path = os.path.abspath('.')
	start_watch(path, None)


















