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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION

#include <string>
#include <functional>
#include "portaudio.h"

// Audio manager, creates its own audio stream using the PortAudio library, after making sure it is initialized.
class KTech::Audio
{
public:
	class Source
	{
	public:
		bool loaded = false;
		bool playing = false;

		uint32_t cur = 0UL;
		uint32_t frames = 0UL;
		uint32_t endpointToPlay = 0UL;
		uint32_t startPoint = 0UL;
		float volume = 1.0f;

		uint8_t channels = 0U;
		uint32_t dataSize = 0UL;

		int16_t* data = nullptr;

		std::function<void()> OnEnd;

		// For the play chain
		size_t nextSource;

		// Set the settings of the source to play.
		// This does not actually play the source. For that you need an AudioManager.
		void SetSettingsToPlay(uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);

		Source(const std::string& fileName, std::function<void()> OnEnd = nullptr);
		~Source();
	};

	// Instances - not pointers!
	std::vector<Source> sources;

	// Chain of the playing sources. Made to replace iterating through all of the sources in the callback.
	size_t playChain;
	size_t chainLength = 0;

	// Creates a source, stores it in `sources`, returns index (not UUID yet, that will be in the future)
	size_t CreateSource(const std::string& fileName, std::function<void()> OnEnd = nullptr);

	bool PlaySource(size_t index, uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);
	void PauseSource(size_t index);
	bool ResumeSource(size_t index);
	void StopSource(size_t index);

	const uint16_t bufferSize;
	int32_t* unlimitedOutput;

	Audio(uint16_t bufferSize = 256);
	
	~Audio();

private:
	inline static bool paInitialized = false;
	PaStream* stream = nullptr;
	PaStreamParameters outputParameters;
	int32_t tempAudioLimiter = 0;

	// This class and its static instance will automatically terminate PortAudio.
	// Its only way to know whether PortAudio was initialized is usign the static boolean `paInitialized` which, if `AudioStream`s were created, should be true.
	// Terminating isn't done in `AudioManager::~AudioManager()`, because there could be multiple `AudioManager`s, that are created and destoryed in different times.
	class PortAudioAutomaticTerminator { inline ~PortAudioAutomaticTerminator() { if (paInitialized) Pa_Terminate(); } };
	static PortAudioAutomaticTerminator portAudioAutomaticTerminator;
};