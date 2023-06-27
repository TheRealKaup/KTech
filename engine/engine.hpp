/*

	  || /   ----  ----  ----
	 |||/\   |     |  _  |--
	||||  \  \___  \__/  \___  kaup's console game engine
	
	Docs will be available at https://www.kaup.dev/kcge. Capybaright � 2021-2022 Kaup. All rights reserved.

*/

#pragma once

#include <cstdint>
#include <iostream> // Printing, strings, et cetera
#include <string> // to_string
#include <vector> // Heavily based
#include <functional> // For sharing functions with the engine for it to call on events
#include <thread> // For sleeping, time management.
#include <chrono> // Time management
#include <fstream> // Reading textures and audio files
#include <cstring> // strcmp
#include "config.hpp" // macros
#include <portaudio.h> // PortAudio is a cross-platform audio library
// Linux libraries
#include <signal.h> // For getting signals (like on quit)
#include <termios.h> // For changing terminal attributes
#include <sys/ioctl.h> // Contains some data types (winsize for termios.h)
#include <stdint.h> // int32_t

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

	extern std::vector<std::vector<unsigned char>> colliderTypes; /* Example:
	0 - Can't push | 1 - Can push | 2 - Can overlap
	{
		  A  B, C <- Being collided
	  A { 0, 1, 2 }, <- Moving
	  B { 0, 1, 2 },
	  C { 2, 2, 2 }
	}
	// A Can't be pushed, can push (Heavy)
	// B Can be pushed, can push (Normal)
	// C Can be pushed, can't push (Light) */

	extern winsize terminalSize;

	extern int tps;
	extern long int deltaTime;
	extern TimePoint thisTickStartTP;
	extern float potentialfps;
	extern float fps;
	extern TimePoint engineStartTP;
	extern long int totalTicks;

	extern std::function<void()> GlobalOnTick;
	extern bool running;

	// The final image that will be printed. This allows multiple cameras to print at the same time
	extern std::vector<std::vector<Cell>> image;
	// Instead of allocating a string (that will be printed to the console) every print, it is kept for next prints.
	extern std::string stringImage;

	// Global PortAudio stream
	extern PaStream* stream;

	// Keyboard input things
	namespace Input
	{	
		struct Handler
		{
			std::string input;
			std::vector<std::function<void()>> calls;
			std::vector<bool> onTicks;
			unsigned timesPressed = 0;

			void AddCall(std::function<void()> call, bool onTick);

			Handler(std::string input, std::function<void()> call, bool onTick);
		};

		// The last input as the engine received it, including escape codes.
		extern char* buf;
		// The length (in bytes) of the last input as received by the engine.
		extern unsigned char length;
		// Input handlers.
		extern std::vector<Handler> handlers;
		// You can use and `Engine::Input::handlers` to get some more information about the handler which last called.
		extern unsigned handlerIndex;

		// Register an input handler in order to get your function called on a keyboard input event.
		// Unlike most game engines, this one uses the terminal to recieve keyboard input.
		// This means that the engine receives ASCII and asynchronous input. For example:
		// When the player presses the `'a'` key, the engine will receive and set `Engine::Input::input` to the byte `'a'`.
		// When the player presses the `'a'` key but also the `Shift` modifier key, the engine receives `'A'`.
		// When the player presses a non ASCII key, such as `Arrow Up`, the engine receives what's called an escape sequence/code,
		// which in the case of the `Arrow Up` key, it's `"\033[A"`.
		// Since it is hard to remember all of the escape codes, I have made for you some macros in `config.hpp`, such as `kUp` and `F3`
		// `onTick` - False: calls the moment input is received. True: stores the input and calls once per tick, at the start of the tick.
		void RegisterHandler(std::string stringKey, std::function<void()> call, bool onTick = false);
		void RegisterHandler(char charKey, std::function<void()> call, bool onTick = false);
		// Get inputs (and calls registered input handler accordingly).
		// Returns the input (also updates Engine::Input::buf).
		char* Get();
		// Call this function in order to call all handlers who got their input received since the last time you called this function.
		// This function also resets all `Engine::Input::handlers[].timesPressed`.
		// Returns the amount of calls.
		void Call();
		// A premade loop for automatically getting inputs and calling handlers.
		// You need to create a new thread for this loop (as in `std::thread t_inputLoop(Engine::Input::Loop);`).
		// Calls OnQuit automatically when Ctrl+C is received.
		void Loop();
		
		// Returns true if the last input is equal to stringKey
		bool Is(std::string stringKey);
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
		unsigned char Num();
	}

	struct RGB
	{
		unsigned char r, g, b;
		RGB(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);
	};

	struct RGBA
	{
		unsigned char r, g, b, a;
		RGBA(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0, unsigned char alpha = 255);
	};

	struct Point
	{
		long x, y;
		Point(long x = 0, long y = 0);
	};

	struct UPoint
	{
		unsigned long x, y;
		UPoint(unsigned long x = 0, unsigned long y = 0);
	};

	struct Cell
	{
		char32_t character;
		RGB frgb;
		RGB brgb;
		Cell(char32_t character = ' ', RGB foreground = {0, 0, 0}, RGB background = { 0, 0, 0 });
	};

	struct CellA
	{
		char character;
		RGBA frgba;
		RGBA brgba;
		CellA(char32_t character = ' ', RGBA foreground = {255, 255, 255, 255}, RGBA background = {0, 0, 0, 255});
	};

	struct Texture
	{
		// Position relative to the texture's parent object.
		Point pos = {0, 0};
		// `true` - simple texture mode.
		// `false` - complex texture mode.
		// A simple texture is a rectangle which is faster to render and uses less memory than a complex texture, but is limited to a single cell value.
		// A complex texture is a specific 2D cell shape which is slower to render and uses more memory than a simple texture, but is more capable than a simple texture.
		bool simple = true;
		// `true` - count in the texture and allow to render it.
		// `false` - skip the texture and don't allow to render it.
		bool active = true;

		// (Used if the texture is simple) The size of the texture's rectangle.
		UPoint size = { 0, 0 };
		// (Used if the texture is simple) The cell value of the texture's rectangle.
		CellA value = CellA('#', RGBA());

		// (Used if the texture is complex) The 2D cell shape.
		std::vector<std::vector<CellA>> t = {}; 

		// The constructors are functions instead of actual constructors because of my own preference.
		// Visual Studio has a hard time correctly showing the parameters when filling them, so I decided that
		// this is the best solution for making the writing experience easier.
		
		// A rectangle of the same value. (Limited to a single color and character)
		void Rectangle(UPoint size, CellA value, Point pos);
		// Load from a file. (Requires file)
		void File(std::string fileName, Point pos);
		// Create a texture by writing it. (Limited to a single color)
		void Write(std::vector<std::string> stringVector, RGBA frgba, RGBA brgba, Point pos);
		// Change all the cells' values
		void SetCell(CellA value);
		// Chagne all the cells' foreground color values
		void SetForeground(RGBA value);
		// Chagne all the cells' background color values
		void SetBackground(RGBA value);
		// Chagne all the cells' character values
		void SetCharacter(char value);
	};

	struct Collider
	{
		// Position relative to the collider's parent object.
		Point pos = { 0, 0 };
		// `true` - simple collider mode.
		// `false` - complex collider mode.
		// A simple collider is a rectangle which is faster to process and uses less memory than a complex collider.
		bool simple = true;
		// The collider type which is defined by Engine::colliderTypes
		unsigned char type = 0;
		// Turn the collider on/off.
		bool active = true;

		// Used if the collider is simple.
		// The size of the collider's rectangle.
		UPoint size = { 0, 0 };
		
		// Used if the collider is complex.
		// A 2D boolean vector 
		std::vector<std::vector<bool>> c = {};


		// For a simple collider
		Collider(UPoint size = {0, 0}, Point pos = {0, 0}, int type = 0);
		// For a complex collider
		Collider(std::vector<std::vector<bool>> collider, Point pos = { 0, 0 }, int type = 0);
	};

	struct Object
	{
		Layer* parentLayer = NULL;

		std::function<void()> OnTick = NULL;
		
		std::string name = "Unnamed Object";
		Point pos = { 0, 0 };

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		Point lastPush = { 0, 0 };
		size_t colliderIndex = -1;
		Object* otherObject = NULL;
		size_t otherColliderIndex = -1;

		// This event can be called after calling Move. OnPushed means that another object pushed this one.
		std::function<void()> OnPushed = NULL;
		// This event can be called after calling Move. OnPush means that this objects pushed another object.
		std::function<void()> OnPush = NULL;
		// This event can be called after calling Move. OnBlocked means that another objects blocked this one.
		std::function<void()> OnBlocked = NULL;
		// This event can be called after calling Move. OnBlock means that this object blocked another object.
		std::function<void()> OnBlock = NULL;
		// This event can be called after calling Move if a collider from this object started overlapping a collider from another object.
		std::function<void()> OnOverlap = NULL;
		// This event can be called after calling Move if a collider from this object stopped overlapping a collider from another object.
		std::function<void()> OnOverlapExit = NULL;
		// This event can be called after calling Move if a collider from another object started overlapping a collider from this object.
		std::function<void()> OnOverlapped = NULL;
		// This event can be called after calling Move if a collider from another object stopped overlapping a collider from this object.
		std::function<void()> OnOverlappedExit = NULL;

		// Tries to move the object by processing colliders in the object's parent layer and determining if the object should move, push, or be blocked.
		bool Move(Point dir);

		// Engine used function.
		void ExpandMovementTree(Point dir,
			std::vector<Object*>& pushingObjects,
			std::vector<Object*>& objectsToPush,
			std::vector<size_t>& pushingColliders,
			std::vector<size_t>& collidersToPush,
			std::vector<Object*>& blockedObjects,
			std::vector<Object*>& blockingObjects,
			std::vector<size_t>& blockedColliders,
			std::vector<size_t>& blockingColliders,
			std::vector<Object*>& overlappingObjects,
			std::vector<Object*>& overlappedObjects,
			std::vector<size_t>& overlappingColliders,
			std::vector<size_t>& overlappedColliders,
			std::vector<Object*>& exitOverlappingObjects,
			std::vector<Object*>& exitOverlappedObjects,
			std::vector<size_t>& exitOverlappingColliders,
			std::vector<size_t>& exitOverlappedColliders);

		Object(Point position = {0U, 0U}, std::string objectName = "");

		~Object();
	};

	struct Layer
	{
		bool active = true;

		std::function<void()> OnTick = NULL;

		std::vector<Object*> objects = {};
		
		RGBA frgba = { 0, 0, 0, 0 };
		RGBA brgba = { 0, 0, 0, 0 };
		float opacity = 1.0f;

		int AddObject(Object* object);
		// Remove an object by its index, returns false if couldn't find the object.
		bool RemoveObject(int index);
		// Remove an object by its name, returns false if couldn't find the object.
		bool RemoveObject(std::string name);
		// Remove an object by its pointer, returns false if couldn't find the object.
		bool RemoveObject(Object* object);
	};
	
	struct AudioSource
	{
		// PortAudio's opaque stream pointer
		PaStream* stream;
		// Current sample
		unsigned long cur = 0UL;
		// Amount of samples
		unsigned long frames = 0UL;
		// The end point in samples
		unsigned long endpointToPlay = 0UL;
		// Loops to play
		unsigned long loopsToPlay = 0UL;
		// Start point to play in samples
		unsigned long startPoint = 0UL;
		// Volume
		float volume = 1.0f;
		// Is playing
		bool playing = false;

		// Note that these variables don't do anything if changed, but can be used to access the data about the sound.

		// The size of the .wav file itself (in bytes).
		unsigned long fileSize = 0UL;
		// The amount of channels used to play the sound.
		unsigned channels = 0U;
		// The sample rate (per second).
		unsigned long sampleRate = 0UL;
		// The byte rate (per second).
		unsigned long byteRate = 0UL;
		// I honestly don't know what this is.
		unsigned blockAlign = 0U;
		// The size of a sample (in... bits).
		unsigned bitsPerSample = 0U;
		// The size of the data within the file (in bytes).
		unsigned long dataSize = 0UL;

		// The audio in bytes after it was loaded.
		int16_t* data = nullptr;

		// Default constructer, doesn't do anything.
		AudioSource();
		// Load .wav file.
		// fileName - the name of the file.
		// Returns true if succeeded
		bool LoadWavFile(std::string fileName);

		// Play the sound from the beginning.
		// start - the start point in samples. 0 means the first sample.
		// length - the length in samples. 0 means the entire sound.
		// loops - how many times to loop. 0 and 1 mean play once. 
		// a vector of floats between 0.0f to 1.0f, each element will be the volume of each corresponding channel.
		void Play(unsigned long start = 0, unsigned long length = 0, unsigned short loops = 0, float volume = 1.0f);
		// Pause playing the sound.
		void Pause();
		// Resume playing the sound.
		void Resume();
		// Stop playing the sound.
		void Stop();
	};
	
	struct Camera
	{
		std::function<void()> OnTick = NULL;
		std::string name = "Unnamed Camera";
		Point pos = { 0, 0 };
		UPoint res = { 10U, 10U };
		std::vector<std::vector<Cell>> image = {};

		Camera(Point position = {0, 0}, UPoint resolution = {0, 0}, std::string cameraName = "Unnamed Camera");
		
		void Render(std::vector<Layer*> layers);
		// This function will draw what was rendered (Camera::image's contents) into the "final" image (Engine::image), which will be printed at once with Engine::Print.
		// pos is the position desired to draw the rendered image to the final image.
		// left, top, right and bottom are a rectangle representing the area of the rendered image desired to draw to the final image.
		// Setting right/bottom to 0 will default to the rendered image's size.
		void Draw(Point pos = {0, 0}, unsigned left = 0U, unsigned top = 0U, unsigned right = 0U, unsigned bottom = 0U);
	};
	
	struct Map
	{
	public:
		std::function<void()> OnTick = NULL;
		std::vector<Camera*> cameras = {};
		std::vector<Layer*> layers = {};
		int activeCameraI = -1;

		int AddCamera(Camera* camera, bool asActiveCamera = false);
		int AddLayer(Layer* layer);

		// Returns true if managed to render (active camera is valid), otherwise, false.
		bool Render();
		// Returns true if managed to draw (active camera is valid), otherwise, false.
		bool Draw(Point pos = {0, 0}, unsigned left = 0U, unsigned top = 0U, unsigned right = 0U, unsigned bottom = 0U);
	};

	// An easy to use time point which uses std::chrono.
	// If you want to measure the time passed between 2 points in your code, you use 2 time points at those points and subtract the last one by the first one, which will get you the time passed between those 2 time points.
	struct TimePoint
	{
		// The std::chrono time point
		std::chrono::high_resolution_clock::time_point chornoTimePoint;

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

	// Storage, could be used for many things.
	extern std::vector<Object> storedObjects;
	extern std::vector<Layer> storedLayers;
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
	// Wait the precise amount of time to fill the frame according to the ticks per second value.
	// Also adds 1 to Engine::totalTicks and resets thisTickStartTP
	void WaitUntilNextTick();
	// Give this function a map and it will call all the OnKey functions of the layers, objects and cameras (map itself included) in the map.
	// This way, you don't have to write the very long and ugly for loops to do so.
	void CallOnTicks(Map* map);
};