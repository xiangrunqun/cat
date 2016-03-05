#!/usr/bin/env python3
# -*- coding:utf-8 -*-

''' this is a web core handler....'''
__author__ = 'chan'


import logging;logging.basicConfig(level=logging.INFO)
import asyncio,os,json,time,inspect,functools


def get(path):
	''' Define decorator @get('/path') '''
	def decorator(func):
		@functools.wraps(func)
		def wrapper(*args, **kw):
			return func(*args,**kw)
		wrapper.__method__  = 'GET'
		wrapper.__route__ = path
		return wrapper
	return decorator


# request handling class...... u know what dose it mean....
class RequestHandler(object):
	def __init__(self, app, fn):
		self._app = app
		self._func = fn

	# extracting data from request .....
	@asyncio.coroutine
	def __call__(self, request):
		# logging.info('url pararmeter:',request.match_info['bb'])
		logging.info('query string:',request.query_string)
		kw = {'kw':'this is a kw value....'}
		r = yield from self._func(**kw)
		return r

# binding request to method....
def add_route(app, fn):
	method = getattr(fn,'__method__', None)
	path = getattr(fn,'__route__', None)
	if path is None or method is None:
		raise ValueError('@get not defined in %s.'%str(fn))
	if not asyncio.iscoroutinefunction(fn) \
		and not inspect.isgeneratorfunction(fn):
		fn = asyncio.coroutine(fn)
	logging.info('add route %s %s => %s(%s)'%(method,path,fn.__name__,
		','.join(inspect.signature(fn).parameters.keys())))
	app.router.add_route(method, path, RequestHandler(app,fn))


# auto binding...
def add_routes(app, module_name):
	n = module_name.rfind('.')
	if n == (-1):
		mod  = __import__(module_name, globals(), locals())
	else:
		name = module_name[n+1:]
		mod = getattr(__import__(module_name[:n],globals(),locals(),[name]),
			name)
	for attr in dir(mod):
		if attr.startswith('_'):
			continue 
		fn = getattr(mod,attr)
		if callable(fn):
			method = getattr(fn, '__method__',None)
			path = getattr(fn, '__route__',None)
			if method and path:
				add_route(app, fn)



