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
	struct Source
	{
		bool m_loaded = false;
		bool m_playing = false;
		
		int16_t* m_data = nullptr;

		uint32_t m_cur = 0UL;
		uint32_t m_frames = 0UL;
		uint32_t m_endpointToPlay = 0UL;
		uint32_t m_startPoint = 0UL;
		float m_volume = 1.0f;

		uint8_t m_channels = 0U;
		uint32_t m_dataSize = 0UL;

		std::function<void()> m_OnEnd;

		// For the play chain
		size_t m_nextSource;

		Source(const std::string& fileName, std::function<void()> OnEnd = nullptr);
		~Source();

		// Set the settings of the source to play.
		// This does not actually play the source. For that you need an AudioManager.
		void SetSettingsToPlay(uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);
	};

	Audio(uint16_t bufferSize = 256);
	~Audio();

	size_t CreateSource(const std::string& fileName, std::function<void()> OnEnd = nullptr);

	bool PlaySource(size_t index, uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);
	void PauseSource(size_t index);
	bool ResumeSource(size_t index);
	void StopSource(size_t index);

private:
	std::vector<Source> m_sources;
	size_t m_playChain;
	size_t m_chainLength = 0;
	const uint16_t m_bufferSize;
	int32_t* m_unlimitedOutput;

	int Callback(void* output);

	static int Callback(
		const void *input,
		void *output,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo *timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData
	);

	inline static bool m_paInitialized = false;
	PaStream* m_stream = nullptr;
	PaStreamParameters m_outputParameters;

	// This class and its static instance will automatically terminate PortAudio.
	// Its only way to know whether PortAudio was initialized is usign the static boolean `paInitialized` which, if `AudioStream`s were created, should be true.
	// Terminating isn't done in `AudioManager::~AudioManager()`, because there could be multiple `AudioManager`s, that are created and destoryed in different times.
	class PortAudioAutomaticTerminator
	{
		inline ~PortAudioAutomaticTerminator()
		{
			if (m_paInitialized)
				Pa_Terminate();
		}
	};
	static PortAudioAutomaticTerminator m_portAudioAutomaticTerminator;
};