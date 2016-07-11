#!/usr/bin/python 

from api import *

TEMPLATE_DIR = "templates"
INO_DIR = "rpnled"
WEB_DIR = "web"
CONTROL_DIR = "control"

cpplines = (l for l in file(TEMPLATE_DIR + "/commands.cpp.templ").readlines())
hpplines = (l for l in file(TEMPLATE_DIR + "/commands.h.templ").readlines())
htmllines = (l for l in file(TEMPLATE_DIR + "/index.html.templ").readlines())
jslines = (l for l in file(TEMPLATE_DIR + "/runcmd.js.templ").readlines())

with open(INO_DIR + "/commands.h", "w") as hpp, \
  open(INO_DIR + "/commands.cpp", "w") as cpp, \
  open(CONTROL_DIR + "/commands.py", "w") as py, \
  open(WEB_DIR + "/const.js", "w") as js, \
  open(WEB_DIR + "/runcmd.js", "w") as jsrun, \
  open(WEB_DIR + "/index.html", "w") as html:

      for l in hpplines:
        if "CONST" in l:
          break
        hpp.write(l)

      for l in cpplines:
        if "CONST" in l:
          break
        cpp.write(l)

      for l in htmllines:
        if "CONST" in l:
          break
        html.write(l)

      for l in jslines:
        if "CONST" in l:
          break
        jsrun.write(l)

      val = 0

      for op in operators:
        val -= 1
        if op is not None:
          c = "C_" + op[0]

          pop, push = op[2]
          cmd = op[3]
          cmd = cmd.replace("#3", "stack[stack_ptr-3]")
          cmd = cmd.replace("#2", "stack[stack_ptr-2]")
          cmd = cmd.replace("#1", "stack[stack_ptr-1]")
          cmd = cmd.replace("#0", "stack[stack_ptr]")

          case = "      case " + c + ": " 
          case += push > pop and "PUSH(" or "POP("
          case += repr(max(push, pop)) + "); "
          case += cmd
          if push != pop: 
            case += " stack_ptr -= %d;" % (pop - push)
          case += " break;\n"

          cpp.write( case )
          jsrun.write( case )
          hpp.write( "#define " + c + " " + repr(val) + "\n")
          py.write( c + " = " + repr(val) + "\n")
          js.write( "var " + c + " = " + repr(val) + ";\n")
          #html.write( "<button id=go onclick=\"addcmd("+c+")\">" + c + "</button><br>\n")
          html.write( c + "<br>\n")
        else:
          html.write( "<br>\n")
          cpp.write("\n")
          hpp.write("\n")
          py.write("\n")
          js.write("\n")

        if not (val%29):
          html.write("</td><td>")

        '''
        else:
          while val % 10:
            val -= 1
        '''

      for i, (c, s) in enumerate(other):
        hpp.write( "#define " + c + " " + s + "\n")
        js.write( "var " + c + " = " + s + ";\n")

      for l in hpplines:
        hpp.write(l)

      for l in cpplines:
        cpp.write(l)

      for l in htmllines:
        html.write(l)

      for l in jslines:
        jsrun.write(l)

import re
plines = file(CONTROL_DIR + "/patterns.py").readlines()
start = re.compile("^([a-zA-Z0-9_]+) = \[\s*$");
end = re.compile("^]")
name = None
patt = ""
allnames = []

with open(WEB_DIR + "/patterns.js", "w") as js:
  for l in plines:
    if name is not None:
      result = end.match(l)
      if result:
        #print name + " : " + patt
        js.write("var code_" + name + " = \"" + patt + "\";\n")
        allnames.append(name)
        name = None
        patt = ""
      else:
        patt += l.rstrip() + "\\n"
    else:
      result = start.match(l)
      if result:
        name = result.group(1)
  js.write("var code_patterns = {\n")
  for name in allnames:
    js.write(name + " : code_" + name + ",\n")
  js.write("}")

import os
os.chdir(WEB_DIR)
os.system("uglifyjs  const.js patterns.js runcmd.js lights.js -c -m  > led.js")
os.system("gzip < led.js > led.js.gz")
