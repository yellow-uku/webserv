#!/usr/bin/python
import os

for param in os.environ.keys():
   print "<b>%20s</b>: %s</br>" % (param, os.environ[param])