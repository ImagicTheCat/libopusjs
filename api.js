
function Encoder()
{
  this.enc = Module._Encoder_new.apply(null, arguments);
  this.out = Module._String_new();
}

// free encoder memory
Encoder.prototype.destroy = function()
{ 
  Module._Encoder_delete(this.enc); 
  Module._String_delete(this.out);
}

// add samples to the encoder buffer
// samples: Int16Array of interleaved (if multiple channels) samples
Encoder.prototype.input = function(samples)
{
  var ptr = Module._malloc(samples.length*samples.BYTES_PER_ELEMENT);
  var pdata = new Uint8Array(Module.HEAPU8.buffer, ptr, samples.length*samples.BYTES_PER_ELEMENT);
  pdata.set(new Uint8Array(samples.buffer));

  Module._Encoder_input(this.enc, ptr, samples.length);
  Module._free(ptr);
}

// output the next encoded packet
// return Uint8Array (valid until the next output call) or null if there is no packet to output
Encoder.prototype.output = function()
{
  var ok = Module._Encoder_output(this.enc, this.str);
  if(ok)
    return new Uint8Array(Module.HEAPU8.buffer, this.str, Module._String_size(this.str));
}

//export objects
Module.Encoder = Encoder;
//make the module global if not using nodejs
if(Module["ENVIRONMENT"] != "NODE")
  libopus = Module;
