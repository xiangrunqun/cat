#!/usr/bin/env python3
# -*- coding:utf-8 -*-

''' this is a web app...'''

__author__ = 'chan'


import logging;logging.basicConfig(level=logging.INFO)
import asyncio,os,json,time,functools
from aiohttp import web
from web_core import add_routes


@asyncio.coroutine
def init(loop):
	app = web.Application(loop=loop,
		middlewares=[logger_factory,response_factory])
	add_routes(app,'index')
	srv = yield from loop.create_server(app.make_handler(), '127.0.0.1',
		9000)
	logging.info('server started at http://127.0.0.1:9000...')
	return srv



@asyncio.coroutine
def logger_factory(app, handler):
	@asyncio.coroutine
	def logger(request):
		# logging request info....
		logging.info('logger factory Request: %s %s' % 
			(request.method, request.path))
		# continue to handle request.....
		return (yield from handler(request))
	return logger


@asyncio.coroutine
def response_factory(app, handler):
	@asyncio.coroutine
	def response(request):
		logging.info('response factory....' )
		# handling result of response.....
		r = yield from handler(request)
		if isinstance(r, web.StreamResponse):
			return r
		if isinstance(r, bytes):
			resp = web.Response(body=r)
			resp.content_type = 'application/octet-stream'
			return resp
		if isinstance(r, str):
			resp = web.Response(body=r.encode('utf-8'))
			resp.content_type = 'text/html;charset=utf-8'
			return resp
	return response


loop = asyncio.get_event_loop()
loop.run_until_complete(init(loop))
loop.run_forever()

