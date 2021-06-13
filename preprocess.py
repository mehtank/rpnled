#!/usr/bin/python 

from api import *

TEMPLATE_DIR = "templates"
INO_DIR = "ino"
WEB_DIR = "web"
CONTROL_DIR = "control"

cpplines = (l for l in open(TEMPLATE_DIR + "/commands.cpp.templ").readlines())
hpplines = (l for l in open(TEMPLATE_DIR + "/commands.h.templ").readlines())
htmllines = (l for l in open(TEMPLATE_DIR + "/index.html.templ").readlines())
jslines = (l for l in open(TEMPLATE_DIR + "/runcmd.js.templ").readlines())

with open(INO_DIR + "/commands.h", "w") as hpp, \
    open(INO_DIR + "/commands.cpp", "w") as cpp, \
    open(CONTROL_DIR + "/commands.py", "w") as py, \
    open(WEB_DIR + "/const.js", "w") as js, \
    open(WEB_DIR + "/runcmd.js", "w") as jsrun, \
    open(WEB_DIR + "/index.html", "w") as html:

    allfiles = ((hpp, hpplines), (cpp, cpplines), (html, htmllines), (jsrun, jslines))

    for fn, lines in allfiles:
        for l in lines:
            if "CONST" in l:
                break
            fn.write(l)

    val = -1

    for op in operators:
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
        val -= 1
      else:
        html.write( "<br>\n")
        cpp.write("\n")
        hpp.write("\n")
        py.write("\n")
        js.write("\n")
        while val % 10:
          val -= 1

      if not (val%29):
        html.write("</td><td>")

    for i, (c, s) in enumerate(other):
      hpp.write( "#define " + c + " " + s + "\n")
      js.write( "var " + c + " = " + s + ";\n")

    for fn, lines in allfiles:
        for l in lines:
            fn.write(l)

import re
plines = open(CONTROL_DIR + "/patterns.py").readlines()
start = re.compile("^([a-zA-Z0-9_]+) = makepattern\(\[\s*$");
end = re.compile("^]\)")
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
  js.write("};")

import os
from css_html_js_minify import process_single_html_file, process_single_js_file

os.chdir(WEB_DIR)
os.system("cat  const.js patterns.js runcmd.js lights.js > led.js")
process_single_js_file("led.js", "w")
os.system("gzip -f led.min.js")
os.system("gzip -f led.js")
os.system("gzip -f index.html")
os.system(f"/bin/bash -c \"cat <(xxd -i index.html.gz) <(xxd -i led.js.gz) | sed 's/unsigned/const unsigned/' | sed 's/\[\] =/\[\] PROGMEM =/' > ../{INO_DIR}/files.h\" ")
