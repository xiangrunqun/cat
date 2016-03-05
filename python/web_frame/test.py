#!/usr/bin/env python3
# -*- coding:utf-8 -*-

''' test...'''

__author__ = 'chan'

# import functools 

# def log(text):
# 	def decorator(func):
# 		@functools.wraps(func)
# 		def wrapper(*args, **kw):
# 			print('call %s():' % func.__name__)
# 			return func(*args, **kw)
# 		wrapper.__method__ = 'get'
# 		return wrapper
# 	return decorator

# @log('why')
# def index():
# 	return '<h1>awesome pthon framework~~ ao....</h1>'


mod  = __import__('index', globals(), locals())
# mod.index()
for attr in dir(mod):
	if attr.startswith('_'):
		continue 
	fn = getattr(mod,attr)
	if callable(fn):
		method = getattr(fn, '__method__',None)
		path = getattr(fn, '__route__',None)
		print(fn,method,path)

# print(getattr(index,'__method__',None))
