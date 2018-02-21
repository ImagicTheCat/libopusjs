
# libopusjs

This project is mostly about having access to the low-level packet encoding/decoding API of libopus in JavaScript.
Opus allow to encode audio without container and can handle packet loss, which makes it very suitable for VoIP over UDP (it also has voice optimizations).

# API

```js
// Encoder

// create encoder
// channels: 1-2
// samplerate: 8000,12000,16000,24000,48000
// bitrate
// voice_optimization: true/false 
Encoder(channels, samplerate, bitrate, voice_optimization)

// free encoder memory
Encoder.destroy()

// add samples to the encoder buffer
// samples: Int16Array of interleaved (if multiple channels) samples
Encoder.input(samples)

// output the next encoded packet
// return Uint8Array (valid until the next output call) or null if there is no packet to output
Encoder.output()

// Decoder

// create decoder
// channels and samplerate should match the encoder options
Decoder(channels, samplerate)

// free decoder memory
Decoder.destroy()

// add packet to the decoder buffer
// packet: Uint8Array
Decoder.input(packet)

// output the next decoded samples
// return samples (interleaved if multiple channels) as Int16Array (valid until the next output call) or null if there is no output
Decoder.output()
```

# Usage

Input data to the encoder or the decoder, then loop over the output call until it returns null. A single input can give multiple outputs or none.

See `test.js`.

```js
// nodejs: 
var libopus = require("./libopus.js");

// in browser: 
// load the script, libopus will be available as a global

// create encoder
var enc = new libopus.Encoder(1,48000,24000,false);

// create decoder
var dec = new libopus.Decoder(1,48000);

// destroy encoder
enc.destroy();

// destroy decoder
dec.destroy();
```

