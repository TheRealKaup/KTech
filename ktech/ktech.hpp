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

/*        
	  |  /  ----- ----- ----- -   -  
	 || /\    |   |---  |     |---| 
	|||/  \   |   \____ \____ |   |  kaup's c++ 2d terminal game engine library

	In order to contact me (Kaup), refer to the "readme.md" file.
*/

#pragma once

// Standard libraries
#include <iostream> // Printing, strings, et cetera
#include <string> // to_string
#include <vector> // Heavily based
#include <functional> // For sharing functions with the engine for it to call on events
#include <thread> // For sleeping, time management.
#include <chrono> // Time management
#include <fstream> // Reading textures and audio files
#include <cstring> // strcmp
#include <stdint.h> // int32_t
// Linux libraries
#include <signal.h> // For getting signals (like on quit)
#include <termios.h> // For changing terminal attributes
#include <sys/ioctl.h> // Contains some data types (winsize for termios.h)
// Cross-platform libraries
#include <portaudio.h> // PortAudio is a cross-platform audio library

namespace KTech
{
	struct RGB;
	struct RGBA;
	struct Point;
	struct UPoint;
	struct Cell;
	struct CellA;
	struct Texture;
	struct Collider;
	struct Object;
	struct Layer;
	struct Camera;
	struct Map;
	class Engine;

	struct RGB
	{
		uint8_t r, g, b;
		inline constexpr RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) : r(red), g(green), b(blue) {}
	};

	struct RGBA
	{
		uint8_t r, g, b, a;
		inline constexpr RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
		inline constexpr RGBA(RGB rgb, uint8_t alpha) : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha) {}
	};

	struct Point
	{
		int32_t x, y;
		constexpr inline Point(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}
	};

	struct UPoint
	{
		uint32_t x, y;
		constexpr inline UPoint(uint32_t x = 0, uint32_t y = 0) : x(x), y(y) {}
	};

	struct Cell
	{
		RGB f;
		RGB b;
		char c;
		constexpr inline Cell(char character = ' ', RGB foreground = RGB(0, 0, 0), RGB background = RGB(0, 0, 0)) : c(character), f{foreground}, b(background) {}
	};

	struct CellA
	{
		RGBA f;
		RGBA b;
		char c;
		constexpr inline CellA(char character = ' ', RGBA foreground = RGBA(0, 0, 0, 0), RGBA background = RGBA(0, 0, 0, 0)) : c(character), f{foreground}, b(background) {}
	};

	struct Texture
	{
		bool active = true;
		Point pos_r = {0, 0};
		bool simple = true;

		UPoint size = { 0, 0 };
		CellA value = CellA('#', RGBA());

		std::vector<std::vector<CellA>> t = {}; 
		
		void Simple(UPoint size, CellA value, Point relative_position);
		void Rectangle(UPoint size, CellA value, Point relative_position);// Load from a file.
		UPoint File(const std::string& fileName, Point relative_position);
		void Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point relative_position);

		void Resize(UPoint newSize, CellA newValue);
		void SetCell(CellA value);
		void SetForeground(RGBA value);
		void SetBackground(RGBA value);
		void SetCharacter(char value);
		void SetAlpha(uint8_t value);
		
		UPoint GetSize() const;
		void ExportToFile(const std::string& fileName) const;
	};

	struct Collider
	{
		bool active = true;
		Point pos_r = { 0, 0 };
		bool simple = true;
		uint8_t type = 0;

		UPoint size = { 0, 0 };
		
		std::vector<std::vector<bool>> c = {};

		void Simple(UPoint size, uint8_t type, Point relative_position);
		bool File(const std::string& fileName, uint8_t type, Point relative_position);
		void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relative_position);
		void ByTextureCharacter(const Texture& texture, uint8_t alphaThreshold, uint8_t type);
		void ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t type);

		UPoint GetSize() const;
	};

	struct Object
	{
		Layer* parentLayer = nullptr;

		std::string name = "";
		Point pos = { 0, 0 };

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		Point lastPush = { 0, 0 };
		size_t colliderIndex = -1;
		Object* otherObject = nullptr;
		size_t otherColliderIndex = -1;

		enum class EventType : uint8_t
		{
			onTick, // Called by `Map::CallOnTicks()`.
			onPushed, // A different object (`otherObject`) pushed this object.
			onPush, // This object pushed a different object (`otherObject`)
			onBlocked, // A different object (`otherObject`) blocked this object
			onBlock, // This object blocked a different object (`otherObject`)
			onOverlap, // This object entered an overlap with a different object (`otherObject`)
			onOverlapExit, // This object exited an overlap with a different object (`otherObject`)
			onOverlapped, // A different object (`otherObject`) entered an overlap with this object
			onOverlappedExit // A different object (`otherObject`) exited an overlap with this object
		};
		std::function<void(EventType)> OnEvent = nullptr;
		bool Move(Point dir);
		
		void EnterLayer(Layer* layer);
		Object(Point position = Point(0, 0), Layer* layer = nullptr, const std::string& objectName = "");

		~Object();
	};

	struct Layer
	{
		std::function<void()> OnTick = nullptr;
		
		std::vector<Object*> objects = {};
		
		bool visible = true;
		uint8_t alpha = 255;
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };

		int AddObject(Object* object);
		bool RemoveObject(const std::string& name);
		bool RemoveObject(Object* object);
		~Layer();
	};
	
	struct Camera
	{
		std::string name = "";
		Point pos = Point(0, 0);
		UPoint res = UPoint(10, 10);
		std::vector<std::vector<Cell>> image = {};

		std::function<void()> OnTick = nullptr;

		Camera(Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
		
		void Render(const std::vector<Layer*>& layers);
		void Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0) const;
		void Resize(UPoint res);
	};
	
	struct Map
	{
	public:
		std::vector<Camera*> cameras = {};
		std::vector<Layer*> layers = {};
		size_t activeCameraI = -1;

		std::function<void()> OnTick = NULL;

		int AddCamera(Camera* camera, bool asActiveCamera = false);
		int AddLayer(Layer* layer);

		bool Render() const;
		bool RenderReversed() const;
		bool Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0) const;
		void CallOnTicks() const;
	};

	struct AudioSource
	{
		bool loaded;

		PaStream* stream;
		uint32_t cur = 0UL;
		uint32_t frames = 0UL;
		uint32_t endpointToPlay = 0UL;
		uint32_t startPoint = 0UL;
		float volume = 1.0f;
		bool playing = false;

		uint32_t fileSize = 0UL;
		uint8_t channels = 0U;
		uint32_t sampleRate = 0UL;
		uint32_t byteRate = 0UL;
		uint8_t blockAlign = 0U;
		uint16_t bitsPerSample = 0U;
		uint32_t dataSize = 0UL;

		int16_t* data = nullptr;

		std::function<void()> OnEnd;

		AudioSource(const std::string& fileName, std::function<void()> OnEnd = nullptr);

		bool Play(uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);
		void Pause();
		void Resume();
		void Stop();
	};

	// Collision Result
	enum class CR : uint8_t
	{
		B,	// Block
		P,	// Push
		O	// Overlap
	};

	// Manager of all things that directly work with terminal I/O.
	class IOManager
	{
	public:
		// Prepares terminal, creates input loop thread
		IOManager();
		// Resets terminal
		~IOManager();

		termios oldTerminalAttributes;
		
		winsize terminalSize;
		std::vector<std::vector<Cell>> image;
		std::string stringImage;

		void PrintStartupNotice(const std::string& title, const std::string& years, const std::string author, const std::string programName);
		void PrintStartupNotice(const std::string& text);
			
		void Print();

		void Log(const std::string& text, RGB color);

		std::string& Get();

		struct Callback
		{
			bool enabled = true;
			std::function<void()> callback;
			inline Callback(const std::function<void()>& callback) : callback(callback) {}
		};

		struct BasicHandler
		{
			struct BasicCallback : Callback
			{
				BasicHandler* parentHandler;
				bool onTick;
				inline BasicCallback(const std::function<void()>& callback, BasicHandler* parentHandler, bool onTick) : Callback(callback), parentHandler(parentHandler), onTick(onTick) {}
			};
			inline static std::vector<BasicHandler*> handlers; // Handlers are stored as pointers because the vector changes its size, and Callbacks need a consistent pointer to their parent handler.
			std::vector<BasicCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
			std::string input;
			uint8_t timesPressed = 0;
			inline BasicHandler(const std::string& input) : input(input) {};
		};

		struct RangedHandler
		{
			struct RangedCallback : Callback
			{
				RangedHandler* parentHandler;
				inline RangedCallback(const std::function<void()>& callback, RangedHandler* parentHandler) : Callback(callback), parentHandler(parentHandler) {}
			};
			inline static std::vector<RangedHandler*> handlers; // Handlers are stored as pointers because the vector changes its size, and Callbacks need a consistent pointer to their parent handler.
			std::vector<RangedCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
			char key1, key2;
			inline RangedHandler(char key1, char key2) : key1(key1), key2(key2) {};
		};

		struct CallbacksGroup
		{
			inline static std::vector<CallbacksGroup*> groups;

			std::vector<BasicHandler::BasicCallback*> basicCallbacks;
			std::vector<RangedHandler::RangedCallback*> rangedCallbacks;
			bool enabled;
			bool synced = true;
			inline CallbacksGroup(bool enabled = true) : enabled(enabled) { groups.push_back(this); }
			inline ~CallbacksGroup() { for (size_t i = 0; i < groups.size(); i++) if (groups[i] == this) groups.erase(groups.begin() + i); }
			inline void AddCallback(BasicHandler::BasicCallback* callback) { basicCallbacks.push_back(callback); callback->enabled = enabled; }
			inline void AddCallback(RangedHandler::RangedCallback* callback) { rangedCallbacks.push_back(callback); callback->enabled = enabled; }
			inline void Enable() { enabled = true; synced = false; }
			inline void Disable() { enabled = false; synced = false; }
			void DeleteCallbacks();
		};

		BasicHandler::BasicCallback* RegisterCallback(const std::string& stringKey, const std::function<void()>& callback, bool onTick = false);
		RangedHandler::RangedCallback* RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback);

		std::string input;
		std::string quitKey = "\x03";

		void Call();
		void Loop();

		bool Is(const std::string& stringKey);
		bool Is(char charKey);
		bool Bigger(char charKey);
		bool Smaller(char charKey);
		bool Between(char charKey1, char charKey2);
		uint8_t GetInt();
	};

	// Time manager
	class TimeManager
	{
	public:
		enum class Measurement
		{
			ticks,
			seconds,
			milliseconds,
			microseconds
		};

		struct TimePoint
		{
			std::chrono::high_resolution_clock::time_point chronoTimePoint;
			void SetToNow();
			long GetInt(Measurement timeMeasurement) const;
			TimePoint();
		};

		const TimePoint startTP;
		TimePoint thisTickStartTP;

		int16_t tpsLimit = 24;
		float tps;
		float tpsPotential;
		int32_t deltaTime;
		int32_t ticksCounter = 0;
		
		inline void StartThisTick() { thisTickStartTP.SetToNow(); }
		void WaitUntilNextTick();
		
		struct Invocation
		{
			inline static std::vector<Invocation*> invocations;
			std::function<void()> callback;
			uint32_t ticksLeft; // Current ticks left in this instance
			inline Invocation(const std::function<void()>& callback, uint32_t ticks)
				: callback(callback), ticksLeft(ticks) {}
		};
		Invocation* Invoke(const std::function<void()>& callback, uint32_t time, Measurement timeMeasurement);
		bool CancelInvocation(Invocation* invocation);
		void CallInvocations();

		// 
		TimeManager(int16_t tps);
		
		~TimeManager();
	};

	class AudioManager
	{
	public:
		AudioManager();
		~AudioManager();
	};

	// Core class, the user should create one of these, and shouldn't have to construct themselves 
	// a system using the other namespaces themselves
	class Engine
	{
	public:
		bool running = true;

		IOManager io;
		TimeManager time;
		AudioManager audio;

		std::vector<std::vector<CR>> colliderTypes = {
			{ CR::B, CR::P, CR::O }, // Heavy - 0
			{ CR::B, CR::P, CR::O }, // Normal - 1
			{ CR::O, CR::O, CR::O } // Overlappable - 2
		};

		Engine(int16_t tps = 24) : time(TimeManager(tps))
		{
		}

		~Engine()
		{
		}
	};

	namespace RGBColors {}
	namespace RGBAColors {}
}

namespace KTech::RGBColors
{
	constexpr RGB red = RGB(255, 0, 0);
	constexpr RGB orange = RGB(255, 128, 0);
	constexpr RGB yellow = RGB(255, 255, 0);
	constexpr RGB lime = RGB(128, 255, 0);
	constexpr RGB green = RGB(0, 255, 0);
	constexpr RGB mint = RGB(0, 255, 128);
	constexpr RGB cyan = RGB(0, 255, 255);
	constexpr RGB blue = RGB(0, 128, 255);
	constexpr RGB primaryBlue = RGB(0, 0, 255);
	constexpr RGB purple = RGB(128, 0, 255);
	constexpr RGB pink = RGB(255, 0, 255);
	constexpr RGB hotPink = RGB(255, 0, 128);
	constexpr RGB white = RGB(255, 255, 255);
	constexpr RGB gray = RGB(160, 160, 160);
	constexpr RGB black = RGB(0, 0, 0);
}

namespace KTech::RGBAColors
{
	constexpr RGBA red = RGBA(255, 0, 0, 255);
	constexpr RGBA orange = RGBA(255, 128, 0, 255);
	constexpr RGBA yellow = RGBA(255, 255, 0, 255);
	constexpr RGBA lime = RGBA(128, 255, 0, 255);
	constexpr RGBA green = RGBA(0, 255, 0, 255);
	constexpr RGBA mint = RGBA(0, 255, 128, 255);
	constexpr RGBA cyan = RGBA(0, 255, 255, 255);
	constexpr RGBA blue = RGBA(0, 128, 255, 255);
	constexpr RGBA primaryBlue = RGBA(0, 0, 255, 255);
	constexpr RGBA purple = RGBA(128, 0, 255, 255);
	constexpr RGBA pink = RGBA(255, 0, 255, 255);
	constexpr RGBA hotPink = RGBA(255, 0, 128, 255);
	constexpr RGBA white = RGBA(255, 255, 255, 255);
	constexpr RGBA gray = RGBA(160, 160, 160, 255);
	constexpr RGBA black = RGBA(0, 0, 0, 255);
	constexpr RGBA transparent = RGBA(0, 0, 0, 0);
}

namespace KTech::Input::K
{
	constexpr char const* up = "\x1b[A";
	constexpr char const* down = "\x1b[B";
	constexpr char const* right = "\x1b[C";
	constexpr char const* left = "\x1b[D";
	constexpr char const* return_ = "\x0a";
	constexpr char const* backspace = "\x7f";
	constexpr char const* escape = "\x1b";
	constexpr char const* pageUp = "\x1b[5~";
	constexpr char const* pageDown = "\x1b[6~";
	constexpr char const* home = "\x1b[H";
	constexpr char const* end = "\x1b[F";
	constexpr char const* insert = "\x1b[2~";
	constexpr char const* delete_ = "\x1b[3~";
	constexpr char const* f1 = "\x1bOP";
	constexpr char const* f2 = "\x1bOQ";
	constexpr char const* f3 = "\x1bOR";
	constexpr char const* f4 = "\x1bOS";
	constexpr char const* f5 = "\x1b[15~";
	constexpr char const* f6 = "\x1b[17~";
	constexpr char const* f7 = "\x1b[18~";
	constexpr char const* f8 = "\x1b[19~";
	constexpr char const* f9 = "\x1b[20~";
	constexpr char const* f10 = "\x1b[21~";
	// No F11 because of fullscreen
	constexpr char const* f12 = "\x1b[24~";
	constexpr char const* tab = "\x09";
	
	namespace Shift
	{
		constexpr char const* return_ = "\x1bOM";
		constexpr char const* delete_ = "\x1b[3;2~";
		constexpr char const* tab = "\x1b[Z";
	}

	namespace Ctrl
	{
		constexpr char const* up = "\33[1;5A";
		constexpr char const* down = "\33[1;5B";
		constexpr char const* right = "\33[1;5C";
		constexpr char const* left = "\33[1;5D";
		constexpr char const* pageUp = "\x1b[5;5~";
		constexpr char const* pageDown = "\x1b[6;5~";
		constexpr char const* home = "\x1b[1;5H";
		constexpr char const* end = "\x1b[1;5F";
		constexpr char const* delete_ = "\x1b[3;5~";
		constexpr char const* backspace = "\x08";
	}

	namespace Alt
	{
		constexpr char const* up = "\33[1;3A";
		constexpr char const* down = "\33[1;3B";
		constexpr char const* right = "\33[1;3C";
		constexpr char const* left = "\33[1;3D";
		constexpr char const* return_ = "\x1b\x0a";
		constexpr char const* backspace = "\x1b\x7f";
		constexpr char const* escape = "\x1b\x1b";
		constexpr char const* pageUp = "\x1b[5;3~";
		constexpr char const* pageDown = "\x1b[6;3~";
		constexpr char const* home = "\x1b[1;3H";
		constexpr char const* end = "\x1b[1;3F";
		constexpr char const* insert = "\x1b[2;3~";
		constexpr char const* delete_ = "\x1b[3;3~";
	}

	namespace CtrlAlt
	{
		constexpr char const* up = "\33[1;7A";
		constexpr char const* down = "\33[1;7B";
		constexpr char const* right = "\33[1;7C";
		constexpr char const* left = "\33[1;7D";
		constexpr char const* pageUp = "\x1b[5;7~";
		constexpr char const* pageDown = "\x1b[6;7~";
		constexpr char const* home = "\x1b[1;7H";
		constexpr char const* end = "\x1b[1;7F";
		constexpr char const* insert = "\x1b[2;7~";
	}

	namespace CtrlShift
	{
		constexpr char const* up = "\33[1;6A";
		constexpr char const* down = "\33[1;6B";
		constexpr char const* right = "\33[1;6C";
		constexpr char const* left = "\33[1;6D";
	}
}