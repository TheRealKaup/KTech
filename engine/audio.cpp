#include "config.hpp"
#include "engine.hpp"
#include <cstddef>
#include <portaudio.h>

#define config_framesPerBuffer 256

// Audio values: 16 bit depth, 2 channels, default (probably 44100) sample rate

static void Log(std::string string)
{
	std::cout << "\033[38;2;255;255;0m" << string << "\033[m" << std::endl << std::flush;
}
static void Log(std::string string, unsigned number)
{
	std::cout << "\033[38;2;255;255;0m" << string << number << "\033[m" << std::endl << std::flush;
}

// Engine
PaStream* Engine::stream = nullptr;
long Engine::audioPerformance = 0L;
// std::vector<Engine::AudioSource*> Engine::audioSources;

// default device info
static PaStreamParameters outputParameters;
static unsigned char stream_SampleSize = 2;
static unsigned char stream_Channels = 2;
static unsigned char stream_BlockAlign = 4;

static std::vector<Engine::AudioSource*> activeSources;
static int32_t tempAudioLimiter = 0;

static int32_t unlimitedOutput[config_framesPerBuffer * 2];

static Engine::TimePoint start;
static Engine::TimePoint end;

int Callback(
	const void *input,
	void *output,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	// start.SetToNow();
	static bool firstSourceFilled;
	static size_t f;

	// Set unlimitedOutput to the first source (which saves memsetting it)
	if (activeSources.size() > 0)
	{
		firstSourceFilled = true;
		// Set to first source (which saves a memset at the start)
		if (activeSources[0]->channels == 2)
		{
			for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[0]->cur++)
			{
				if (activeSources[0]->cur >= activeSources[0]->endpointToPlay)
				{
					if (activeSources[0]->loopsToPlay > 0)
					{
						activeSources[0]->loopsToPlay--;
						activeSources[0]->cur = activeSources[0]->startPoint;
					}
					else
					{
						activeSources[0]->playing = false;
						activeSources.erase(activeSources.begin());
						memset(unlimitedOutput + f, 0, (config_framesPerBuffer * 2 - f) * 4);
						firstSourceFilled = false;
						break;
					}
				}
				unlimitedOutput[f] = activeSources[0]->data[activeSources[0]->cur * 2] * activeSources[0]->volume;
				unlimitedOutput[f + 1] = activeSources[0]->data[activeSources[0]->cur * 2 + 1] * activeSources[0]->volume;
			}
		}
		else if (activeSources[0]->channels == 1)
		{
			for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[0]->cur++)
			{				
				if (activeSources[0]->cur >= activeSources[0]->endpointToPlay)
				{
					if (activeSources[0]->loopsToPlay > 0)
					{
						activeSources[0]->loopsToPlay--;
						activeSources[0]->cur = activeSources[0]->startPoint;
					}
					else
					{
						activeSources[0]->playing = false;
						activeSources.erase(activeSources.begin());
						memset(unlimitedOutput + f, 0, (config_framesPerBuffer * 2 - f) * 4);
						firstSourceFilled = false;
						break;
					}
				}
				unlimitedOutput[f] = activeSources[0]->data[activeSources[0]->cur] * activeSources[0]->volume;
				unlimitedOutput[f + 1] = activeSources[0]->data[activeSources[0]->cur] * activeSources[0]->volume;
			}
		}
		// Add the other sources
		for (size_t s = firstSourceFilled ? 1 : 0; s < activeSources.size(); s++)
		{

			if (activeSources[s]->channels == 2)
			{
				for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[s]->cur++)
				{
					if (activeSources[s]->cur >= activeSources[s]->endpointToPlay)
					{
						if (activeSources[s]->loopsToPlay > 0) // Loop
						{
							activeSources[s]->loopsToPlay--;
							activeSources[s]->cur = activeSources[s]->startPoint;
						}
						else
						{
							activeSources[s]->playing = false;
							activeSources.erase(activeSources.begin() + s);
							s--;
							break;
						}
					}
					unlimitedOutput[f] += activeSources[s]->data[activeSources[s]->cur * 2] * activeSources[s]->volume;
					unlimitedOutput[f + 1] += activeSources[s]->data[activeSources[s]->cur * 2 + 1] * activeSources[s]->volume;
				}
			}
			else if (activeSources[s]->channels == 1)
			{
				for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[s]->cur++)
				{				
					if (activeSources[s]->cur >= activeSources[s]->endpointToPlay)
					{
						if (activeSources[s]->loopsToPlay > 0) // Loop
						{
							activeSources[s]->loopsToPlay--;
							activeSources[s]->cur = activeSources[s]->startPoint;
						}
						else
						{
							activeSources[s]->playing = false;
							activeSources.erase(activeSources.begin() + s);
							s--;
							break;
						}
					}
					unlimitedOutput[f] += activeSources[s]->data[activeSources[s]->cur] * activeSources[s]->volume;
					unlimitedOutput[f + 1] += activeSources[s]->data[activeSources[s]->cur] * activeSources[s]->volume;
				}
			}
		}
		// At last, limit `unlimitedOutput` and add it to `output`.
		for (f = 0; f < config_framesPerBuffer * 2; f++)
		{
			if (unlimitedOutput[f] > 32767)
				((int16_t*)(output))[f] = 32767;
			else if (unlimitedOutput[f] < -32767)
				((int16_t*)(output))[f] = -32767;
			else
				((int16_t*)(output))[f] = unlimitedOutput[f];
		}
	}
	else
		memset(output, 0, config_framesPerBuffer * 4);

	// end.SetToNow();
	// Engine::audioPerformance = (end.Nanoseconds() - start.Nanoseconds());

	return paContinue;
}

void Engine::InitializeAudio()
{
	// Start PortAudio
	Pa_Initialize();
	// Open PortAudio stream
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	Pa_OpenStream(&stream, NULL, &outputParameters, Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultSampleRate, config_framesPerBuffer, paClipOff, Callback, nullptr);
	// Data used in callback function
	stream_SampleSize = Pa_GetSampleSize(paInt16);
	stream_Channels = outputParameters.channelCount;
	stream_BlockAlign = stream_SampleSize * stream_Channels;
	// Start the stream
	Pa_StartStream(stream);
}
void Engine::TerminateAudio()
{
	Pa_AbortStream(stream);
	Pa_Terminate();
}

bool Engine::AudioSource::LoadWavFile(std::string fileName)
{
	int32_t temp = 0UL;
	int16_t temp2 = 0UL;
	
	std::ifstream file(fileName, std::ifstream::binary);

	// Couldn't open file
	if (!file.is_open())
		return false;
	
	// RIFF chunk
	while (true) // Find the chunk
	{
		file.read((char*)&temp, 4);
		if (temp != 1179011410) // 'FFIR'
			file.seekg(-3, std::ifstream::cur);
		else
			break;
	}
	file.read((char*)&fileSize, 4); 
	file.read((char*)&temp, 4); // "WAVE"

	// FMT chunk
	while (true) // Find the chunk
	{
		file.read((char*)&temp, 4);
		if (temp != 544501094) // ' tmf'
			file.seekg(-3, std::ifstream::cur);
		else
		 	break;
	}
	file.read((char*)&temp, 4); // FMT size -8 (16)
	if (temp != 16) // something something 16 is for PCM?
		return false;
	file.read((char*)&temp2, 2); // Compression state
	if (temp2 != 1) // Other than 1 means some form of compression.
		return false;
	file.read((char*)&channels, 2); // Channels
	file.read((char*)&sampleRate, 4); // Sample rate
	file.read((char*)&byteRate, 4); // Byte rate
	file.read((char*)&blockAlign, 2); // Block align
	file.read((char*)&bitsPerSample, 2); // Bits per sample
	
	// Data chunk
	while (true) // Find the chunk
	{
		file.read((char*)&temp, 4);
		if (temp != 1635017060) // 'atad'
			file.seekg(-3, std::ifstream::cur);
		else
		 	break;
	}
	file.read((char*)&dataSize, 4); // Data size
	// Get it ourselves in case it isn't accurate
	unsigned startPos = file.tellg(); // Get the current position
	file.seekg(0, std::ifstream::end); // Move to the end
	unsigned endPos = file.tellg(); // Get the current position
	dataSize = endPos - startPos; // Get the size
	file.seekg(startPos, std::ifstream::beg); // Return to the last position
	data = new int16_t[dataSize / 2]; // Resize data array to fit the data
	file.read((char*)data, dataSize);// Read the data into the data array

	frames = dataSize / blockAlign;
	
	return true;
}

void Engine::AudioSource::Play(unsigned long start, unsigned long length, unsigned short loops, float volume)
{
	playing = true;
	this->volume = volume;
	if (length == 0)
		endpointToPlay = frames;
	else if (length > (dataSize / channels))
		length = frames;
	else
		endpointToPlay = length + start;
	loopsToPlay = loops;
	cur = start;
	startPoint = cur;
	for (unsigned i = 0; i < activeSources.size(); i++)
		if (activeSources[i] == this)
			return; // Don't add to activeSources if already there.
	activeSources.push_back(this);
}

Engine::AudioSource::AudioSource() {}

void Engine::AudioSource::Pause()
{
	playing = false;
}

void Engine::AudioSource::Resume()
{
	playing = true;
}

void Engine::AudioSource::Stop()
{
	playing = false;
	cur = 0;
}