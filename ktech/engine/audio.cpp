/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2024 Ethan Kaufman (AKA Kaup)

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

#include "audio.hpp"

#include <cstring>
#include <fstream>

// Audio values: 16 bit depth, 2 channels, default (probably 44100) sample rate

int KTech::Audio::Callback(void* output)
{
// Clear unlimitedOutput
	memset(m_unlimitedOutput, 0, m_bufferSize * 8);

	// No audio sources, return
	if (m_chainLength == 0)
		return paContinue;

	// There are audio sources, populate and send
	for (size_t s = m_playChain; s != m_sources[s].m_nextSource /*End of chain*/; s = m_sources[s].m_nextSource)
	{
		// 2 channels
		if (m_sources[s].m_channels == 2)
		{
			for (size_t f = 0; f < m_bufferSize * 2; f+=2, m_sources[s].m_cur++)
			{
				// Unregister audio source if reached its end
				if (m_sources[s].m_cur >= m_sources[s].m_endpointToPlay)
				{
					m_sources[s].m_playing = false;
					if (m_sources[s].m_OnEnd) // Callback
						m_sources[s].m_OnEnd();
					s--;
					break;
				}
				// Otherwise, populate output buffer
				m_unlimitedOutput[f] += m_sources[s].m_data[m_sources[s].m_cur * 2] * m_sources[s].m_volume;
				m_unlimitedOutput[f + 1] += m_sources[s].m_data[m_sources[s].m_cur * 2 + 1] * m_sources[s].m_volume;
			}
		}
		// 1 channel
		else if (m_sources[s].m_channels == 1)
		{
			for (size_t f = 0; f < m_bufferSize * 2; f+=2, m_sources[s].m_cur++)
			{				
				// Unregister audio source if reached its end
				if (m_sources[s].m_cur >= m_sources[s].m_endpointToPlay)
				{
					m_sources[s].m_playing = false;
					if (m_sources[s].m_OnEnd) // Callback
						m_sources[s].m_OnEnd();
					s--;
					break;
				}
				// Otherwise, populate output buffer
				m_unlimitedOutput[f] += m_sources[s].m_data[m_sources[s].m_cur] * m_sources[s].m_volume;
				m_unlimitedOutput[f + 1] += m_sources[s].m_data[m_sources[s].m_cur] * m_sources[s].m_volume;
			}
		}
	}
	// At last, limit `unlimitedOutput` and add it to `output`.
	for (size_t f = 0; f < m_bufferSize * 2; f++)
	{
		if (m_unlimitedOutput[f] > 32767)
			((int16_t*)(output))[f] = 32767;
		else if (m_unlimitedOutput[f] < -32767)
			((int16_t*)(output))[f] = -32767;
		else
			((int16_t*)(output))[f] = m_unlimitedOutput[f];
	}
	return paContinue;
}

int KTech::Audio::Callback(
	const void* input,
	void* output,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	// Done this way as required variable members are private, to fit code conventions. 
	return ((KTech::Audio*)userData)->Callback(output);
}

KTech::Audio::Audio(uint16_t bufferSize) : m_bufferSize(bufferSize)
{
	m_unlimitedOutput = new int32_t[bufferSize * 2];
	// Initialize PortAudio if it isn't already initialized
	if (!m_paInitialized)
	{
		Pa_Initialize();
		m_paInitialized = true;
	}
	// Open PortAudio stream
	m_outputParameters.device = Pa_GetDefaultOutputDevice();
	m_outputParameters.channelCount = 2;
	m_outputParameters.sampleFormat = paInt16;
	m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultHighOutputLatency;
	m_outputParameters.hostApiSpecificStreamInfo = NULL;
	Pa_OpenStream(&m_stream, NULL, &m_outputParameters, Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultSampleRate, bufferSize, paClipOff, Callback, this);
	// Start the stream
	Pa_StartStream(m_stream);
}

KTech::Audio::~Audio()
{
	Pa_AbortStream(m_stream);
}

KTech::Audio::Source::Source(const std::string& fileName, std::function<void()> OnEnd)
	: m_OnEnd(OnEnd)
{

	int32_t temp = 0UL;
	int16_t temp2 = 0UL;
	
	std::ifstream file(fileName, std::ifstream::binary);

	// Couldn't open file
	if (!file.is_open())
	{
		m_loaded = false;
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
		m_loaded = false;
		return;
	}
	file.read((char*)&temp2, 2); // Compression state
	if (temp2 != 1) // Other than 1 means some form of compression.
	{
		m_loaded = false;
		return;
	}
	file.read((char*)&m_channels, 2); // Channels
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
	file.read((char*)&m_dataSize, 4); // Data size
	// Get it ourselves in case it's too large
	unsigned startPos = file.tellg(); // Get the current position
	file.seekg(0, std::ifstream::end); // Move to the end
	unsigned endPos = file.tellg(); // Get the last position
	if (m_dataSize > endPos - startPos) // Correct the size only if the written data size is too large
		m_dataSize = endPos - startPos; // Correct the size
	file.seekg(startPos, std::ifstream::beg); // Return to the last position
	m_data = new int16_t[m_dataSize / 2]; // Resize data array to fit the data
	file.read((char*)m_data, m_dataSize);// Read the data into the data array

	m_frames = m_dataSize / temp;
	
	m_loaded = true;
}

KTech::Audio::Source::~Source()
{
	delete [] m_data; 
}

void KTech::Audio::Source::SetSettingsToPlay(uint32_t start, uint32_t length, float volume)
{
	m_playing = true;
	this->m_volume = volume;
	if (length == 0)
		m_endpointToPlay = m_frames;
	else if (length > (m_dataSize / m_channels))
		length = m_frames;
	else
		m_endpointToPlay = length + start;
	m_cur = start;
	m_startPoint = m_cur;
}

size_t KTech::Audio::CreateSource(const std::string& fileName, std::function<void()> OnEnd)
{
	// Create the source and push it 
	m_sources.push_back(Source(fileName, OnEnd));
	return m_sources.size() - 1;
}

bool KTech::Audio::PlaySource(size_t i, uint32_t start, uint32_t length, float volume)
{
	// Not loaded
	if (!m_sources[i].m_loaded)
		return false;
	// Iterate m_playChain
	for (size_t node = m_playChain;; node = m_sources[node].m_nextSource)
	{
		// Already playing
		if (node == i)
		{
			// Reset play
			m_sources[i].SetSettingsToPlay(start, length, volume);
			return true;
		}
		// Reached end (not already playing)
		else if (node == m_sources[node].m_nextSource)
		{
			// Set to play and add to play chain
			m_sources[i].SetSettingsToPlay(start, length, volume);
			m_sources[node].m_nextSource = i; // Add to chain
			m_sources[i].m_nextSource = i; // If a node's next node is itself, then that node is the end of the chain
			break;
			m_chainLength++;
		}
	}
	return true;
}

void KTech::Audio::PauseSource(size_t i)
{
	// Not loaded
	if (!m_sources[i].m_loaded)
		return;
	// Pause
	m_sources[i].m_playing = false;
	// Iterate m_playChain
	for (size_t node = m_playChain; node != m_sources[node].m_nextSource /*Reached end*/; node = m_sources[node].m_nextSource)
	{
		// The next source is the one being paused
		if (m_sources[node].m_nextSource == i)
		{
			// The node being removed is the last node in the chain
			if (m_sources[i].m_nextSource == i)
				m_sources[node].m_nextSource = node; // Make this node the last node
			else // Otherwise, there is a node after the one being removed
				m_sources[node].m_nextSource = m_sources[i].m_nextSource; // Set the next node to the one after the node being removed
			m_chainLength--;
			break;
		}
	}
}

bool KTech::Audio::ResumeSource(size_t i)
{
	// Not loaded
	if (!m_sources[i].m_loaded)
		return false;
	// Iterate m_playChain
	for (size_t node = m_playChain;; node = m_sources[node].m_nextSource)
	{
		// Already playing
		if (node == i)
		{
			m_sources[i].m_playing = true; // Resume
			break;
		}
		// Reached end (not already playing)
		else if (node == m_sources[node].m_nextSource)
		{
			m_sources[i].m_playing = true; // Resume
			m_sources[node].m_nextSource = i; // Add to chain
			m_sources[i].m_nextSource = i; // If a node's next node is itself, then that node is the end of the chain
			m_chainLength++;
			break;
		}
	}
	return true;
}

void KTech::Audio::StopSource(size_t i)
{
	// Not loaded
	if (!m_sources[i].m_loaded)
		return;
	// Pause
	m_sources[i].m_playing = false;
	m_sources[i].m_cur = 0;
	// Iterate m_playChain
	for (size_t node = m_playChain; node != m_sources[node].m_nextSource /*Reached end*/; node = m_sources[node].m_nextSource)
	{
		// The next source is the one being paused
		if (m_sources[node].m_nextSource == i)
		{
			// The node being removed is the last node in the chain
			if (m_sources[i].m_nextSource == i)
				m_sources[node].m_nextSource = node; // Make this node the last node
			else // Otherwise, there is a node after the one being removed
				m_sources[node].m_nextSource = m_sources[i].m_nextSource; // Set the next node to the one after the node being removed
			m_chainLength--;
		}
	}
}
