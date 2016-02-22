#!/usr/bin/python
# Copyright 2010 Google Inc.
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

# Google's Python Class
# http://code.google.com/edu/languages/google-python-class/

import sys
import re
import os
import shutil
import commands

"""Copy Special exercise
"""

# +++your code here+++
# Write functions and modify main() to call them
def display(dir):
	
	filenames = os.listdir(dir)
	print filenames
	for filename in filenames:
		match = re.search('__\w+__',filename)
		if match:
			path = os.path.join(dir, filename)
			print os.path.abspath(path)

def to_dir(todir, dir):
	
	os.mkdir(todir)
	os.chmod(todir, 0777)
	filenames = os.listdir(dir)
	print filenames
	for filename in filenames:
		match = re.search('__\w+__',filename)
		if match:
			path = os.path.join(dir, filename)
			shutil.copy(path,todir)
def main():
  # This basic command line argument parsing code is provided.
  # Add code to call your functions below.
	
  # Make a list of command line arguments, omitting the [0] element
  # which is the script itself.
  args = sys.argv[1:]
  if not args:
    print "usage: [--todir dir][--tozip zipfile] dir [dir ...]";
    sys.exit(1)

  # todir and tozip are either set from command line
  # or left as the empty string.
  # The args array is left just containing the dirs.
  todir = ''
  if args[0] == '--todir':
    todir = args[1]
    to_dir(todir, args[2])
    sys.exit(1)
  tozip = ''
  if args[0] == '--tozip':
    tozip = args[1]
    to_zip(tozip, args[2])
    sys.exit(1)
  if len(args) == 0:
    print "error: must specify one or more dirs"
    sys.exit(1)

  # +++your code here+++
  # Call your functions
    display(args[0])
if __name__ == "__main__":
  main()
