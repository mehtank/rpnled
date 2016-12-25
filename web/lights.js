var c;
var ctx;
var run = 0;

function load_code(code) {
  var p = document.getElementById("code_raw");
  p.value = code;
  var p = document.getElementById("code");
  p.textContent = "";
  var p = document.getElementById("code_widgets");
  while (p.firstChild) 
    p.removeChild(p.firstChild);
  prog();
}

function clear() {
  draw([0,C_VALUE], 0);
}

function draw(code, time) {
  var rnd = Math.floor((Math.random() * 256));
  for (var i = 0; i < 600; i++) {
    ctx.fillStyle = "#ffffff";
    var fill = runCmd(code, time, i, rnd);
    if (fill) {
      ctx.fillStyle = fill
      if (i < 200) 
        ctx.fillRect(1, (200-i)*5,4,4);
      else if (i < 400) 
        ctx.fillRect((i-199)*5,1,4,4);
      else 
        ctx.fillRect(1004, (i-399)*5,4,4);
    } else {
      run = 0;
      return;
    }
  }
}

window.onload = function() {
  c = document.getElementById("lights");
  ctx = c.getContext("2d");

  var p = document.getElementById("code_buttons");
  var s = document.createElement("select");
  for (key in code_patterns) {
    var b = document.createElement("option");
    b.text = "Pattern: " + key;
    b.onmouseup = new Function("load_code(code_patterns[\"" + key + "\"]);");
    s.appendChild(b);
  }
  p.appendChild(s);

  load_code(code_patterns["rainbow"]);
  clear();
  loop(1);
}

function reprog() {
  var p = document.getElementById("code_raw");
  load_code(p.value);
}

function prog() {
  resolve();
  var p = document.getElementById("code");
  code = eval("[" + p.textContent + "]");
  console.log(code, code.length);
  clear();
}


function loop(forever) {
  if (forever !== undefined)
    run = forever;
  var d = new Date();
  var n = d.getTime();
  var time = n & 0xffffffff;
  // console.log(time);
  draw(code, time)
  if (run)
    setTimeout(loop, 10);
}

var code = [C_TIME, 4, C_RSHIFT, C_INDEX, 3, C_LSHIFT, C_PLUS, 6, C_TIMES, 256, 6, C_TIMES, C_MOD, 255, 255, C_HSV];

function CRGB(r,g,b) {
  r &= 0xff;
  g &= 0xff;
  b &= 0xff;
  //console.log(r, g, b);
  str = "000000" + ((r << 16) + (g << 8) + b).toString(16);
  //console.log(str);
  return "#" + str.slice(-6);
}

function CHSV(h, s, v) {
  h &= 0xff;
  s &= 0xff;
  v &= 0xff;

    var f = (h % 42) * 255 / 42 | 0;
    var p = (255 - s) * v / 255 | 0;
    var q = (255 - f * s / 255) * v / 255 | 0;
    var t = (255 - (255 - f) * s / 255) * v / 255 | 0;
    var r = 0, g = 0, b = 0;
    switch(~~(h / 42) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    //console.log(f, p, q, t, h, s, v, r, g, b);
    //console.log(r,g,b);
    return CRGB(r, g, b);
}


var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
connection.binaryType = "arraybuffer";
connection.onopen = function(){
  connection.send('Connect ' + new Date()); 
};
connection.onerror = function(error){
  console.log('WebSocket Error ', error);
};
connection.onmessage = function(e){
  console.log('Server: ', e.data);
};

function resolve() {
  var raw = document.getElementById("code_raw");
  var code = document.getElementById("code");
  var wds = raw.value.split(",").map(function(x){return x.trim()});
  console.log(wds);

  for (var i = 0, len = wds.length; i < len; i++) {
    w = wds[i];
    if (w[0] == "$") {
        var tokens = w.substring(1).split("_");
        var name = tokens[0];
        var param = document.getElementById("codeparam_" + name + "_value");
        if (param === null) {
            addwidget(tokens);
            wds[i] = tokens[1];
        } else {
            wds[i] = param.value;
        }
    }
  }

  code.textContent = wds.join(", ");
}

function addwidget(tokens) {
  var w = document.getElementById("code_widgets");
  var p = document.createElement("div");

  name = tokens[0];

  var b = document.createElement("span");
  b.textContent = name;
  b.id = "codeparam_" + name + "_name";
  p.appendChild(b);

  var b = document.createElement("input");
  b.value = tokens[1];
  b.id = "codeparam_" + name + "_value";
  b.onchange = new Function("prog()");
  p.appendChild(b);

  w.appendChild(p);
}

function upload() {
  resolve();
  var p = document.getElementById("code");
  var arr = eval("[" + p.textContent + "]");

  var bytes = [36, 80, 82, 79, 71, arr.length]; // $PROG + len 
  for (var i = 0, len = arr.length; i < len; i++) {
      bytes.push(arr[i] & 0xff);
      bytes.push((arr[i] & 0xffff) >> 8);
  }

  var bbuf = new Uint8Array(arr.length*2 + 6);
  for (var i = 0, len = bytes.length; i < len; i++) 
    bbuf[i] = bytes[i];

  console.log(bytes.length);
  connection.send(bbuf.buffer);
}
