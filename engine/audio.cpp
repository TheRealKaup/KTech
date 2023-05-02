// Link: XAudio2.lib
/*
#include "engine.h"

#pragma warning( disable : 6031 )

IXAudio2* Engine::xAudio2 = nullptr;
IXAudio2MasteringVoice* Engine::masterVoice = nullptr;

void Engine::InitializeAudio()
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	xAudio2->CreateMasteringVoice(&masterVoice);
}

Engine::AudioSource::AudioSource(LPCWSTR fileName)
{
	if (Engine::xAudio2 == nullptr || Engine::masterVoice == nullptr) {
		InitializeAudio();
		if (Engine::xAudio2 == nullptr || Engine::masterVoice == nullptr)
			return;
	}

	unsigned long temp = 0UL; // 4 bytes
	unsigned temp2 = 0UL; // 2 bytes
	unsigned long read = 0UL;
	
	file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (GetLastError() == 2) // Couldn't find file
		return;

	SetFilePointer(file, 0, NULL, FILE_BEGIN);

	// --== RIFF chunk ==--
	while (temp != 'FFIR') {// find the chunk
		ReadFile(file, &temp, 4, &read, NULL);
		SetFilePointer(file, -3, NULL, FILE_CURRENT);
	}
	SetFilePointer(file, 3, NULL, FILE_CURRENT);
	ReadFile(file, &fileSize, 4, &read, NULL); // Get the file size
	ReadFile(file, &temp, 4, &read, NULL); // Get the format ('WAVE' expected)

	// --== FMT chunk ==--
	while (temp != ' tmf') { // Find the chunk
		ReadFile(file, &temp, 4, &read, NULL);
		SetFilePointer(file, -3, NULL, FILE_CURRENT);
	}
	SetFilePointer(file, 3, NULL, FILE_CURRENT);
	ReadFile(file, &temp, 4, &read, NULL); // FMT Size -8 (16)
	if (temp != 16) // something something 16 is for PCM?
		return;
	ReadFile(file, &temp2, 2, &read, NULL); // Compression state
	if (temp2 != 1) // Other than 1 means some form of compression.
		return;
	ReadFile(file, &channels, 2, &read, NULL); // Channels
	ReadFile(file, &sampleRate, 4, &read, NULL); // Sample rate
	ReadFile(file, &byteRate, 4, &read, NULL); // Sample rate
	ReadFile(file, &blockAlign, 2, &read, NULL); // Block align
	ReadFile(file, &bitsPerSample, 2, &read, NULL); // Bits per sample

	// --== Data chunk ==--
	while (temp != 'atad') { // find the chunkXAudio2.lib;%(AdditionalDependencies)
		ReadFile(file, &temp, 4, &read, NULL);
		SetFilePointer(file, -3, NULL, FILE_CURRENT);
	}
	SetFilePointer(file, 3, NULL, FILE_CURRENT);
	ReadFile(file, &dataSize, 4, &read, NULL); // Data size
	data = new unsigned char[dataSize]; // Resize data array to fit the data
	ReadFile(file, data, dataSize, &read, NULL);// Read the data into the data array

	// --== XAudio2 ==--
	// Initialize "buffer"
	buffer.Flags = 0;
	buffer.AudioBytes = dataSize;
	buffer.pAudioData = data;
	// Initialize the source voice.
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channels;
	wfx.cbSize = sizeof(WAVEFORMATEX);
	wfx.nSamplesPerSec = sampleRate;
	wfx.nAvgBytesPerSec = byteRate;
	wfx.nBlockAlign = blockAlign;
	wfx.wBitsPerSample = bitsPerSample;
	// Initialize the source voice.
	Engine::xAudio2->CreateSourceVoice(&sourceVoice, &wfx);
}
 
void Engine::AudioSource::Play(unsigned long start, unsigned long length, unsigned short loops, std::vector<float> volumes)
{
	// Return if didn't manage to initialize.
	if (sourceVoice == nullptr)
		return;
	// Stop playing.
	sourceVoice->Stop();
	// Clean the current buffer, if exists or not.
	sourceVoice->FlushSourceBuffers();
	// Change volumes
	float* volumesArray = new float[channels];
	for (unsigned int i = 0; i < channels; i++)
		volumesArray[i] = (i < volumes.size() ? volumes[i] : (volumes.size() > 0 ? volumes[volumes.size() - 1] : 1.0f));
	sourceVoice->SetChannelVolumes(channels, volumesArray); 
	// If out of range (invalid) then play from start.
	if (start > buffer.AudioBytes * 8 / bitsPerSample)
		start = 0;
	// Start and length.
	buffer.PlayBegin = start;
	buffer.PlayLength = length;
	// Loop (this way works better than the integrated loop features).
	if (loops == 0)
		loops = 1;
	for (unsigned short i = 0; i < loops; i++)
		sourceVoice->SubmitSourceBuffer(&buffer);
	// Play.
	sourceVoice->Start();
}

void Engine::AudioSource::Pause()
{
	if (sourceVoice == nullptr)
		return;
	sourceVoice->Stop();
}

void Engine::AudioSource::Resume()
{
	if (sourceVoice == nullptr)
		return;
	sourceVoice->Start();
}

void Engine::AudioSource::Stop()
{
	if (sourceVoice == nullptr)
		return;
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
}

void Engine::AudioSource::ChangeVolume(float volume)
{
	float* volumesArray = new float[channels];
	for (unsigned int i = 0; i < channels; i++)
		volumesArray[i] = volume;
	sourceVoice->SetChannelVolumes(channels, volumesArray);
}

void Engine::AudioSource::ChangeVolumes(std::vector<float> volumes)
{
	float* volumesArray = new float[channels];
	for (unsigned int i = 0; i < channels; i++)
		volumesArray[i] = (i < volumes.size() ? volumes[i] : (volumes.size() > 0 ? volumes[volumes.size() - 1] : 1.0f));
	sourceVoice->SetChannelVolumes(channels, volumesArray);
}*/
