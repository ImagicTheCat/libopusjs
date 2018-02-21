
#include <emscripten.h>
#include <opus.h>
#include <iostream>
#include <vector>

typedef std::string String;

class Encoder{
  public:
    Encoder(int _channels, long int _samplerate, long int _bitrate, bool voice_optimization): enc(NULL), samplerate(_samplerate), channels(_channels), bitrate(_bitrate)
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
        int samples_per_multiple = samplerate*2.5/1000.0;

        //allowed opus multiples are 1(2.5ms), 2(5ms), 4(10ms), 8(20ms), 16(40ms), 24(60ms)
        int multiple = samples.size()/(channels*samples_per_multiple);

        if(multiple >= 24)
          multiple = 24;
        else if(multiple >= 16)
          multiple = 16;
        else if(multiple >= 8)
          multiple = 8;
        else if(multiple >= 4)
          multiple = 4;
        else if(multiple >= 2)
          multiple = 2;
        else if(multiple >= 1)
          multiple = 1;

        int n_samples = multiple*samples_per_multiple*channels;

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
      
      return ok;
    }

    ~Encoder()
    {
      if(enc)
        opus_encoder_destroy(enc);
    }

  private:
    long int bitrate, samplerate;
    int channels;
    OpusEncoder *enc;
    std::vector<int16_t> samples;
    std::vector<std::string> packets;
};

extern "C"{

// Encoder

EMSCRIPTEN_KEEPALIVE Encoder* Encoder_new(int channels, long int samplerate, long int bitrate, bool voice_optimization)
{
  return new Encoder(channels, samplerate, bitrate, voice_optimization);
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

}
