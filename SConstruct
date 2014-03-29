###############################################################################
##
##  Copyright (C) 2014 Tavendo GmbH
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http:##www.apache.org#licenses#LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.
##
###############################################################################

import os

env = Environment(ENV = os.environ)

USE_SHIT_COMPILER = False

if USE_SHIT_COMPILER:
   env["CC"] = "gcc"
   env["CXX"] = "g++"
   env.Append(CXXFLAGS = ['-std=c++11', '-Wall', '-Wno-deprecated-declarations', '-Wl,--no-as-needed', '-pthread'])
else:
#   env.Append(CXXFLAGS = ['-g', '-std=c++1y', '-stdlib=libc++', '-Wall', '-Wno-deprecated-declarations', '-Wno-unused-value', '-pthread'])
   env.Append(CXXFLAGS = ['-std=c++11', '-stdlib=libc++', '-Wall', '-pedantic', '-pthread'])
#   env.Append(CXXFLAGS = ['-std=c++11', '-stdlib=libc++', '-Wall', '-Wno-deprecated-declarations', '-Wno-unused-value', '-pthread'])
#   env.Append(CXXFLAGS = ['-stdlib=libc++', '-Wall', '-Wno-deprecated-declarations', '-pthread'])
   env.Append(LINKFLAGS = ['-stdlib=libc++', '-pthread'])
   env["CC"] = "clang"
   env["CXX"] = "clang++"

# -std=c++11 -stdlib=libc++
#

if False:
   env.Append(CPPPATH = ['/home/oberstet/boost_1_55_0/'])
   env.Append(LIBPATH = ['/home/oberstet/boost_1_55_0/stage/lib'])
else:
   env.Append(CPPPATH = ['/home/oberstet/boost/'])
   env.Append(LIBPATH = ['/home/oberstet/boost/stage/lib'])

env.Append(CPPPATH = ['#/autobahn'])
env.Append(CPPPATH = [os.path.join(os.environ['HOME'], 'msgpack_clang/include')])
env.Append(LIBPATH = [os.path.join(os.environ['HOME'], 'msgpack_clang/lib')])
#env.Append(LIBPATH = ['/usr/lib/x86_64-linux-gnu/'])

Export('env')

#autobahn = SConscript('src/SConscript', variant_dir = 'build/src', duplicate = 0)

#Export('autobahn')

#tests = SConscript('test/SConscript', variant_dir = 'build/test', duplicate = 0)
examples = SConscript('examples/SConscript', variant_dir = 'build/examples', duplicate = 0)
