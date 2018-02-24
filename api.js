// Encoder

// create encoder
// channels: 1-2
// samplerate: 8000,12000,16000,24000,48000
// bitrate
// voice_optimization: true/false 
function Encoder(channels, samplerate, bitrate, voice_optimization)
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
  pdata.set(new Uint8Array(samples.buffer, samples.byteOffset, samples.length*samples.BYTES_PER_ELEMENT));

  Module._Encoder_input(this.enc, ptr, samples.length);
  Module._free(ptr);
}

// output the next encoded packet
// return Uint8Array (valid until the next output call) or null if there is no packet to output
Encoder.prototype.output = function()
{
  var ok = Module._Encoder_output(this.enc, this.out);
  if(ok)
    return new Uint8Array(Module.HEAPU8.buffer, Module._String_data(this.out), Module._String_size(this.out));
}

// Decoder

// create decoder
// channels and samplerate should match the encoder options
function Decoder(channels, samplerate)
{
  this.dec = Module._Decoder_new.apply(null, arguments);
  this.out = Module._Int16Array_new();
}

// free decoder memory
Decoder.prototype.destroy = function()
{ 
  Module._Decoder_delete(this.dec); 
  Module._Int16Array_delete(this.out);
}

// add packet to the decoder buffer
// packet: Uint8Array
Decoder.prototype.input = function(packet)
{
  var ptr = Module._malloc(packet.length*packet.BYTES_PER_ELEMENT);
  var pdata = new Uint8Array(Module.HEAPU8.buffer, ptr, packet.length*packet.BYTES_PER_ELEMENT);
  pdata.set(new Uint8Array(packet.buffer, packet.byteOffset, packet.length*packet.BYTES_PER_ELEMENT));

  Module._Decoder_input(this.dec, ptr, packet.length);
  Module._free(ptr);
}

// output the next decoded samples
// return samples (interleaved if multiple channels) as Int16Array (valid until the next output call) or null if there is no output
Decoder.prototype.output = function()
{
  var ok = Module._Decoder_output(this.dec, this.out);
  if(ok)
    return new Int16Array(Module.HEAPU8.buffer, Module._Int16Array_data(this.out), Module._Int16Array_size(this.out));
}


//export objects
Module.Encoder = Encoder;
Module.Decoder = Decoder;

//make the module global if not using nodejs
if(Module["ENVIRONMENT"] != "NODE")
  libopus = Module;
