#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
static uint64_t get_tm(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return ts.tv_sec*1000000000ull + ts.tv_nsec;
}

/* WAV解析 */
#define CHUNK_RIFF "RIFF"
#define CHUNK_WAVE "WAVE"
#define CHUNK_FMT "fmt "
#define CHUNK_DATA "data"

typedef struct
{
    uint32_t off;
    uint32_t chunksize;
    uint16_t audioformat;
    uint16_t numchannels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t blockalign;
    uint16_t bitspersample;
    uint32_t datasize;
}wav_t;

static int wav_decode_head(uint8_t* buffer, wav_t* wav)
{
    uint8_t* p = buffer;
    uint32_t chunksize;
    uint32_t subchunksize;
    if(0 != memcmp(p,CHUNK_RIFF,4))
    {
        return -1;
    }
    p += 4;
    chunksize = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
    wav->chunksize = chunksize;
    p += 4;
    if(0 != memcmp(p,CHUNK_WAVE,4))
    {
        return -2;
    }
    p += 4;

    do
    {
        if(0 == memcmp(p,CHUNK_FMT,4))
        {
            p += 4;
            subchunksize = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
            p += 4;
            /* 解析参数 */
            wav->audioformat = (uint16_t)p[0] | ((uint16_t)p[1]<<8);
            if((wav->audioformat == 0x0001) || (wav->audioformat == 0xFFFE))
            {
                p += 2;
                wav->numchannels = (uint16_t)p[0] | ((uint16_t)p[1]<<8);
                p += 2;
                wav->samplerate = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
                p += 4;
                wav->byterate = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
                p += 4;
                wav->blockalign = (uint16_t)p[0] | ((uint16_t)p[1]<<8);
                p += 2;
                wav ->bitspersample = (uint16_t)p[0] | ((uint16_t)p[1]<<8);
                p += 2;

                if(subchunksize >16)
                {
                    /* 有ext区域 */
                    uint16_t cbsize = (uint16_t)p[0] | ((uint16_t)p[1]<<8);
                    p += 2;
                    if(cbsize > 0)
                    {
                        /* ext数据 2字节有效bits wValidBitsPerSample ，4字节dwChannelMask 16字节SubFormat */
                        p += 2;
                        p += 4;
                        /* 比对subformat */
                        p += 16;       
                    }
                }
            }
            else
            {
                p += subchunksize;
            }
        }
        else if(0 == memcmp(p,CHUNK_DATA,4))
        {
            p += 4;
            subchunksize = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
            wav->datasize = subchunksize;
            p += 4;
            wav->off = (uint32_t)(p- buffer);
            return 0;
        }
        else
        {
            p += 4;
            subchunksize = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
            p += 4;
            p += subchunksize;
        }
    }while((uint32_t)(p - buffer) < (chunksize + 8));
    return -3;
}

/* 填充44字节的wav头 */
static void wav_fill_head(uint8_t* buffer, int samples, int chnum, int freq)
{
    /*
     * 添加wav头信息
     */
    uint32_t chunksize = 44-8+samples*chnum*16/8;
    uint8_t* p = (uint8_t*)buffer;
    uint32_t bps = freq*chnum*16/8;
    uint32_t datalen = samples*chnum*16/8;
    p[0] = 'R';
    p[1] = 'I';
    p[2] = 'F';
    p[3] = 'F';
    p[4] = chunksize & 0xFF;
    p[5] = (chunksize>>8) & 0xFF;
    p[6] = (chunksize>>16) & 0xFF;
    p[7] = (chunksize>>24) & 0xFF;
    p[8] = 'W';
    p[9] = 'A';
    p[10] = 'V';
    p[11] = 'E';

    p[12] = 'f';
    p[13] = 'm';
    p[14] = 't';
    p[15] = ' ';

    p[16] = 16;  /* Subchunk1Size */
    p[17] = 0;
    p[18] = 0;
    p[19] = 0;

    p[20] = 1;  /* PCM */
    p[21] = 0;

    p[22] = chnum; /* 通道数 */
    p[23] = 0;

    p[24] = freq & 0xFF;
    p[25] = (freq>>8) & 0xFF;
    p[26] = (freq>>16) & 0xFF;
    p[27] = (freq>>24) & 0xFF; 

    p[28] = bps & 0xFF;      /* ByteRate */
    p[29] = (bps>>8) & 0xFF;
    p[30] = (bps>>16) & 0xFF;
    p[31] = (bps>>24) & 0xFF; 

    p[32] = chnum*16/8; /* BlockAlign */
    p[33] = 0;

    p[34] = 16;  /* BitsPerSample */
    p[35] = 0;

    p[36] = 'd';
    p[37] = 'a';
    p[38] = 't';
    p[39] = 'a';

    p[40] = datalen & 0xFF;
    p[41] = (datalen>>8) & 0xFF;
    p[42] = (datalen>>16) & 0xFF;
    p[43] = (datalen>>24) & 0xFF; 
}

void wav_print(wav_t* wav)
{
   printf("off:%d\r\n",wav->off); 
   printf("chunksize:%d\r\n",wav->chunksize); 
   printf("audioformat:%d\r\n",wav->audioformat); 
   printf("numchannels:%d\r\n",wav->numchannels); 
   printf("samplerate:%d\r\n",wav->samplerate); 
   printf("byterate:%d\r\n",wav->byterate); 
   printf("blockalign:%d\r\n",wav->blockalign); 
   printf("bitspersample:%d\r\n",wav->bitspersample); 
   printf("datasize:%d\r\n",wav->datasize); 
}

#define NN 128
#define TAIL 1024

int main(int argc, char **argv)
{
   FILE *spk_fd, *mic_fd, *out_fd;   
   short spk_buf[NN], mic_buf[NN], out_buf[NN];
   uint8_t spk_wav_buf[44]; /* 输入spk wav文件头缓存 */
   uint8_t mic_wav_buf[44]; /* 输入mic wav文件头缓存 */
   uint8_t out_wav_buf[44]; /* 输出文件wav头缓存 */
   wav_t spk_wav;
   wav_t mic_wav;
   int samps;  /* 采样点数 */
   int times;    /* 读取次数 */
   SpeexEchoState *st;
   SpeexPreprocessState *den;
   int sampleRate;
   char* mic_fname = argv[1];
   char* spk_fname = argv[2];
   char* out_fname = argv[3];
   int ctl_i;
   float ctl_f;
   if (argc != 4)
   {
      fprintf(stderr, "testecho mic.wav spk.wav out.wav\n");
      exit(1);
   }
   spk_fd = fopen(spk_fname, "rb");
   if(spk_fd == NULL){
      fprintf(stderr, "open file %s err\n",spk_fname);
      exit(1);
   }else{
      fprintf(stderr, "open file %s ok\n",spk_fname);
   }
   mic_fd  = fopen(mic_fname,  "rb");
   if(mic_fd == NULL){
      fprintf(stderr, "open file %s err\n",mic_fname);
      fclose(spk_fd);
      exit(1);
   }else{
      fprintf(stderr, "open file %s ok\n",mic_fname);
   }
   out_fd    = fopen(out_fname, "wb");
   if(out_fd == NULL){
      fprintf(stderr, "open file %s err\n",out_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      exit(1);
   }else{
      fprintf(stderr, "open file %s ok\n",out_fname);
   }

   if(44 != fread(mic_wav_buf, 1, 44, mic_fd)){
      fprintf(stderr, "read file %s err\n",mic_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      fclose(out_fd);
      exit(1);
   }else{
      fprintf(stderr, "read file %s ok\n",mic_fname);
   }
   if(44 != fread(spk_wav_buf, 1, 44, spk_fd)){
      fprintf(stderr, "read file %s err\n",spk_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      fclose(out_fd);
      exit(1); 
   }else{
      fprintf(stderr, "read file %s ok\n",spk_fname);
   }

   if(0 != wav_decode_head(spk_wav_buf, &spk_wav)){
      fprintf(stderr, "decode file %s err\n",spk_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      fclose(out_fd);
      exit(1); 
   }else{
      fprintf(stderr, "decode file %s ok\n",spk_fname);
   }
   printf("[spk_wav]\r\n");
   wav_print(&spk_wav);
   if(0 != wav_decode_head(mic_wav_buf, &mic_wav)){
      fprintf(stderr, "decode file %s err\n",mic_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      fclose(out_fd);
      exit(1);  
   }else{
      fprintf(stderr, "decode file %s ok\n",mic_fname);
   }
   printf("[mic_wav]\r\n");
   wav_print(&mic_wav);

   samps = spk_wav.datasize > mic_wav.datasize ? mic_wav.datasize : spk_wav.datasize; /* 获取较小的数据大小 */
   samps /= spk_wav.blockalign;  /* 采样点数 =  数据大小 除以 blockalign */
   printf("\r\nsamps:%d\r\n",samps);

   sampleRate = spk_wav.samplerate;

   wav_fill_head(out_wav_buf, samps, 1, sampleRate);  /* 输出文件头 */
   if(44 != fwrite(out_wav_buf, 1, 44, out_fd)){
      fprintf(stderr, "write file %s err\n",out_fname);
      fclose(spk_fd);
      fclose(mic_fd);
      fclose(out_fd);
      exit(1);
   }

   st = speex_echo_state_init(NN, TAIL);
   den = speex_preprocess_state_init(NN, sampleRate);
   speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);

   ctl_i=1;
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_DENOISE, &ctl_i); /* 打开降噪 ctl_i=1打开 0关闭*/
   ctl_i=80;
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &ctl_i); 
   ctl_i=80;
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &ctl_i);

   times = samps / NN;   /* 一次读取NN个点,读取times次 */
   for(int i=0; i<times; i++)
   {
      if(NN != fread(mic_buf, sizeof(short), NN, mic_fd)){
        fprintf(stderr, "read file %s err\n",mic_fname);
        fclose(spk_fd);
        fclose(mic_fd);
        fclose(out_fd);
        exit(1);
      }

      if(NN != fread(spk_buf, sizeof(short), NN, spk_fd)){
        fprintf(stderr, "read file %s err\n",spk_fname);
        fclose(spk_fd);
        fclose(mic_fd);
        fclose(out_fd);
        exit(1);
      }

      uint32_t t0;
      uint32_t t1;
	  t0 = get_tm();
      speex_echo_cancellation(st, mic_buf, spk_buf, out_buf);
      speex_preprocess_run(den, out_buf);
      t1 = get_tm();
      printf("used:%duS\r\n",(t1-t0)/1000);

      if(NN != fwrite(out_buf, sizeof(short), NN, out_fd)){
        fprintf(stderr, "write file %s err\n",out_fname);
        fclose(spk_fd);
        fclose(mic_fd);
        fclose(out_fd);
        exit(1);
      }
   }
   speex_echo_state_destroy(st);
   speex_preprocess_state_destroy(den);

   fclose(out_fd);
   fclose(spk_fd);
   fclose(mic_fd);
   return 0;
}

