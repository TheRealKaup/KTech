/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

	This file is part of KTech.

	KTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	KTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KTech. If not, see <https://www.gnu.org/licenses/>.
*/

#include "ktech.hpp"

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

static PaStream* stream = nullptr;

// default device info
static PaStreamParameters outputParameters;

static std::vector<Engine::AudioSource*> activeSources;
static int32_t tempAudioLimiter = 0;

static int32_t unlimitedOutput[config_framesPerBuffer * 2];

int Callback(
	const void *input,
	void *output,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	static size_t f;

	// Set unlimitedOutput to the first source (which saves memsetting it)
	if (activeSources.size() > 0)
	{
		size_t s = 1;
		// Set to first source (which saves a memset at the start)
		if (activeSources[0]->channels == 2)
		{
			for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[0]->cur++)
			{
				// Unregister audio source if reached its end
				if (activeSources[0]->cur >= activeSources[0]->endpointToPlay)
				{
					activeSources[0]->playing = false;
					if (activeSources[0]->OnEnd) // Callback
						activeSources[0]->OnEnd();
					activeSources.erase(activeSources.begin());
					memset(unlimitedOutput + f, 0, (config_framesPerBuffer * 2 - f) * 4);
					s--; // First audio source didn't last until the end of the buffer
					break;
				}
				// Otherwise, populate output buffer
				unlimitedOutput[f] = activeSources[0]->data[activeSources[0]->cur * 2] * activeSources[0]->volume;
				unlimitedOutput[f + 1] = activeSources[0]->data[activeSources[0]->cur * 2 + 1] * activeSources[0]->volume;
			}
		}
		else if (activeSources[0]->channels == 1)
		{
			for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[0]->cur++)
			{				
				// Unregister audio source if reached its end
				if (activeSources[0]->cur >= activeSources[0]->endpointToPlay)
				{
					activeSources[0]->playing = false;
					if (activeSources[0]->OnEnd) // Callback
						activeSources[0]->OnEnd();
					activeSources.erase(activeSources.begin());
					memset(unlimitedOutput + f, 0, (config_framesPerBuffer * 2 - f) * 4);
					s--; // First audio source didn't last until the end of the buffer
					break;
				}
				// Otherwise, populate output buffer
				unlimitedOutput[f] = activeSources[0]->data[activeSources[0]->cur] * activeSources[0]->volume;
				unlimitedOutput[f + 1] = activeSources[0]->data[activeSources[0]->cur] * activeSources[0]->volume;
			}
		}
		// Add the other sources
		for (; s < activeSources.size(); s++)
		{
			if (activeSources[s]->channels == 2)
			{
				for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[s]->cur++)
				{
					// Unregister audio source if reached its end
					if (activeSources[s]->cur >= activeSources[s]->endpointToPlay)
					{
						activeSources[s]->playing = false;
						if (activeSources[s]->OnEnd) // Callback
							activeSources[s]->OnEnd();
						activeSources.erase(activeSources.begin() + s);
						s--;
						break;
					}
					// Otherwise, populate output buffer
					unlimitedOutput[f] += activeSources[s]->data[activeSources[s]->cur * 2] * activeSources[s]->volume;
					unlimitedOutput[f + 1] += activeSources[s]->data[activeSources[s]->cur * 2 + 1] * activeSources[s]->volume;
				}
			}
			else if (activeSources[s]->channels == 1)
			{
				for (f = 0; f < config_framesPerBuffer * 2; f+=2, activeSources[s]->cur++)
				{				
					// Unregister audio source if reached its end
					if (activeSources[s]->cur >= activeSources[s]->endpointToPlay)
					{
						activeSources[s]->playing = false;
						if (activeSources[s]->OnEnd) // Callback
							activeSources[s]->OnEnd();
						activeSources.erase(activeSources.begin() + s);
						s--;
						break;
					}
					// Otherwise, populate output buffer
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
	// Start the stream
	Pa_StartStream(stream);

	audioInitialized = true;
}
void Engine::TerminateAudio()
{
	Pa_AbortStream(stream);
	Pa_Terminate();
}

Engine::AudioSource::AudioSource(const std::string& fileName, std::function<void()> OnEnd)
	: OnEnd(OnEnd)
{
	int32_t temp = 0UL;
	int16_t temp2 = 0UL;
	
	std::ifstream file(fileName, std::ifstream::binary);

	// Couldn't open file
	if (!file.is_open())
	{
		loaded = false;
		return;
	}
	
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
	{
		loaded = false;
		return;
	}
	file.read((char*)&temp2, 2); // Compression state
	if (temp2 != 1) // Other than 1 means some form of compression.
	{
		loaded = false;
		return;
	}
	file.read((char*)&channels, 2); // Channels
	file.read((char*)&sampleRate, 4); // Sample rate
	file.read((char*)&byteRate, 4); // Byte rate
	file.read((char*)&blockAlign, 2); // Block align
	file.read((char*)&bitsPerSample, 2); // Bits per sample
	
	Log("SampleRate=", sampleRate);

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
	// Get it ourselves in case it's too large
	unsigned startPos = file.tellg(); // Get the current position
	file.seekg(0, std::ifstream::end); // Move to the end
	unsigned endPos = file.tellg(); // Get the last position
	if (dataSize > endPos - startPos) // Correct the size only if the written data size is too large
		dataSize = endPos - startPos; // Correct the size
	file.seekg(startPos, std::ifstream::beg); // Return to the last position
	data = new int16_t[dataSize / 2]; // Resize data array to fit the data
	file.read((char*)data, dataSize);// Read the data into the data array

	frames = dataSize / blockAlign;
	
	loaded = true;
}

bool Engine::AudioSource::Play(uint32_t start, uint32_t length, float volume)
{
	if (!loaded)
		return false;
	playing = true;
	this->volume = volume;
	if (length == 0)
		endpointToPlay = frames;
	else if (length > (dataSize / channels))
		length = frames;
	else
		endpointToPlay = length + start;
	cur = start;
	startPoint = cur;
	for (unsigned i = 0; i < activeSources.size(); i++)
		if (activeSources[i] == this)
			return true; // Don't add to activeSources if already there.
	activeSources.push_back(this);
	return true;
}

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