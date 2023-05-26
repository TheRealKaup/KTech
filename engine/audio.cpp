#include "engine.hpp"

static void Log(std::string string)
{
	std::cout << "\033[38;2;255;255;0m" << string << "\033[m" << std::endl << std::flush;
}
static void Log(std::string string, unsigned number)
{
	std::cout << "\033[38;2;255;255;0m" << string << number << "\033[m" << std::endl << std::flush;
}

PaStream* Engine::stream = nullptr;
// default device info
const PaDeviceInfo* ddi;
PaStreamParameters outputParameters;
unsigned char stream_SampleSize;
unsigned char stream_Channels;
unsigned char stream_BlockAlign;

std::vector<Engine::AudioSource*> activeSources;

int Callback(
	const void *input,
	void *output,
	unsigned long framesPerBuffer, // samples/channels in this buffer
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	// Set output to 0
	for (unsigned i = 0; i < framesPerBuffer * 4; i++)
		((char*)(output))[i] = 0;

	// Add all the active audio sources' data into the buffer
	for (unsigned s = 0; s < activeSources.size(); s++)
	{
		// Add all the frames of each source one by one.
		for (unsigned f = 0; f < framesPerBuffer; f++, activeSources[s]->cur++)
		{
			// Add the source's frame and make it fit in correctly with the stream's channel and sample size.
			for (unsigned c = 0; c < outputParameters.channelCount; c++) // Per channels
			{
				for (unsigned b = 0; b < stream_SampleSize; b++) // Per sample byte
				{
					((char*)output)[f * stream_BlockAlign + c * stream_SampleSize + b];// = activeSources[s]->data[activeSources[s]->cur * activeSources[s]->blockAlign + j];
				}
			}
			if (activeSources[s]->cur >= activeSources[s]->endpointToPlay)
			{
				if (activeSources[s]->loopsToPlay > 0)
				{
					activeSources[s]->loopsToPlay--;
					activeSources[s]->cur = activeSources[s]->startPoint;
				}
				else
				{
					activeSources[s]->playing = false;
					return paContinue;
				}
			}
		}
	}

	return paContinue;
}

void Engine::InitializeAudio()
{
	Pa_Initialize();

	ddi = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());

	// Open PortAudio stream
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = ddi->maxOutputChannels >= 2 ? 2 : 1;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = ddi->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	Pa_OpenStream(&stream, NULL, &outputParameters, ddi->defaultSampleRate, 256, paNoFlag, Callback, nullptr);
	stream_SampleSize = Pa_GetSampleSize(paInt16);
	stream_Channels = outputParameters.channelCount;
	stream_BlockAlign = stream_SampleSize * stream_Channels;

}
void Engine::TerminateAudio()
{
	Pa_AbortStream(stream);
	Pa_CloseStream(stream);
	Pa_Terminate();
}

bool Engine::AudioSource::LoadWavFile(std::string fileName)
{
	char32_t temp = 0UL;
	char16_t temp2 = 0UL;
	unsigned long read = 0UL;
	
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
	data = new char[dataSize]; // Resize data array to fit the data
	file.read((char*)data, dataSize);// Read the data into the data array

	dataSizeInSamples = dataSize / blockAlign;
	
	return true;
}

void Engine::AudioSource::Play(unsigned long start, unsigned long length, unsigned short loops, std::vector<float> volumes)
{
	Log("Playing");
	playing = true;
	if (length == 0)
		endpointToPlay = dataSizeInSamples;
	else	
		endpointToPlay = length + start;
	loopsToPlay = loops;
	cur = start;
	startPoint = cur;
	Pa_StartStream(stream);
}

Engine::AudioSource::AudioSource() {}

void Engine::AudioSource::Pause()
{
	Pa_StopStream(stream);
}

void Engine::AudioSource::Resume()
{
	Pa_StartStream(stream);
}

void Engine::AudioSource::Stop()
{
	Pa_StopStream(stream);
	cur = 0;
}

void Engine::AudioSource::ChangeVolume(float volume)
{
}

void Engine::AudioSource::ChangeVolumes(std::vector<float> volumes)
{
}