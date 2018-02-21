
var libopus = require("./libopus.js");

var enc = new libopus.Encoder(1,48000,24000,false);

var samples = new Int16Array(48000);
for(var k = 0; k < 48000; k++)
  samples[k] = Math.random()*30000;

enc.input(samples);

var data = enc.output();
var tsize = 0
while(data){
  tsize += data.length;
  console.log(data.length);
  data = enc.output();
}

console.log("total size = "+tsize);
