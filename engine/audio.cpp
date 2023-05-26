#include "engine.hpp"

// Audio values: 16 bit depth, 2 channels, default (unknown) sample rate

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
PaStreamParameters outputParameters;
unsigned char stream_SampleSize = 2;
unsigned char stream_Channels = 2;
unsigned char stream_BlockAlign = 4;

std::vector<Engine::AudioSource*> activeSources;
int32_t tempAudioLimiter = 0;

int Callback(
	const void *input,
	void *output,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	memset(output, 0, framesPerBuffer * stream_BlockAlign);

	// Add all the active audio sources' data into the buffer
	for (size_t s = 0; s < activeSources.size(); s++)
	{
		// Only supports 16 bit depth
		if (activeSources[s]->bitsPerSample != 16)
			continue;

		// Only supports 2 or 1 channels (which will be converted into 2 channels)
		if (activeSources[s]->channels == 2)
		{
			// Add all the frames of each source one by one.
			for (unsigned f = 0; f < framesPerBuffer; f++, activeSources[s]->cur++)
			{
				// Populate output, each channel separately
				tempAudioLimiter = (int32_t)((int16_t*)(output))[f * 2] + (int32_t)((int16_t*)(activeSources[s]->data))[activeSources[s]->cur * 2] * activeSources[s]->volume;
				if (tempAudioLimiter > 32767)
					((int16_t*)(output))[f * 2] = 32767;
				else if (tempAudioLimiter < -32767)
					((int16_t*)(output))[f * 2] = -32767;
				else
					((int16_t*)(output))[f * 2] = tempAudioLimiter;
				tempAudioLimiter = (int32_t)((int16_t*)(output))[f * 2 + 1] + (int32_t)((int16_t*)(activeSources[s]->data))[activeSources[s]->cur * 2 + 1] * activeSources[s]->volume;
				if (tempAudioLimiter > 32767)
					((int16_t*)(output))[f * 2 + 1] = 32767;
				else if (tempAudioLimiter < -32767)
					((int16_t*)(output))[f * 2 + 1] = -32767;
				else
					((int16_t*)(output))[f * 2 + 1] = tempAudioLimiter;
				
				// Check if reached the end of sound
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
						break;
					}
				}
			}
		}
		else if (activeSources[s]->channels == 1)
		{
			for (unsigned f = 0; f < framesPerBuffer; f++, activeSources[s]->cur++)
			{
				// Populate output, each channel separately
				tempAudioLimiter = (int32_t)((int16_t*)(output))[f * 2] + (int32_t)((int16_t*)(activeSources[s]->data))[activeSources[s]->cur] * activeSources[s]->volume;
				if (tempAudioLimiter > 32767)
					((int16_t*)(output))[f * 2] = 32767;
				else if (tempAudioLimiter < -32767)
					((int16_t*)(output))[f * 2] = -32767;
				else
					((int16_t*)(output))[f * 2] = tempAudioLimiter;
				tempAudioLimiter = (int32_t)((int16_t*)(output))[f * 2 + 1] + (int32_t)((int16_t*)(activeSources[s]->data))[activeSources[s]->cur] * activeSources[s]->volume;
				if (tempAudioLimiter > 32767)
					((int16_t*)(output))[f * 2 + 1] = 32767;
				else if (tempAudioLimiter < -32767)
					((int16_t*)(output))[f * 2 + 1] = -32767;
				else
					((int16_t*)(output))[f * 2 + 1] = tempAudioLimiter;
				
				// Check if reached the end of sound
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
						break;
					}
				}
			}
		}
	}

	return paContinue;
}

void Engine::InitializeAudio()
{
	Pa_Initialize();
	// Open PortAudio stream
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	Pa_OpenStream(&stream, NULL, &outputParameters, Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultSampleRate, config_framesPerBuffer, paNoFlag, Callback, nullptr);
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
	data = new char[dataSize]; // Resize data array to fit the data
	file.read((char*)data, dataSize);// Read the data into the data array

	dataSizeInSamples = dataSize / blockAlign;
	
	return true;
}

void Engine::AudioSource::Play(unsigned long start, unsigned long length, unsigned short loops, float volume)
{
	this->volume = volume;
	Log("Play");
	for (unsigned i = 0; i < activeSources.size(); i++)
	{
		if (activeSources[i] == this)
		{
			playing = false;
			return;
			if (length == 0)
				endpointToPlay = dataSizeInSamples;
			else	
				endpointToPlay = length + start;
			loopsToPlay = loops;
			cur = start;
			startPoint = cur;
			return;
		}
	}
	playing = true;
	if (length == 0)
		endpointToPlay = dataSizeInSamples;
	else	
		endpointToPlay = length + start;
	loopsToPlay = loops;
	cur = start;
	startPoint = cur;
	activeSources.push_back(this);
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