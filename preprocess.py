#!/usr/bin/python 

from control.constants import *

TEMPLATE_DIR = "templates"
INO_DIR = "rpnled"
WEB_DIR = "web"
CONTROL_DIR = "control"

cpplines = (l for l in file(TEMPLATE_DIR + "/commands.h.templ").readlines())
htmllines = (l for l in file(TEMPLATE_DIR + "/index.html.templ").readlines())

with open(INO_DIR + "/commands.h", "w") as cpp:
  with open(WEB_DIR + "/const.js", "w") as js:
    with open(WEB_DIR + "/index.html", "w") as html:
      for l in cpplines:
        if "CONST" in l:
          break
        cpp.write(l)

      for l in htmllines:
        if "CONST" in l:
          break
        html.write(l)
        
      val = 0

      for (c, s) in constants:
        val -= 1
        if c is not None:
          cpp.write( "#define " + c + " " + repr(val) + "\n")
          js.write( "var " + c + " = " + repr(val) + ";\n")
          #html.write( "<button id=go onclick=\"addcmd("+c+")\">" + c + "</button><br>\n")
          html.write( c + "<br>\n")
        else:
          html.write( "<br>\n")
        if not (val%29):
          html.write("</td><td>")

        '''
        else:
          while val % 10:
            val -= 1
        '''

      for i, (c, s) in enumerate(other):
        cpp.write( "#define " + c + " " + s + "\n")
        js.write( "var " + c + " = " + s + ";\n")

      for l in cpplines:
        cpp.write(l)

      for l in htmllines:
        html.write(l)

with open(WEB_DIR + "/runcmd.js", "w") as js:
  jslines = file(TEMPLATE_DIR + "/runcmd.js.templ").readlines()
  for jsline in jslines:
    if "SWITCH" in jsline: 
      copying = False
      lines = file(INO_DIR + "/commands.cpp").readlines()
      for line in lines:
        if "SWITCH" in line: 
          copying = not copying
        if copying:
          js.write(line)
    else:
      js.write(jsline)

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
