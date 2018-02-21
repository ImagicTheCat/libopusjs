
var libopus = require("./libopus.js");

var enc = new libopus.Encoder(1,48000,24000,false);
var dec = new libopus.Decoder(1,48000);

var samples = new Int16Array(48000);
for(var k = 0; k < 48000; k++)
  samples[k] = Math.random()*30000;

enc.input(samples);

var data = enc.output();
var tsize = 0;
var tsamples = 0;
while(data){
  tsize += data.length;

  dec.input(data);
  var osamples = dec.output();
  console.log("encoded  "+data.length+" bytes");
  while(osamples){
    tsamples += osamples.length;
    console.log("decoded "+osamples.length+" samples");
    osamples = dec.output();
  }

  data = enc.output();
}

console.log("1s, 48000Hz, random => total encoded size "+tsize+" total decoded samples "+tsamples);
