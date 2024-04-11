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

/*        
	  |  /  ----- ----- ----- -   -  
	 || /\    |   |---  |     |---| 
	|||/  \   |   \____ \____ |   |  kaup's c++ 2d terminal game engine library

	In order to contact me (Kaup), refer to the "readme.md" file.
*/

#pragma once

// Standard libraries
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <fstream>
#include <cstring>
#include <stdint.h>
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
	struct Widget;
	struct UI;
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
		constexpr inline CellA(char character = ' ', RGBA foreground = RGBA(0, 0, 0, 0), RGBA background = RGBA(0, 0, 0, 0)) : c(character), f(foreground), b(background) {}
	};

	// Manager of all things that directly work with terminal I/O.
	struct IO
	{
		Engine* engine;

		termios oldTerminalAttributes;
		
		winsize terminalSize;
		std::vector<std::vector<Cell>> image;
		std::string stringImage;

		std::thread t_inputLoop;

		// Prepares terminal, creates input loop thread
		IO(KTech::UPoint imageSize, Engine* engine);
		// Resets terminal
		~IO();

		void PrintStartupNotice(const std::string& title, const std::string& years, const std::string author, const std::string programName);

		// Draw, usually the image of a camera, to the IO
		void Draw(const std::vector<std::vector<CellA>>& image, Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0, uint8_t alpha = 255);

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
			inline BasicHandler(const std::string& input) : input(input) {};
			void RemoveCallback(BasicCallback*);
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
			inline RangedHandler(char key1, char key2) : key1(key1), key2(key2) {};
			void RemoveCallback(RangedCallback*);
		};

		struct CallbacksGroup
		{
			enum class Status : uint8_t
			{
				disabled,
				enabled,
				removeDisabled, // Remove and then return status to disabled
				removeEnabled, // Remove and then return status to enabled
			};
			std::vector<BasicHandler::BasicCallback*> basicCallbacks;
			std::vector<RangedHandler::RangedCallback*> rangedCallbacks;
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

	// Collision Result
	enum class CR : uint8_t
	{
		B,	// Block
		P,	// Push
		O	// Overlap
	};

	struct Texture
	{
		bool active = true;
		Point pos_r = {0, 0};
		bool simple = true;

		UPoint size = { 0, 0 };
		CellA value = CellA('#', RGBA());

		std::vector<std::vector<CellA>> t = {}; 
		
		void Simple(UPoint size, CellA value, Point relative_position = Point(0, 0));
		void Rectangle(UPoint size, CellA value, Point relative_position = Point(0, 0));// Load from a file.
		UPoint File(const std::string& fileName, Point relative_position = Point(0, 0));
		void Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point relative_position = Point(0, 0));

		void Resize(UPoint newSize, CellA newValue = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)));
		void SetCell(CellA value);
		void SetForeground(RGBA value);
		void SetBackground(RGBA value);
		void SetCharacter(char value);
		void SetForegroundAlpha(uint8_t value);
		void SetBackgroundAlpha(uint8_t value);
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

		void Simple(UPoint size, uint8_t type, Point relative_position = Point(0, 0));
		bool File(const std::string& fileName, uint8_t type, Point relative_position = Point(0, 0));
		void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relative_position = Point(0, 0));
		void ByTextureCharacter(const Texture& texture, uint8_t alphaThreshold, uint8_t type);
		void ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t type);

		UPoint GetSize() const;
	};

	template<class T>
	struct ID
	{
		inline static uint64_t GenerateUUID() { static uint64_t uuid = 0; return ++uuid; }
		size_t i; // Index
		inline ID() : i(0), uuid(GenerateUUID()) {}
		constexpr inline ID(size_t i, size_t uuid) : i(i), uuid(uuid) {}
		inline bool operator==(ID other) const { return other.uuid == uuid; }
		inline uint64_t GetUUID() const { return uuid; }
	private:
		uint64_t uuid; // UUID
	};

	struct Object
	{
		Engine& engine;
		ID<Object> id;

		ID<Layer> parentLayer;

		std::string name = "";
		Point pos = Point(0, 0);

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		bool Move(Point dir);
		
		void EnterLayer(ID<Layer>& layer);

		virtual void OnTick() {} // Called by `Map::CallOnTicks()`.
		virtual void OnPushed(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) pushed this object.
		virtual void OnPush(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object pushed a different object (`otherObject`)
		virtual void OnBlocked(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) blocked this object
		virtual void OnBlock(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object blocked a different object (`otherObject`)
		virtual void OnOverlap(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object entered an overlap with a different object (`otherObject`)
		virtual void OnOverlapExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object exited an overlap with a different object (`otherObject`)
		virtual void OnOverlapped(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) entered an overlap with this object
		virtual void OnOverlappedExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) exited an overlap with this object
		
		Object(Engine& engine, Point position = Point(0, 0), const std::string& name = "");
		Object(Engine& engine, ID<Layer>& parentLayer, Point position = Point(0, 0), const std::string& name = "");
		~Object();
	};

	struct Layer
	{
		Engine& engine;
		ID<Layer> id;
		
		ID<Map> parentMap;

		std::string name = "";
		std::vector<ID<Object>> objects = {};
		
		bool visible = true;
		uint8_t alpha = 255;
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };

		void EnterMap(ID<Map>& map);

		void AddObject(ID<Object>& object);
		bool RemoveObject(const std::string& name);
		bool RemoveObject(ID<Object>& object);
		
		inline virtual void OnTick() {};

		// WARNING: RETURNED REFERENCE IS NOT PERMANENT (VECTOR REALLOCATES)
		ID<Object>& operator[](size_t i) { return objects[i]; }

		Layer(Engine& engine);
		Layer(Engine& engine, ID<Map>& parentMap);
		
		~Layer();
	};

	struct Camera
	{
		Engine& engine;
		ID<Camera> id;

		ID<Map> parentMap;

		std::string name = "";
		Point pos = Point(0, 0);
		UPoint res = UPoint(10, 10);
		CellA background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); // The background to render upon.
		std::vector<std::vector<CellA>> image = {};

		void EnterMap(ID<Map>& map);

		void Render();
		void Render(const std::vector<ID<Layer>>& layers);
		void Resize(UPoint res);

		inline virtual void OnTick() {};

		Camera(Engine& engine, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
		Camera(Engine& engine, ID<Map>& parentMap, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
		~Camera();
	};

	struct Map
	{
	public:
		Engine& engine;
		ID<Map> id;

		std::string name = "";
		std::vector<ID<Camera>> cameras = {};
		std::vector<ID<Layer>> layers = {};
		size_t activeCameraI = -1;

		int AddLayer(ID<Layer>& layer);
		int AddCamera(ID<Camera>& camera, bool asActiveCamera = false);

		bool RemoveLayer(ID<Layer>& layer);
		bool RemoveCamera(ID<Camera>& camera);

		bool Render();

		inline virtual void OnTick() {};
		
		Map(Engine& engine);
		~Map();
	};

	// Widget is now a non-optional KTech standard
	struct Widget
	{
		Engine& engine;
		ID<Widget> id;
		std::string name = "";
		KTech::ID<KTech::UI> parentUI;

		Point pos = Point(0, 0);
		
		std::vector<Texture> textures = {};
		IO::CallbacksGroup* callbacksGroup;
		bool selected = false;
		bool shown = true;

		Widget(Engine& engine, ID<UI> parentUI, Point pos);
		~Widget();

		inline virtual void RenderSelected () {}
		inline virtual void RenderUnselected () {}
		void Select();
		void Deselect();

		void EnterUI(ID<UI> ui);

		inline virtual void OnTick() {};
	};

	// Acts as a camera and a layer for `Widget`s. Image is compatible with `IO::Draw()`.
	struct UI
	{
		Engine& engine;
		ID<UI> id;
		std::string name = "";

		// Layer parts
		std::vector<ID<Widget>> widgets = {};
		bool visible = true;
		uint8_t alpha = 255;
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };

		// Camera parts
		UPoint res = UPoint(10, 10);
		CellA background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); // The background to render upon.
		std::vector<std::vector<CellA>> image = {};

		void AddWidget(ID<Widget> widget);
		bool RemoveWidget(ID<Widget> widget);

		void Render();
		void Resize(UPoint res);
	
		inline virtual void OnTick() {};
		
		UI(Engine& engine, UPoint resolution = UPoint(10, 10), const std::string& name = "");
		~UI();
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
		Engine* engine;

		std::vector<std::vector<CR>> colliderTypes = {
			{ CR::B, CR::P, CR::O }, // Unpushable - 0
			{ CR::B, CR::P, CR::O }, // Pushable - 1
			{ CR::O, CR::O, CR::O } // overlapping - 2
		};

		struct CollisionData{
			ID<Object>& activeObject;
			ID<Object>& passiveObject;
			size_t activeCollider;
			size_t passiveCollider;
		};

		CR GetPotentialCollisionResult(uint8_t t1, uint8_t t2);
	
		bool MoveObject(ID<Object>& object, Point dir);

		inline Collision(Engine* engine) : engine(engine) {};

	private:
		static bool AreSimpleCollidersOverlapping(UPoint simple1, Point pos1, UPoint simple2, Point pos2);
		static bool AreSimpleAndComplexCollidersOverlapping(UPoint simple, Point simplePos, const std::vector<std::vector<bool>>& complex, Point complexPos);
		static bool AreComplexCollidersOverlapping(const std::vector<std::vector<bool>>& c1, Point p1, const std::vector<std::vector<bool>>& c2, Point p2);
	
		void ExpandMovementTree(ID<Object>& thisObj, Point dir,
			std::vector<CollisionData>& pushData,
			std::vector<CollisionData>& blockData,
			std::vector<CollisionData>& overlapData,
			std::vector<CollisionData>& exitOverlapData);
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
	public:
		template<typename T>
		struct Container
		{
			// The structures are stored as generic identifiables, which contain UUIDs.
			T** arr;
			// The size of the array
			size_t size = 0;

			// Returns the valid index of the ID.
			// If the UUID is missing, return the size of the array making the index invalid.
			// Fixes the ID if outdated.
			inline size_t IDToIndex(ID<T>& id);
			// Returns the valid index of the ID.
			// If the UUID is missing, return the size of the array making the index invalid.
			inline size_t IDToIndex(const ID<T>& id);

			// Returns true if the structure is found, false if missing.
			// Fixes the ID if outdated.
			bool Exists(ID<T>& id);
			// Returns true if the structure is found, false if missing.
			bool Exists(const ID<T>& id);

			// Adds the pointer to the container.
			// Automatically called by objects, layers, cameras and maps for themselves.
			// You shouldn't call this manualy on a structure.
			ID<T> Add(T* structure);

			// Remove a structure from storage (doesn't delete it's memory).
			// Returns true if the structure was found and removed.
			// Returns false if the structre is missing.
			bool Remove(const ID<T>& id);

			// Takes an ID reference, returns the pointer to the structure.
			// If the ID is outdated (made so by structures being removed) then update it.
			// If the ID's UUID doesn't exist in the storage at all, this will reset the ID and will return `nullptr`.
			T* operator[](ID<T>& id);
			// Takes an ID reference, returns the pointer to the structure.
			// Constant version of the previous operator, which will not update the given ID if it's outdated/missing.
			T* operator[](const ID<T>& id);
		};

		Container<Object> objects;
		Container<Layer> layers;
		Container<Camera> cameras;
		Container<Map> maps;
		Container<Widget> widgets;
		Container<UI> uis;

		void CallOnTicks();
	};

	// Core class, the user should create one of these, and shouldn't have to construct themselves 
	// a system using the other namespaces themselves
	class Engine
	{
	public:
		bool running = true;

		Collision collision = Collision(this);
		Audio audio;
		Time time;
		IO io;
		Memory memory;

		inline Engine(UPoint imageSize, int16_t tps = 24) : io(IO(imageSize, this)), time(Time(tps, running)) {}
		inline ~Engine() { IO::Log("<Engine::~Engine()>", RGB(255, 0, 0)); }
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
