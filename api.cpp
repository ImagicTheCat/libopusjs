
#include <emscripten.h>
#include <opus.h>
#include <iostream>
#include <vector>

typedef std::string String;
typedef std::vector<int16_t> Int16Array;

class Encoder{
  public:
    Encoder(int _channels, long int _samplerate, long int _bitrate, float _frame_size, bool voice_optimization): enc(NULL), samplerate(_samplerate), channels(_channels), bitrate(_bitrate), frame_size(_frame_size)
    {
      int err;
      enc = opus_encoder_create(samplerate, channels, (voice_optimization ? OPUS_APPLICATION_VOIP : OPUS_APPLICATION_AUDIO), &err);

      if(enc != NULL){
        if(bitrate <= 0)
          opus_encoder_ctl(enc, OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
        else
          opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrate));
      }
      else
        std::cerr << "[libopusjs] error while creating opus encoder (errcode " << err << ")" << std::endl;
    }

    void input(const int16_t *data, int size)
    {
      std::vector<int16_t> nsamples(data, data+size);
      samples.insert(samples.end(), nsamples.begin(), nsamples.end());
    }

    bool output(String *out)
    {
      bool ok = false;
      if(enc){
        //compute the size of samples to read
        long int n_samples = samplerate*(frame_size/1000.0)*channels;
        if(samples.size() >= n_samples){
          //initialize the ouput buffer
          size_t max_size = n_samples/(float)samplerate*bitrate/8*1.5;
          char *buffer = new char[max_size];

          //convert samples to packets
          long int packet_size = (n_samples > 0 ? opus_encode(enc, &samples[0], n_samples/channels, (unsigned char*)buffer, max_size) : 0);

          if(packet_size > 0){
            out->assign(buffer, packet_size);
            ok = true;
          }

          //std::cout << samples.size() << " == " << n_samples << "(" << samples_per_multiple << ") " << (packet_size > 0 ? "success" : "failed") << std::endl;
          delete [] buffer;
          samples.erase(samples.begin(), samples.begin()+n_samples);
        }
      }
      
      return ok;
    }

    ~Encoder()
    {
      if(enc)
        opus_encoder_destroy(enc);
    }

  private:
    float frame_size;
    long int bitrate, samplerate;
    int channels;
    OpusEncoder *enc;
    std::vector<int16_t> samples;
};

class Decoder{
  public:
    Decoder(int _channels, long int _samplerate): dec(NULL), samplerate(_samplerate), channels(_channels)
    {
      int err;
      dec = opus_decoder_create(samplerate, channels, &err);

      if(dec == NULL)
        std::cerr << "[libopusjs] error while creating opus decoder (errcode " << err << ")" << std::endl;
    }

    void input(const char *data, size_t size)
    {
      packets.push_back(std::string(data,size));
    }

    bool output(Int16Array *out)
    {
      bool ok = false;

      if(packets.size() > 0 && dec != NULL){
        int ret_size = 0;

        long int buffer_size = 120/1000.0*samplerate*channels; //120ms max
        int16_t *buffer = new int16_t[buffer_size];

        //extract samples from packets
        std::string &packet = packets[0];

        if(packet.size() > 0)
          ret_size = opus_decode(dec, (const unsigned char*)packet.c_str(), packet.size(), buffer, buffer_size/channels, 0);

        if(ret_size > 0){
          *out = std::vector<int16_t>(buffer, buffer+ret_size*channels);
          ok = true;
        }

        packets.erase(packets.begin());
        delete [] buffer;
      }

      return ok;
    }

    ~Decoder()
    {
      if(dec)
        opus_decoder_destroy(dec);
    }

  private:
    long int samplerate;
    int channels;
    OpusDecoder *dec;
    std::vector<std::string> packets;
};

extern "C"{

// Encoder

EMSCRIPTEN_KEEPALIVE Encoder* Encoder_new(int channels, long int samplerate, long int bitrate, float frame_size, bool voice_optimization)
{
  return new Encoder(channels, samplerate, bitrate, frame_size, voice_optimization);
}

EMSCRIPTEN_KEEPALIVE void Encoder_delete(Encoder *self)
{
  delete self;
}

EMSCRIPTEN_KEEPALIVE void Encoder_input(Encoder *self, const int16_t *data, int size)
{
  self->input(data, size);
}

EMSCRIPTEN_KEEPALIVE bool Encoder_output(Encoder *self, String *out)
{
  return self->output(out);
}

// Decoder

EMSCRIPTEN_KEEPALIVE Decoder* Decoder_new(int channels, long int samplerate)
{
  return new Decoder(channels, samplerate);
}

EMSCRIPTEN_KEEPALIVE void Decoder_delete(Decoder *self)
{
  delete self;
}

EMSCRIPTEN_KEEPALIVE void Decoder_input(Decoder *self, const char* data, size_t size)
{
  self->input(data,size);
}

EMSCRIPTEN_KEEPALIVE bool Decoder_output(Decoder *self, Int16Array *out)
{
  return self->output(out);
}

// String

EMSCRIPTEN_KEEPALIVE size_t String_size(String *self)
{
  return self->size();
}

EMSCRIPTEN_KEEPALIVE String* String_new()
{
  return new std::string();
}

EMSCRIPTEN_KEEPALIVE const char* String_data(String *self)
{
  return self->c_str();
}

EMSCRIPTEN_KEEPALIVE void String_delete(String *self)
{
  delete self;
}

// Int16Array

EMSCRIPTEN_KEEPALIVE size_t Int16Array_size(Int16Array *self)
{
  return self->size();
}

EMSCRIPTEN_KEEPALIVE Int16Array* Int16Array_new()
{
  return new std::vector<int16_t>();
}

EMSCRIPTEN_KEEPALIVE const int16_t* Int16Array_data(Int16Array *self)
{
  return &(*self)[0];
}

EMSCRIPTEN_KEEPALIVE void Int16Array_delete(Int16Array *self)
{
  delete self;
}

}

