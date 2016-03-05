#!/usr/bin/env python3
# -*- coding:utf-8 -*-

''' my web index handler...'''

__author__ = 'chan'

import logging;logging.basicConfig(level=logging.INFO)
# import functools
from web_core import get


@get(r'/')
def index(**kw):
	print('index path parameter....', kw)
	return '<h1>awesome pthon framework~~ ao....</h1>'



