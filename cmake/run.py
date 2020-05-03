#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.dont_write_bytecode = True
import argparse
import os
import subprocess

scriptPath = os.path.dirname(__file__)

# ------------------------------------------------------------------------------
if __name__ == '__main__':

	parser = argparse.ArgumentParser()
	parser.add_argument('--platform'   , action="store", required=True)
	parser.add_argument('--build-path'   , action="store", required=True)
	parser.add_argument('--source-path'   , action="store", required=True)
	args = parser.parse_args()

	cmakeCmd = ""

	sourcePath = args.source_path
	buildPath = args.build_path

	if not os.path.exists(buildPath):
		os.makedirs(buildPath)
	os.chdir(buildPath)
	
	platformStr = args.platform.upper()
	if platformStr == 'MACOSX_XCODE':
		generator = 'Xcode'
		cmakeCmd = ['cmake', '-G', generator, sourcePath, '-DMACOSX=TRUE', '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.9', '-DCMAKE_OSX_SYSROOT=macosx']
	elif platformStr == 'WIN_VS':
		generator = 'Visual Studio 15 2017'
		cmakeCmd = ['cmake', '-G', generator, sourcePath, '-A', 'x64']
	else:
		print("Unknown platform " + args.platform)

	print('Command: ' + str(cmakeCmd))
	subprocess.check_call(cmakeCmd, stderr=subprocess.STDOUT)