/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 Ethan Kaufman (AKA Kaup)

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

// Audio values: 16 bit depth, 2 channels, default (probably 44100) sample rate

static void Log(std::string string)
{
	std::cout << "\033[38;2;255;255;0m" << string << "\033[m" << std::endl << std::flush;
}
static void Log(std::string string, unsigned number)
{
	std::cout << "\033[38;2;255;255;0m" << string << number << "\033[m" << std::endl << std::flush;
}

int Callback(
	const void *input,
	void *output,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	KTech::Audio* audioManager = (KTech::Audio*)userData;

	// Clear unlimitedOutput
	memset(audioManager->unlimitedOutput, 0, audioManager->bufferSize * 8);

	// No audio sources, return
	if (audioManager->chainLength == 0)
		return paContinue;

	// There are audio sources, populate and send
	for (size_t s = audioManager->playChain; s != audioManager->sources[s].nextSource /*End of chain*/; s = audioManager->sources[s].nextSource)
	{
		// 2 channels
		if (audioManager->sources[s].channels == 2)
		{
			for (size_t f = 0; f < audioManager->bufferSize * 2; f+=2, audioManager->sources[s].cur++)
			{
				// Unregister audio source if reached its end
				if (audioManager->sources[s].cur >= audioManager->sources[s].endpointToPlay)
				{
					audioManager->sources[s].playing = false;
					if (audioManager->sources[s].OnEnd) // Callback
						audioManager->sources[s].OnEnd();
					s--;
					break;
				}
				// Otherwise, populate output buffer
				audioManager->unlimitedOutput[f] += audioManager->sources[s].data[audioManager->sources[s].cur * 2] * audioManager->sources[s].volume;
				audioManager->unlimitedOutput[f + 1] += audioManager->sources[s].data[audioManager->sources[s].cur * 2 + 1] * audioManager->sources[s].volume;
			}
		}
		// 1 channel
		else if (audioManager->sources[s].channels == 1)
		{
			for (size_t f = 0; f < audioManager->bufferSize * 2; f+=2, audioManager->sources[s].cur++)
			{				
				// Unregister audio source if reached its end
				if (audioManager->sources[s].cur >= audioManager->sources[s].endpointToPlay)
				{
					audioManager->sources[s].playing = false;
					if (audioManager->sources[s].OnEnd) // Callback
						audioManager->sources[s].OnEnd();
					s--;
					break;
				}
				// Otherwise, populate output buffer
				audioManager->unlimitedOutput[f] += audioManager->sources[s].data[audioManager->sources[s].cur] * audioManager->sources[s].volume;
				audioManager->unlimitedOutput[f + 1] += audioManager->sources[s].data[audioManager->sources[s].cur] * audioManager->sources[s].volume;
			}
		}
	}
	// At last, limit `unlimitedOutput` and add it to `output`.
	for (size_t f = 0; f < audioManager->bufferSize * 2; f++)
	{
		if (audioManager->unlimitedOutput[f] > 32767)
			((int16_t*)(output))[f] = 32767;
		else if (audioManager->unlimitedOutput[f] < -32767)
			((int16_t*)(output))[f] = -32767;
		else
			((int16_t*)(output))[f] = audioManager->unlimitedOutput[f];
	}
	return paContinue;
}

KTech::Audio::Audio(uint16_t bufferSize) : bufferSize(bufferSize)
{
	unlimitedOutput = new int32_t[bufferSize * 2];
	// Initialize PortAudio if it isn't already initialized
	if (!paInitialized)
	{
		Pa_Initialize();
		paInitialized = true;
	}
	// Open PortAudio stream
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	Pa_OpenStream(&stream, NULL, &outputParameters, Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultSampleRate, bufferSize, paClipOff, Callback, this);
	// Start the stream
	Pa_StartStream(stream);
}

KTech::Audio::~Audio()
{
	Pa_AbortStream(stream);
}

KTech::Audio::Source::Source(const std::string& fileName, std::function<void()> OnEnd)
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
	file.read((char*)&temp, 4); 
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
	file.read((char*)&temp, 4); // Sample rate
	file.read((char*)&temp, 4); // Byte rate
	int16_t blockAlign;
	file.read((char*)&blockAlign, 2); // Block align
	file.read((char*)&temp, 2); // Bits per sample

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

	frames = dataSize / temp;
	
	loaded = true;
}

KTech::Audio::Source::~Source()
{
	delete [] data; 
}

void KTech::Audio::Source::SetSettingsToPlay(uint32_t start, uint32_t length, float volume)
{
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
}

size_t KTech::Audio::CreateSource(const std::string& fileName, std::function<void()> OnEnd)
{
	// Create the source and push it 
	sources.push_back(Source(fileName, OnEnd));
	return sources.size() - 1;
}

bool KTech::Audio::PlaySource(size_t i, uint32_t start, uint32_t length, float volume)
{
	// Not loaded
	if (!sources[i].loaded)
		return false;
	// Iterate playChain
	for (size_t node = playChain;; node = sources[node].nextSource)
	{
		// Already playing
		if (node == i)
		{
			// Reset play
			sources[i].SetSettingsToPlay(start, length, volume);
			return true;
		}
		// Reached end (not already playing)
		else if (node == sources[node].nextSource)
		{
			// Set to play and add to play chain
			sources[i].SetSettingsToPlay(start, length, volume);
			sources[node].nextSource = i; // Add to chain
			sources[i].nextSource = i; // If a node's next node is itself, then that node is the end of the chain
			break;
			chainLength++;
		}
	}
	return true;
}

void KTech::Audio::PauseSource(size_t i)
{
	// Not loaded
	if (!sources[i].loaded)
		return;
	// Pause
	sources[i].playing = false;
	// Iterate playChain
	for (size_t node = playChain; node != sources[node].nextSource /*Reached end*/; node = sources[node].nextSource)
	{
		// The next source is the one being paused
		if (sources[node].nextSource == i)
		{
			// The node being removed is the last node in the chain
			if (sources[i].nextSource == i)
				sources[node].nextSource = node; // Make this node the last node
			else // Otherwise, there is a node after the one being removed
				sources[node].nextSource = sources[i].nextSource; // Set the next node to the one after the node being removed
			chainLength--;
			break;
		}
	}
}

bool KTech::Audio::ResumeSource(size_t i)
{
	// Not loaded
	if (!sources[i].loaded)
		return false;
	// Iterate playChain
	for (size_t node = playChain;; node = sources[node].nextSource)
	{
		// Already playing
		if (node == i)
		{
			sources[i].playing = true; // Resume
			break;
		}
		// Reached end (not already playing)
		else if (node == sources[node].nextSource)
		{
			sources[i].playing = true; // Resume
			sources[node].nextSource = i; // Add to chain
			sources[i].nextSource = i; // If a node's next node is itself, then that node is the end of the chain
			chainLength++;
			break;
		}
	}
	return true;
}

void KTech::Audio::StopSource(size_t i)
{
	// Not loaded
	if (!sources[i].loaded)
		return;
	// Pause
	sources[i].playing = false;
	sources[i].cur = 0;
	// Iterate playChain
	for (size_t node = playChain; node != sources[node].nextSource /*Reached end*/; node = sources[node].nextSource)
	{
		// The next source is the one being paused
		if (sources[node].nextSource == i)
		{
			// The node being removed is the last node in the chain
			if (sources[i].nextSource == i)
				sources[node].nextSource = node; // Make this node the last node
			else // Otherwise, there is a node after the one being removed
				sources[node].nextSource = sources[i].nextSource; // Set the next node to the one after the node being removed
			chainLength--;
		}
	}
}
