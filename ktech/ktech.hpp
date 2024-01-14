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
#include <functional> // Callback functions
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
		Point pos = Point(0, 0);

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		bool Move(Point dir);
		
		void EnterLayer(Layer* layer);

		virtual void OnTick() {} // Called by `Map::CallOnTicks()`.
		virtual void OnPushed(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // A different object (`otherObject`) pushed this object.
		virtual void OnPush(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // This object pushed a different object (`otherObject`)
		virtual void OnBlocked(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // A different object (`otherObject`) blocked this object
		virtual void OnBlock(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // This object blocked a different object (`otherObject`)
		virtual void OnOverlap(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // This object entered an overlap with a different object (`otherObject`)
		virtual void OnOverlapExit(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // This object exited an overlap with a different object (`otherObject`)
		virtual void OnOverlapped(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // A different object (`otherObject`) entered an overlap with this object
		virtual void OnOverlappedExit(Point dir, size_t collider, Object* otherObject, size_t otherCollider) {} // A different object (`otherObject`) exited an overlap with this object

		Object(Point position = Point(0, 0), Layer* layer = nullptr, const std::string& name = "");

		~Object();
	};

	struct Layer
	{
		Map* parentMap = nullptr;

		std::vector<Object*> objects = {};
		
		bool visible = true;
		uint8_t alpha = 255;
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };

		int AddObject(Object* object);
		bool RemoveObject(const std::string& name);
		bool RemoveObject(Object* object);
		
		inline virtual void OnTick() {};

		Layer(Map* map);

		~Layer();
	};

	struct Camera
	{
		Map* parentMap = nullptr;

		std::string name = "";
		Point pos = Point(0, 0);
		UPoint res = UPoint(10, 10);
		std::vector<std::vector<Cell>> image = {};

		void Render(const std::vector<Layer*>& layers);
		void Resize(UPoint res);

		inline virtual void OnTick() {};
		
		Camera(Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	};

	struct Map
	{
	public:
		Engine* parentEngine;

		std::vector<Camera*> cameras = {};
		std::vector<Layer*> layers = {};
		size_t activeCameraI = -1;

		int AddCamera(Camera* camera, bool asActiveCamera = false);
		int AddLayer(Layer* layer);

		bool Render();
		void CallOnTicks();

		inline virtual void OnTick() {};
		
		Map(Engine* parentEngine) : parentEngine(parentEngine) {};
	};

	// Manager of all things that directly work with terminal I/O.
	class IO
	{
	public:
		Engine* engine;

		// Prepares terminal, creates input loop thread
		IO(KTech::UPoint imageSize, Engine* engine);
		// Resets terminal
		~IO();

		termios oldTerminalAttributes;
		
		winsize terminalSize;
		std::vector<std::vector<Cell>> image;
		std::string stringImage;

		std::thread t_inputLoop;

		void PrintStartupNotice(const std::string& title, const std::string& years, const std::string author, const std::string programName);

		// Draw, usually the image of a camera, to the IO
		void Draw(const std::vector<std::vector<Cell>>& image, Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0);

		void Print();

		static void Log(const std::string& text, RGB color);

		static char* Get();

		struct Callback
		{
			bool enabled = true;
			std::function<void()> ptr;
			inline Callback(const std::function<void()>& callback) : ptr(callback) {}
		};

		struct BasicHandler
		{
			struct BasicCallback : Callback
			{
				BasicHandler* parentHandler;
				bool onTick;
				inline BasicCallback(const std::function<void()>& callback, BasicHandler* parentHandler, bool onTick) : Callback(callback), parentHandler(parentHandler), onTick(onTick) {}
				~BasicCallback();
			};
			std::vector<BasicCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
			std::string input;
			uint8_t timesPressed = 0;
			void RemoveCallback(BasicCallback*);
			inline BasicHandler(const std::string& input) : input(input) {};
		};

		struct RangedHandler
		{
			struct RangedCallback : Callback
			{
				RangedHandler* parentHandler;
				inline RangedCallback(const std::function<void()>& callback, RangedHandler* parentHandler) : Callback(callback), parentHandler(parentHandler) {}
				~RangedCallback();
			};
			std::vector<RangedCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
			char key1, key2;
			void RemoveCallback(RangedCallback*);
			inline RangedHandler(char key1, char key2) : key1(key1), key2(key2) {};
		};

		struct CallbacksGroup
		{
			std::vector<BasicHandler::BasicCallback*> basicCallbacks;
			std::vector<RangedHandler::RangedCallback*> rangedCallbacks;
			enum class Status : uint8_t
			{
				disabled,
				enabled,
				removeDisabled, // Remove and then return status to disabled
				removeEnabled, // Remove and then return status to enabled
			};
			Status status;
			bool synced = true;
			inline CallbacksGroup(bool enabled = true) : status(enabled ? Status::enabled : Status::disabled) { }
			inline void AddCallback(BasicHandler::BasicCallback* callback) { basicCallbacks.push_back(callback); callback->enabled = false; }
			inline void AddCallback(RangedHandler::RangedCallback* callback) { rangedCallbacks.push_back(callback); callback->enabled = false; }
			inline void Enable() { status = Status::enabled; synced = false; }
			inline void Disable() { status = Status::disabled; synced = false; }
			// Remove
			void DeleteCallbacks();
		};

		std::vector<BasicHandler*> basicHandlers; // Never deleted, no need to store in pointers; handleded by indices
		std::vector<RangedHandler*> rangedHandlers; // Never deleted, no need to store in pointers; handleded by indices
		std::vector<CallbacksGroup*> groups; // Usually experiences deletions; handleded by pointers

		BasicHandler::BasicCallback* RegisterCallback(const std::string& stringKey, const std::function<void()>& callback, bool onTick = false);
		RangedHandler::RangedCallback* RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback);
		CallbacksGroup* CreateCallbacksGroup(bool enabled = true);

		std::string input = std::string(7, '\0');
		std::string quitKey = "\x03";

		void Call();

		bool Is(const std::string& stringKey);
		bool Is(char charKey);
		bool Bigger(char charKey);
		bool Smaller(char charKey);
		bool Between(char charKey1, char charKey2);
		uint8_t GetInt();

		void Loop();
	};

	// Time manager
	class Time
	{
	public:
		const bool& running;
		
		enum class Measurement : uint8_t
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
			TimePoint();
		};

		// Get the amount of time that has passed since `TimePoint a` until `TimePoint b`.
		long GetDelta(const TimePoint& a, const TimePoint& b, Measurement timeMeasurement);
		// Get the amount of time that has passed since the creation of `Time` and `TimePoint tp`.
		long GetInt(const TimePoint& tp, Measurement timeMeasurement);

		const TimePoint startTP;
		TimePoint thisTickStartTP;

		int16_t tpsLimit = 24;
		float tps = 0.0f;
		float tpsPotential = 0.0f;
		int32_t deltaTime = 0;
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

		inline Time(int16_t tps, const bool& running) : tps(tps), running(running) {}
	};

	// Exists to store `colliderTypes`.
	class Collision
	{
	public:
		// Collision Result
		enum class CR : uint8_t
		{
			B,	// Block
			P,	// Push
			O	// Overlap
		};

		std::vector<std::vector<CR>> colliderTypes = {
			{ CR::B, CR::P, CR::O }, // Heavy - 0
			{ CR::B, CR::P, CR::O }, // Normal - 1
			{ CR::O, CR::O, CR::O } // Overlappable - 2
		};

		struct CollisionData{
			KTech::Object* activeObject;
			KTech::Object* passiveObject;
			size_t activeCollider;
			size_t passiveCollider;
		};

		CR GetPotentialCollisionResult(uint8_t t1, uint8_t t2);

		static bool AreSimpleCollidersOverlapping(UPoint simple1, Point pos1, UPoint simple2, Point pos2);
		static bool AreSimpleAndComplexCollidersOverlapping(UPoint simple, Point simplePos, const std::vector<std::vector<bool>>& complex, Point complexPos);
		static bool AreComplexCollidersOverlapping(const std::vector<std::vector<bool>>& c1, Point p1, const std::vector<std::vector<bool>>& c2, Point p2);
	
		void ExpandMovementTree(Object* thisObj, Point dir,
			std::vector<CollisionData>& pushData,
			std::vector<CollisionData>& blockData,
			std::vector<CollisionData>& overlapData,
			std::vector<CollisionData>& exitOverlapData);
	
		bool MoveObject(Object* object, Point dir);
	};

	// Audio manager, creates its own audio stream using the PortAudio library, after making sure it is initialized.
	class Audio
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

	class Memory
	{
	private:
		struct ID
		{
			size_t i; // Index
			uint64_t u; // UUID
			constexpr inline ID(size_t i, size_t u) : i(i), u(u) {};
		};

		template<typename T>
		struct Storage
		{
			// Contains a pointer to the structure instnace and the UUID of the instnace
			struct Cell
			{
				T* p; // Pointer
				size_t u; // UUID
			};

			// The structures are stored within cells, with their UUIDs
			Cell* arr;
			// The size of the array
			size_t size = 0;

			// An RNG for multiplayer will be integrated later, but offline this works fine.
			inline static uint64_t GenerateUUID()
			{
				static uint64_t uuid = 0;
				return ++uuid;
			}

			inline ID Add(T* structure)
			{
				Cell* tmp = new Cell[size + 1]; // New array with one more cell
				size_t i = 0; // Outside of for loop's scope for later use
				for (; i < size; i++)
					tmp[i] = arr[i]; // Move the cells to the new array
				tmp[i] = {structure, GenerateUUID()}; // Create a new cell for the added structure
				delete[] arr; // Delete the old array
				arr = tmp; // Set the array as the new array
				size++; // Increase the size by one
				return ID(i, arr[i].u); // Return new ID of structure
 			}

			// Returns the valid index of the ID.
			// If the UUID is missing, return the size of the array making the index invalid.
			inline size_t IDToIndex(const ID& id)
			{
				for (size_t i = (id.i < size ? id.i : size - 1);; i--)
				{
					if (arr[i].u == id.u)
						return i;
					if (i == 0)
						return size;
				}
			}

			// Remove a structure from storage.
			// Returns true if the structure was found and removed.
			// Returns false if the structre is missing.
			inline bool Remove(const ID& id)
			{
				Cell* tmp = new Cell[size - 1]; // New array with one more cell
				size_t toRemove = IDToIndex(id); // Convert the ID to a valid index
				if (toRemove == size) // If the index is the size, then it's invalid
					return false;
				size_t i = 0;
				for (; i < toRemove; i++, i++)
					tmp[i] = arr[i]; // Move the cells to the new array
				for (; i < size; i++)
					tmp[i - 1] = arr[i]; // Move the cells to the new array
				delete[] arr; // Delete the old array
				arr = tmp; // Set the array as the new array
				size--; // Increase the size by one
				return true;
			}

			// Takes an ID reference, returns the pointer to the structure.
			// If the ID is outdated (made so by structures being removed) then update it.
			// If the ID's UUID doesn't exist in the storage at all, this will reset the ID and will return `nullptr`.
			T* operator[](ID& id) const
			{
				for (size_t i = (id.i < size ? id.i : size -1);; i--)
				{
					if (arr[i].u == id.u)
					{
						id.i = i;
						return arr[i].p;
					}
					if (i == 0)
						break;
				}
				id = ID(0, 0);
				return nullptr;
			}

			// Takes an ID reference, returns the pointer to the structure.
			// Constant version of the previous operator, which will not update the given ID if it's outdated/missing.
			T* operator[](const ID& id) const
			{
				for (size_t i = (id.i < size ? id.i : size -1);; i--)
				{
					if (arr[i].u == id.u)
						return arr[i].p;
					if (i == 0)
						break;
				}
				return nullptr;
			}
		};

		Storage<Object> objects;
		Storage<Layer> layers;
		Storage<Camera> cameras;
		Storage<Map> maps;
	};

	// Core class, the user should create one of these, and shouldn't have to construct themselves 
	// a system using the other namespaces themselves
	class Engine
	{
	public:
		bool running = true;

		IO io;
		Time time;
		Audio audio;
		Collision collision;

		inline Engine(UPoint imageSize, int16_t tps = 24) : io(IO(imageSize, this)), time(Time(tps, running)) {}
	};

	namespace RGBColors
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

	namespace RGBAColors
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
	
	namespace Keys
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
}
