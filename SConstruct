
import os, sys
platform = sys.platform

env = Environment(ENV = {'PATH': os.environ['PATH']})
#env = Environment(CPPPATH='src')

flags = '-Wall -pedantic -g'
libs = ['ncurses']
src = Glob('*.c')

env.Append(LIBPATH = ['.'])
env.Append(LIBS = libs)
env.Append(CPPFLAGS=flags)

env.Program('sandworld', src) 