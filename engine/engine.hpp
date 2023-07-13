/*

	  |  /  ----- ----- ----- -   -  
	 || /\    |   |---  |     |---| 
	|||/  \   |   \____ \____ |   |  kaup's terminal game engine

	Tutorials and references are available at https://github.com/TheRealKaup/KTech in `tutorials.md` and `references.md` respectively. Capybaright � 2021-2023 Kaup. All rights reserved.

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

namespace Engine
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
	struct AudioSource;
	struct TimePoint;

	inline bool running = true;

	inline std::vector<std::vector<uint8_t>> colliderTypes = {
		{ 0, 1, 2 }, // Heavy - 0
		{ 0, 1, 2 }, // Normal - 1
		{ 2, 2, 2 } // Overlappable - 2
	};

	extern winsize terminalSize;
	// The final image that will be printed. This allows multiple cameras to print at the same time
	extern std::vector<std::vector<Cell>> image;
	// Instead of allocating a string (that will be printed to the console) every print, it is kept for next prints.
	extern std::string stringImage;

	// Keyboard input; input handlers, input loop...
	namespace Input
	{	
		struct HandlerCallback
		{
			std::function<void()> callback;
			bool onTick;
		};

		struct Handler
		{
			std::string input;
			std::vector<HandlerCallback> callbacks;
			uint8_t timesPressed = 0;

			Handler(const std::string& input, std::function<void()> call, bool onTick);
		};

		// The last input as the engine received it, including escape codes.
		extern std::string input;
		// Input handlers.
		extern std::vector<Handler> handlers;
		// You can use and `Engine::Input::handlers` to get some more information about the handler which last called.
		extern size_t handlerIndex;

		// Register an input handler in order to get your function called on a keyboard input event.
		// Unlike most game engines, this one uses the terminal to recieve keyboard input.
		// This means that the engine receives ASCII and asynchronous input. For example:
		// When the player presses the `'a'` key, the engine will receive and set `Engine::Input::input` to the byte `'a'`.
		// When the player presses the `'a'` key but also the `Shift` modifier key, the engine receives `'A'`.
		// When the player presses a non ASCII key, such as `Arrow Up`, the engine receives what's called an escape sequence/code,
		// which in the case of the `Arrow Up` key, it's `"\033[A"`.
		// Since it is hard to remember all of the escape codes, I have made for you some macros in `config.hpp`, such as `kUp` and `F3`
		// `onTick` - False: calls the moment input is received. True: stores the input and calls once per tick, at the start of the tick.
		void RegisterHandler(const std::string& stringKey, std::function<void()> callback, bool onTick = false);
		// Get inputs (and calls registered input handler accordingly).
		// Returns the input (also updates Engine::Input::buf).
		std::string& Get();
		// Call this function in order to call all handlers who got their input received since the last time you called this function.
		// This function also resets all `Engine::Input::handlers[].timesPressed`.
		// Returns the amount of calls.
		uint32_t Call();
		// A premade loop for automatically getting inputs and calling handlers.
		// You need to create a new thread for this loop (as in `std::thread t_inputLoop(Engine::Input::Loop);`).
		// Calls OnQuit automatically when Ctrl+C is received.
		void Loop();
		
		// Returns true if the last input is equal to stringKey
		bool Is(const std::string& stringKey);
		// Returns true if the last input is equal to charKey
		bool Is(char charKey);
		// Returns true if the last input is BIGGER OR EQUAL than charKey
		bool Bigger(char charKey);
		// Return true if the last input is SMALLER OR EQUAL than charKEy
		bool Smaller(char charKey);
		// Returns true if the last input is between or equal to the specified keys.
		bool Between(char charKey1, char charKey2);
		// Returns true if the last input is a number
		bool IsNum();
		// Returns the last value as a number
		uint8_t GetNum();
	}

	// Time management; invocations, tps, fps...
	namespace Time
	{
		// Time point saving tool, std::chrono wrapper
		struct TimePoint
		{
			// The std::chrono time point
			std::chrono::high_resolution_clock::time_point chronoTimePoint;

			// The default constructor will set the time point to the time it was created
			TimePoint();

			// Will reset the time point to the current time
			void SetToNow();

			// Get an int which represents the time point in seconds
			long Seconds();
			// Get an int which represents the time point in milliseconds
			long Milliseconds();
			// Get an int which represents the time point in microseconds
			long Microseconds();
			// Get an int which represents the time point in nanoseconds
			long Nanoseconds();
		};

		struct Invocation
		{
			std::function<void()> callback;
			uint32_t ticks; // Ticks, not a value that should change
			uint32_t ticksLeft; // Current ticks left in this instance
			uint32_t instancesLeft; // Instances left
			inline Invocation(std::function<void()> callback, uint32_t ticks, uint32_t instances)
				: callback(callback), ticks(ticks), ticksLeft(ticks), instancesLeft(instances) {}
		};

		inline int16_t tps = 24;
		extern int32_t deltaTime;
		extern TimePoint thisTickStartTP;
		extern float potentialfps;
		extern float fps;
		extern TimePoint engineStartTP;
		extern int32_t totalTicks;
		inline std::vector<Invocation> invocations = {};

		enum class TimeMeasurement
		{
			ticks,
			seconds,
			milliseconds
		};
		// Add a callback function to the `Engine::Time::invocations` vector, which will be called in a given time by `CallInvocations`.
		// `std::function<void()> callback` - your callback function.
		// `uint32_t` time - the given time.
		// `TimeMeasurement timeMeasurement` - the time measurement for the given time, can be in ticks, seconds, milliseconds or microseconds (last three are converted into ticks).
		// (optional) `uint32_t instances = 1` - how many times sequentially to invoke this callback function.
		void Invoke(std::function<void()> callback, uint32_t time, TimeMeasurement timeMeasurement, uint32_t instances = 1);

		void CallInvocations();

		inline void StartThisTick() { Time::thisTickStartTP.SetToNow(); }
		// Suspend the thread the amount of time should be left for the tick according to `tps`.
		// Also adds 1 to `totalTicks` and resets `thisTickStartTP`.
		void WaitUntilNextTick();
	}

	struct RGB
	{
		uint8_t r, g, b;
		inline RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) : r(red), g(green), b(blue) {}
	};

	struct RGBA
	{
		uint8_t r, g, b, a;
		inline RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
	};

	struct Point
	{
		int32_t x, y;
		inline Point(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}
	};

	struct UPoint
	{
		uint32_t x, y;
		inline UPoint(uint32_t x = 0, uint32_t y = 0) : x(x), y(y) {}
	};

	struct Cell
	{
		RGB f;
		RGB b;
		char c;
		inline Cell(char character = ' ', RGB foreground = {0, 0, 0}, RGB background = { 0, 0, 0 }) : c(character), f{foreground}, b(background) {}
	};

	struct CellA
	{
		RGBA f;
		RGBA b;
		char c;
		inline CellA(char character = ' ', RGBA foreground = {255, 255, 255, 255}, RGBA background = {0, 0, 0, 0}) : c(character), f{foreground}, b(background) {}
	};

	struct Texture
	{
		// `True` - Allow `Camera::Render()` to render this texture.
		// `False` - Disallow `Camera::Render()` to render this texture.
		bool active = true;
		// Position relative to the texture's parent object.
		Point pos = {0, 0};
		// `True` - Simple texture mode.
		// `False` - Complex texture mode.
		// A simple texture is a rectangle which is faster to render and uses less memory than a complex texture, but is limited to a single `CellA` value.
		// A complex texture is a 2D cell-made shape which is slower to render and uses more memory than a simple texture, but is more capable than a simple texture.
		bool simple = true;

		// (Used if the texture is simple) The size of the texture's rectangle.
		UPoint size = { 0, 0 };
		// (Used if the texture is simple) The cell value of the texture's rectangle.
		CellA value = CellA('#', RGBA());

		// (Used if the texture is complex) The 2D cell shape.
		std::vector<std::vector<CellA>> t = {}; 

		// The constructors are functions instead of actual constructors because of my own preference.
		// Visual Studio has a hard time correctly showing the parameters when filling them, so I decided that
		// this is the best solution for making the writing experience easier.

		// [Simple] Create a rectangle of the same value (limited to a single CellA value).
		void Simple(UPoint size, CellA value, Point position);
		// [Complex] Create a rectangle.
		void Rectangle(UPoint size, CellA value, Point position);// Load from a file.
		// [Complex] Load from a file.
		Engine::UPoint File(const std::string& fileName, Point position);
		// [Complex] Create a texture by writing it (limited to single foreground and background RGBA values)
		void Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point position);
		// [Simple & Complex] Resize the texture, keeps previous values in the given size.
		// `UPoint newSize` - The new rectangle size of the texture.
		// `CellA newValue` - The value which only new cells will be set to. 
		void Resize(UPoint newSize, CellA newValue);
		// [Simple & Complex] Change all the cells' values
		void SetCell(CellA value);
		// [Simple & Complex] Change all the cells' foreground color values
		void SetForeground(RGBA value);
		// [Simple & Complex] Change all the cells' background color values
		void SetBackground(RGBA value);
		// [Simple & Complex] Change all the cells' character values
		void SetCharacter(char value);
		// [Complex] Export the texture to a file (existent or non existent), uses standard ktech texture file format.
		void ExportToFile(const std::string& fileName);
	};

	struct Collider
	{
		// `True` - Allow `Object::Move()` to process this collider.
		// `False` - Disallow `Object::Move()` to process this collider.
		bool active = true;
		// Position relative to the collider's parent object.
		Point pos = { 0, 0 };
		// `True` - simple collider mode.
		// `False` - complex collider mode.
		// A simple collider is a rectangle which is faster to process and uses less memory than a complex collider.
		// A complex collider is a 2D cell-made shape which is slower to process and uses more mamory than a simple collider, but is more capable than a simple collider.
		bool simple = true;
		// The collider type which is defined by Engine::colliderTypes
		uint8_t type = 0;

		// Used if the collider is simple.
		// The size of the collider's rectangle.
		UPoint size = { 0, 0 };
		
		// Used if the collider is complex.
		// A 2D boolean vector 
		std::vector<std::vector<bool>> c = {};

		// A rectangle.
		void Simple(UPoint size, uint8_t type, Point position);
		// Load from a file.
		bool File(const std::string& fileName, uint8_t type, Point position);
		// Create a collider by writing it.
		void Write(const std::vector<std::string>& stringVector, uint8_t type, Point position);
		// Create a collider which corrosponds to a texture (also takes its position)
		void ByTexture(const Texture& texture, uint8_t type);
	};

	struct Object
	{
		Layer* parentLayer = NULL;

		std::function<void()> OnTick = nullptr;
		
		std::string name = "Unnamed Object";
		Point pos = { 0, 0 };

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		Point lastPush = { 0, 0 };
		size_t colliderIndex = -1;
		Object* otherObject = NULL;
		size_t otherColliderIndex = -1;

		// This event can be called after calling Move. OnPushed means that another object pushed this one.
		std::function<void()> OnPushed = nullptr;
		// This event can be called after calling Move. OnPush means that this objects pushed another object.
		std::function<void()> OnPush = nullptr;
		// This event can be called after calling Move. OnBlocked means that another objects blocked this one.
		std::function<void()> OnBlocked = nullptr;
		// This event can be called after calling Move. OnBlock means that this object blocked another object.
		std::function<void()> OnBlock = nullptr;
		// This event can be called after calling Move if a collider from this object started overlapping a collider from another object.
		std::function<void()> OnOverlap = nullptr;
		// This event can be called after calling Move if a collider from this object stopped overlapping a collider from another object.
		std::function<void()> OnOverlapExit = nullptr;
		// This event can be called after calling Move if a collider from another object started overlapping a collider from this object.
		std::function<void()> OnOverlapped = nullptr;
		// This event can be called after calling Move if a collider from another object stopped overlapping a collider from this object.
		std::function<void()> OnOverlappedExit = nullptr;

		// Tries to move the object by processing colliders in the object's parent layer and determining if the object should move, push, or be blocked.
		bool Move(Point dir);

		Object(Point position = Point(0, 0), const std::string& objectName = "");

		~Object();
	};

	struct Layer
	{
		bool active = true;

		std::function<void()> OnTick = nullptr;

		std::vector<Object*> objects = {};
		
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };
		uint8_t alpha = 255;

		int AddObject(Object* object);
		// Remove an object by its index, returns false if couldn't find the object.
		bool RemoveObject(size_t index);
		// Remove an object by its name, returns false if couldn't find the object.
		bool RemoveObject(const std::string& name);
		// Remove an object by its pointer, returns false if couldn't find the object.
		bool RemoveObject(Object* object);
	};
	
	struct AudioSource
	{
		// PortAudio's opaque stream pointer
		PaStream* stream;
		// Current sample
		uint32_t cur = 0UL;
		// Amount of samples
		uint32_t frames = 0UL;
		// The end point in samples
		uint32_t endpointToPlay = 0UL;
		// Loops to play
		uint32_t loopsToPlay = 0UL;
		// Start point to play in samples
		uint32_t startPoint = 0UL;
		// Volume
		float volume = 1.0f;
		// Is playing
		bool playing = false;

		// Note that these variables don't do anything if changed, but can be used to access the data about the sound.

		// The size of the .wav file itself (in bytes).
		uint32_t fileSize = 0UL;
		// The amount of channels used to play the sound.
		uint8_t channels = 0U;
		// The sample rate (per second).
		uint32_t sampleRate = 0UL;
		// The byte rate (per second).
		uint32_t byteRate = 0UL;
		// I honestly don't know what this is.
		uint8_t blockAlign = 0U;
		// The size of a sample (in... bits).
		uint16_t bitsPerSample = 0U;
		// The size of the data within the file (in bytes).
		uint32_t dataSize = 0UL;

		// The audio in bytes after it was loaded.
		int16_t* data = nullptr;

		// Default constructer, doesn't do anything.
		AudioSource();
		// Load .wav file.
		// `fileName` - the name of the file.
		// Returns true if succeeded
		bool LoadWavFile(const std::string& fileName);

		// Play the sound from the beginning.
		// start - the start point in samples. 0 means the first sample.
		// length - the length in samples. 0 means the entire sound.
		// loops - how many times to loop. 0 and 1 mean play once. 
		// a vector of floats between 0.0f to 1.0f, each element will be the volume of each corresponding channel.
		void Play(uint32_t start = 0, uint32_t length = 0, uint16_t loops = 0, float volume = 1.0f);
		// Pause playing the sound.
		void Pause();
		// Resume playing the sound.
		void Resume();
		// Stop playing the sound.
		void Stop();
	};
	
	struct Camera
	{
		std::function<void()> OnTick = nullptr;
		std::string name = "";
		Point pos = Point(0, 0);
		UPoint res = UPoint(10, 10);
		std::vector<std::vector<Cell>> image = {};

		Camera(Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
		
		void Render(std::vector<Layer*> layers);
		// This function will draw what was rendered (Camera::image's contents) into the "final" image (Engine::image), which will be printed at once with Engine::Print.
		// pos is the position desired to draw the rendered image to the final image.
		// left, top, right and bottom are a rectangle representing the area of the rendered image desired to draw to the final image.
		// Setting right/bottom to 0 will default to the rendered image's size.
		void Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0);
	};
	
	struct Map
	{
	public:
		std::function<void()> OnTick = NULL;
		std::vector<Camera*> cameras = {};
		std::vector<Layer*> layers = {};
		size_t activeCameraI = -1;

		int AddCamera(Camera* camera, bool asActiveCamera = false);
		int AddLayer(Layer* layer);

		// Returns true if managed to render (active camera is valid), otherwise, false.
		bool Render();
		// Returns true if managed to draw (active camera is valid), otherwise, false.
		bool Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0);
		// Calls all of the OnTick callback functions of the `Camera`s, `Layer`s and `Object`s within this map, and this `Map`'s OnTick as well.
		void CallOnTicks();
	};

	// Storage, could be used for many things.
	inline std::vector<Object> storedObjects = {};
	inline std::vector<Layer> storedLayers = {};
	// If you don't want to store an object yourself, add it to this function which will store it for you. Add the returned pointer to the desired layer.
	// This is used by the engine when you load a map.
	Object* StoreObject(Object object);
	// If you don't want to store a layer yourself, add it to this function which will store it for you. Add the returned pointer to the desired map.
	// This is used by the engine when you load a map.
	Layer* StoreLayer(Layer layer);

	// Prepare the audio library (AudioPort) for AudioSources.
	void InitializeAudio();
	// Terminate (deallocate) the audio library (PortAudio).
	void TerminateAudio();
	// Prepare the terminal for printing and receiving input
	void PrepareTerminal(UPoint imageSize);
	// Reset the terminal before exiting the game (turn in back to normal).
	void ResetTerminal();
	// Print the final image to the console.
	void Print();

	// Prints text to the terminal, text will only appear if you are not printing.
	void Log(const std::string& text, RGB color);
};