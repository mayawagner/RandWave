// RandWave
// Generates 3 second audio file filled with randomized waveform at pitch C4.
// author: Maya Wagner
// last altered: 15 December 2021
// compile with: clang RandWave.c -o RandWave -lsndfile && ./RandWave


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>
#include <time.h>

#define kSampleRate 44100
#define kSec 3
#define kNumFrames (kSampleRate * kSec)
#define kNumChannels 1
#define kLevel 1
#define kFormat (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

int main(void)
{

// #1
// CREATE SOUND FILE TO WRITE INTO ----------------------|
// USER INPUT -------------------------------------------|

  // allocate memory for user file path, init variables
  char fpath[150];
  int userWave;

  // print user prompt for file path
  printf("\n---------------------\n\n");
  printf("Welcome to RandWave, a wavetable generator where every file generated is unique!\n\n");
  printf("Start by entering the file path and where you would like to save your custom wavetable file. Include the full path and the name you would like to give the wav file.\n\n");
  printf("Example: /Users/username/Desktop/RandWave.wav\n\n");
  printf("\n---------------------\n\n");

  // assign file path to variable for use with sfopen()
  printf("Enter path here: ");
  scanf("%s", fpath);
  FILE *fpath_ptr;
  fpath_ptr = fopen(fpath, "w");

  // store file path for use with sfopen()
  char *soundFile = fpath;
  fclose(fpath_ptr);

  // print user path for base waveform selection
  printf("\nGreat! Now pick a base waveform for your wavetable. Choose from the following by entering the corresponding number.\n\n");
  printf("SQUARE:      1\n");
  printf("TRIANGLE:    2\n");
  printf("SINE:        3\n");
  printf("SAWTOOTH:    4\n");
  printf("\nEnter selection here: ");
  scanf("%i", &userWave);

  if (userWave < 1 | userWave > 4)
  {
    printf("Whoops! That is not an option... Please run the program again and enter a number from 1 to 4.\n");

    return 1;
  }

  else
  {
    printf("\n---------------------\n\n");
    printf("Awesome! Generating file...\n");
  }

// #2
// ALLOCATE MEMORY, INIT BUFFER -------------------------|

  SNDFILE *sndFile;
  SF_INFO sfInfo;
  double *buffer;

  // sound parameters
  double amplitude = 0.25;
  double frequency = 261.1; // C4
  int numHarmonics = 15;

  // init 2 channel buffer for holding audio data
  buffer = malloc(kNumChannels * kNumFrames * sizeof(double));
  if (!buffer)
  {
    printf ("ERROR : Memory allocation failed.\n");
		return 1;
	};

  // init SF_INFO with 0s
  memset(&sfInfo, 0, sizeof(SF_INFO));

  // set format referencing previously defined constants
  sfInfo.samplerate = kSampleRate;
	sfInfo.frames = kNumFrames;
	sfInfo.channels = kNumChannels;
	sfInfo.format = kFormat;

  // open startFile in write mode
  sndFile = sf_open(soundFile, SFM_WRITE, &sfInfo);
  if(!sndFile)
  {
    printf ("ERROR: Waveform initiation failed.\n");
		puts(sf_strerror (NULL));
		return 1;
  }

// #3
// COMPUTE WAVEFORM -------------------------------------|

  for(int t = 0; t < kNumFrames; t++)
  {
    for(int i = 1; i <= numHarmonics; i++)
    {
      double sample;
      switch (userWave)
      {
        case 1:
        sample = amplitude * sin(2.0 * M_PI * (frequency / kSampleRate) * t);
        // write sample into buffer
        for(int c = 0; c < kNumChannels; c++)
        {
          buffer[kNumChannels * t] += sample;
        }
        break;

        case 2:
        sample = amplitude / i * sin(2.0 * M_PI * ((frequency * i)/kSampleRate) * t);
        // write sample into buffer
        for(int c = 0; c < kNumChannels; c++)
        {
          buffer[kNumChannels * t] += sample;
        }
        break;

        case 3:
        sample = amplitude * sin(2.0 * M_PI * (frequency/kSampleRate) * t);
        // write sample into buffer
        for(int c = 0; c < kNumChannels; c++)
        {
          buffer[kNumChannels * t] += sample;
        }
        break;

        case 4:
        sample = amplitude * (2 / M_PI) * asin(sin(t * (2.0 * M_PI) * (frequency / kSampleRate)));
        // write sample into buffer
        for(int c = 0; c < kNumChannels; c++)
        {
          buffer[kNumChannels * t] += sample;
        }
        break;
      }
    }
  }

// #4
// DISTORT FILE AT RANDOM CLIPPING AMOUNTS --------------|

  for(int n = 0; n < kNumFrames; n++)
  {
    if(buffer[n] < 0.0f)
    {
      buffer[n] *= -1.0f; // take absolute value (rectify)
    }

    // randomly clip data in buffer
    if(buffer[n] > 1.0f)
    {
      srand(time(0));
      buffer[n] *= ((rand() % 50) + 1);
    }

    if(buffer[n] > 1.0f)
    {
      // clip signal to 1
      buffer[n] = 1.0f;
    }

    // write distortion into buffer
    buffer[n] *= kLevel;
  }

// #5
// MODULATE ---------------------------------------------|

  for(int i = 0; i < kNumFrames; i++)
  {
    // init random rate, depth, gain variables
    srand(time(0));
    float rate = (rand() % 2) + 1;
    srand(time(0));
    int gain = (rand() % 3) + 1;
    srand(time(0));
    float depth = (rand() % 10) + 1;

    // create sine LFO
    int lowFreq = 1.0 + depth * sin(2.0 * M_PI * (rate / kSampleRate) * i);

    buffer[i] *= (lowFreq * gain) * kLevel;

    lowFreq = 1.0 - depth * sin(2.0 * M_PI * rate * i / kSampleRate);
  }


// #6
// WRITE OUT RESULT INTO AUDIO FILE, CLOSE --------------|

  sf_count_t count = sf_write_double(sndFile, buffer, sfInfo.channels * kNumFrames);

  // check in writing the file succeeeded
  if(count != sfInfo.channels * kNumFrames)
  {
    puts(sf_strerror(sndFile));
  }

  // close sound file and buffer
  sf_close(sndFile);
  free(buffer);

  printf("Your file has been saved at:\n\n");
  printf("%s\n\n", soundFile);
  printf("Thanks so much for using RandWave!\n\n");

  return 0;
}
