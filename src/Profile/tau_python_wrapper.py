# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, ParaTools, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# (1) Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
# (2) Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
# (3) Neither the name of ParaTools, Inc. nor the names of its contributors may
#     be used to endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
"""TAU python instrumentation wrapper.

usage: python -m tau_python_wrapper MODULE

Runs MODULE with automatic Python instrumentation.
"""

from __future__ import print_function
import os
import sys

def dieInFlames(msg):
  print(msg, file=sys.stderr)
  sys.exit(-1)

try:
  modname = sys.argv[1]
except IndexError:
  dieInFlames('Usage: %s <modulename>' % sys.argv[0])

try:
  import tau
except ImportError:
  dieInFlames("module 'tau' not found in PYTHONPATH")
except:
  dieInFlames("Unknown exception while importing tau: %s" % sys.exc_info()[0])

if sys.argv[1] == '-c':
  command = sys.argv[2]
  del sys.argv[2]
  del sys.argv[0]
  tau.run(command)   
else:
  if os.path.exists(modname):
    path = os.path.dirname(modname)
    modname = os.path.basename(modname)
    if modname[-3:].lower() != '.py':
      dieInFlames("Sorry, I don't know how to instrument '%s'" % modname)
    modname = modname[:-3]
    sys.path.append(path)
    sys.argv = sys.argv[1:]
  
  tau.runmodule(modname)

