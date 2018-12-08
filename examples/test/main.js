

if(typeof require != "undefined" && typeof libopus == "undefined"){
  LIBOPUS_WASM_URL = "../../dist/libopus.wasm";
  libopus = require("../../dist/libopus.wasm.js");
}

libopus.onload = function(){
  var start = new Date().getTime();

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

  enc.destroy();
  dec.destroy();

  console.log("1s, 48000Hz, random => total encoded size "+tsize+" total decoded samples "+tsamples);
  console.log("time "+(new Date().getTime()-start)+" ms");
}

//if using asm.js, already loaded, force onload
if(libopus.loaded)
  libopus.onload();
