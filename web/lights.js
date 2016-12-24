var c;
var ctx;
var run = 0;

function load_code(code) {
  var p = document.getElementById("code");
  p.value = code;
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
  for (key in code_patterns) {
    var b = document.createElement("input");
    b.type = "button";
    b.value = "Pattern: " + key;
    b.onclick = new Function("load_code(code_patterns[\"" + key + "\"]);");
    p.appendChild(b);
    p.appendChild(document.createElement("br"));
  }

  load_code(code_patterns["rainbow"]);
  clear();
  loop(1);
}

function prog() {
  var p = document.getElementById("code");
  code = eval("[" + p.value + "]");
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
/*
    var value = v;
    var saturation = s;

    var invsat = 255 - saturation;
    var brightness_floor = (value * invsat) >>> 8;

    // The color amplitude is the maximum amount of R, G, and B
    // that will be added on top of the brightness_floor to
    // create the specific hue desired.
    var color_amplitude = value - brightness_floor;

    // Figure out which section of the hue wheel we're in,
    // and how far offset we are withing that section
    var section = h >>> 6; // 0..2
    var offset = h & 63;  // 0..63

    var rampup = offset; // 0..63
    var rampdown = (64 - 1) - offset; // 63..0

    var rampup_amp_adj   = (rampup   * color_amplitude) >>> 6;
    var rampdown_amp_adj = (rampdown * color_amplitude) >>> 6;

    // add brightness_floor offset to everything
    var rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
    var rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;


    if( section ) {
        if( section == 1) {
            // section 1: 0x40..0x7F
            var r = brightness_floor;
            var g = rampdown_adj_with_floor;
            var b = rampup_adj_with_floor;
        } else {
            // section 2; 0x80..0xBF
            var r = rampup_adj_with_floor;
            var g = brightness_floor;
            var b = rampdown_adj_with_floor;
        }
    } else {
        // section 0: 0x00..0x3F
        var r = rampdown_adj_with_floor;
        var g = rampup_adj_with_floor;
        var b = brightness_floor;
    }
    console.log(r,g,b);
    return CRGB(r,g,b);
*/

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

function upload() {
  var p = document.getElementById("code");
  var arr = eval("[" + p.value + "]");

  var bytes = [36, 80, 82, 79, 71, arr.length]; // $PROG + len 
  for (var i = 0, len = arr.length; i < len; i++) {
      bytes.push(arr[i] & 0xff);
      bytes.push((arr[i] & 0xffff) >> 8);
  }

  var bbuf = new Uint8Array(arr.length*2 + 6);
  for (var i = 0, len = bytes.length; i < len; i++) 
    bbuf[i] = bytes[i];

  // sendstr += String.fromCharCode.apply(null, bytes);

  // console.log(sendstr);
  // console.log(sendstr.length);
  // connection.send(sendstr);
  console.log(bytes.length);
  connection.send(bbuf.buffer);
}
