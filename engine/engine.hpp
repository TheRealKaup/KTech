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

	// Reference to the size of the terminal, changing this won't change the terminal's size.
	inline winsize terminalSize;
	// The final image that will be printed. This allows multiple cameras to print at the same time
	extern std::vector<std::vector<Cell>> image;
	// Instead of allocating a string (that will be printed to the console) every print, it is kept for next prints.
	inline std::string stringImage;

	// Collision Result
	enum class CR : uint8_t
	{
		B,	// Block
		P,	// Push
		O	// Overlap
	};
	inline std::vector<std::vector<CR>> colliderTypes = {
		{ CR::B, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::O }, // Normal - 1
		{ CR::O, CR::O, CR::O } // Overlappable - 2
	};

	// Keyboard input; input handlers, input loop...
	namespace Input
	{
		// Input key values (declared at the end of this file)
		namespace K {};

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
			// What the callbacks' enabled status should be.
			bool enabled;
			// Is the enabled status already synced with the callbacks?
			bool synced = true;
			// Automatically adds itself to the static groups vector
			inline CallbacksGroup(bool enabled = true) : enabled(enabled) { groups.push_back(this); }
			// Automatically removes itself from the static groups vector
			inline ~CallbacksGroup() { for (size_t i = 0; i < groups.size(); i++) if (groups[i] == this) groups.erase(groups.begin() + i); }
			inline void AddCallback(BasicHandler::BasicCallback* callback) { basicCallbacks.push_back(callback); callback->enabled = enabled; }
			inline void AddCallback(RangedHandler::RangedCallback* callback) { rangedCallbacks.push_back(callback); callback->enabled = enabled; }
			// Enable the callbacks in the group at the start of the next tick.
			inline void Enable() { enabled = true; synced = false; }
			// Disable the callbacks in the group at the start of the next tick.
			inline void Disable() { enabled = false; synced = false; }
			// Permanently unregisters and deletes the callbacks in the group.
			void DeleteCallbacks();
		};

		// The last input as the engine received it, including escape codes.
		extern std::string input;

		// Register an input handler in order to get your function called on a keyboard input event.
		// Unlike most game engines, this one uses the terminal to recieve keyboard input.
		// This means that the engine receives ASCII and asynchronous input. For example:
		// When the player presses the `'a'` key, the engine will receive and set `Engine::Input::input` to the byte `'a'`.
		// When the player presses the `'a'` key but also the `Shift` modifier key, the engine receives `'A'`.
		// When the player presses a non ASCII key, such as `Arrow Up`, the engine receives what's called an escape sequence/code,
		// which in the case of the `Arrow Up` key, it's `"\033[A"`.
		// Since it is hard to remember all of the escape codes, I have made for you some macros in `config.hpp`, such as `kUp` and `F3`
		// `onTick` - False: calls the moment input is received. True: stores the input and calls once per tick, at the start of the tick.
		BasicHandler::BasicCallback* RegisterCallback(const std::string& stringKey, const std::function<void()>& callback, bool onTick = false);
		// Register an input handler that would be called if a key between the given keys are detected.
		// This is majorly better in initialzation speed and memory size than `RegisterCallback()` when it comes to registering a bunch of keys that are next to each other in the ASCII table.
		// For example, field widgets use this.
		// The `onTick` feature is not possible with the type of the callback's handler.
		RangedHandler::RangedCallback* RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback);
		// Get inputs (and calls registered input handler accordingly).
		// Returns the input (also updates Engine::Input::buf).
		std::string& Get();
		// Call this function in order to call all handlers who got their input received since the last time you called this function.
		// This function also resets all `Engine::Input::handlers[].timesPressed`.
		// Returns the amount of calls.
		void Call();
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
		// Returns the last input as an integer (assumes that the input is a digit character digit)
		uint8_t GetInt();
	}

	// Time management; invocations, tps...
	namespace Time
	{
		enum class Measurement
		{
			ticks,
			seconds,
			milliseconds,
			microseconds
		};

		// Time point saving tool, std::chrono wrapper
		struct TimePoint
		{
			// The std::chrono time point
			std::chrono::high_resolution_clock::time_point chronoTimePoint;
			// Will reset the time point to the current time
			void SetToNow();
			// Get an int which represents the time point in ticks, seconds, milliseconds or microseconds
			long GetInt(Measurement timeMeasurement) const;
			// The default constructor will set the time point to the time it was created
			TimePoint();
		};

		// The start of the program.
		inline const TimePoint startTP;
		// The start of this current tick (updated by `StartThisTick()`).
		inline TimePoint thisTickStartTP;

		// Miximum (limit) for the ticks per second. Set this at any point. 
		inline int16_t tpsLimit = 24;
		// Actual current ticks per second value.
		extern float tps;
		// The ticks per second value if the game loop wasn't limited by `tpsLimit`.
		extern float tpsPotential;
		// The amount of time that passed between the last tick and the tick before it, in microseconds (updated by `WaitUntilNextTick()`).
		extern int32_t deltaTime;
		// The counter of the total ticks since the game started. Counted by `WaitUntilNextTick()`, each call it adds 1.
		inline int32_t ticksCounter = 0;
		
		// Call this at the start of each tick so `WaitUntilNextTick()` can appropriately suspend the thread between the ticks in order to limit the TPS.
		inline void StartThisTick() { Time::thisTickStartTP.SetToNow(); }
		// Suspend the thread the amount of time should be left for the tick according to `tps`.
		// Also adds 1 to `totalTicks` and resets `thisTickStartTP`.
		void WaitUntilNextTick();
		
		struct Invocation
		{
			inline static std::vector<Invocation*> invocations;
			std::function<void()> callback;
			uint32_t ticksLeft; // Current ticks left in this instance
			inline Invocation(const std::function<void()>& callback, uint32_t ticks)
				: callback(callback), ticksLeft(ticks) {}
		};
		// Add a callback function to the `Engine::Time::invocations` vector, which will be called in a given time by `CallInvocations`.
		// `std::function<void()> callback` - your callback function.
		// `uint32_t` time - the given time.
		// `TimeMeasurement timeMeasurement` - the time measurement for the given time, can be in ticks, seconds, milliseconds or microseconds
		// (optional) `uint32_t instances = 1` - how many times sequentially to invoke this callback function.
		Invocation* Invoke(const std::function<void()>& callback, uint32_t time, Measurement timeMeasurement);
		// Cancel an invocation so the callback function will not be called (also deletes the invocation's memory).
		// returns true if found the invocation and removed it.
		bool CancelInvocation(Invocation* invocation);
		// Make the invocations system proceed to the next tick (place inside game loop).
		void CallInvocations();
	}

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
		// `True` - Allow `Camera::Render()` to render this texture.
		// `False` - Disallow `Camera::Render()` to render this texture.
		bool active = true;
		// Position relative to the texture's parent object.
		Point pos_r = {0, 0};
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
		
		// * CONSTRUCTOR * [Simple] Construct a rectangle of the same value (limited to a single CellA value).
		void Simple(UPoint size, CellA value, Point relative_position);
		// * CONSTRUCTOR * [Complex] Construct a rectangle.
		void Rectangle(UPoint size, CellA value, Point relative_position);// Load from a file.
		// * CONSTRUCTOR * [Complex] Construct by loading from a file.
		// Returns the size of the loaded texture.
		Engine::UPoint File(const std::string& fileName, Point relative_position);
		// * CONSTRUCTOR * [Complex] Construct a texture by writing it (limited to single foreground and background RGBA values)
		void Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point relative_position);

		// [Simple/Complex] Resize the texture, keeps previous values in the given size.
		// `UPoint newSize` - The new rectangle size of the texture.
		// `CellA newValue` - [Complex] The value which *only new cells* will be set to. [Simple] The new value *of all cells*.
		void Resize(UPoint newSize, CellA newValue);
		// [Simple/Complex] Change all the cells' values
		void SetCell(CellA value);
		// [Simple/Complex] Change all the cells' foreground color values
		void SetForeground(RGBA value);
		// [Simple/Complex] Change all the cells' background color values
		void SetBackground(RGBA value);
		// [Simple/Complex] Change all the cells' character values
		void SetCharacter(char value);
		// [Simple/Complex] Change all the cells' alpha values
		void SetAlpha(uint8_t value);
		
		// [Simple/Complex] Get the size of the texture.
		UPoint GetSize() const;
		// [Complex] Export the texture to a file (existent or non existent), uses standard ktech texture file format.
		void ExportToFile(const std::string& fileName) const;
	};

	struct Collider
	{
		// `True` - Allow `Object::Move()` to process this collider.
		// `False` - Disallow `Object::Move()` to process this collider.
		bool active = true;
		// Position relative to the collider's parent object.
		Point pos_r = { 0, 0 };
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

		// * CONSTRUCTOR * [Simple] Construct a rectangle.
		void Simple(UPoint size, uint8_t type, Point relative_position);
		// * CONSTRUCTOR * [Complex] Load from a file.
		bool File(const std::string& fileName, uint8_t type, Point relative_position);
		// * CONSTRUCTOR * [Complex] Construct a collider by writing it.
		void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relative_position);
		// * CONSTRUCTOR * [Complex] Construct a collider which corrosponds to the characters of a texture (also inherits the texture's relative position).
		// Will activate a cell of the collider if its corrosponding texture cell's character isn't space (`' '`) and foreground alpha value is above or equal to the `alphaThreshold`.
		void ByTextureCharacter(const Texture& texture, uint8_t alphaThreshold, uint8_t type);
		// * CONSTRUCTOR * [Complex] Construct a collider which corrosponds to the background color of a texture (also inherits the texture's relative position)
		// Will activate a cell of the collider if its corrosponding texture cell's background alpha value is above or equal to the `alphaThreshold`.
		void ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t type);

		// [Simple/Complex] Get the size of the texture.
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
		// Callback function that gets called each time one of the events listed in the `EventType` enum occurs.
		// EventType - specifies which type of event this call is for.
		std::function<void(EventType)> OnEvent = nullptr;
		// Tries to move this object by processing colliders in the object's parent layer and determining if the object should move, push, or be blocked.
		// Returns `true` if managed to move, returns `false` if didn't manage to move.
		bool Move(Point dir);
		
		// Add the object to a layer.
		// If the object is already in a layer, which should be specified by `parentLayer`, then first remove this object from the old parent layer.
		void EnterLayer(Layer* layer);
		// `Point position` - 2D position, not relative to anything.
		// `Layer* layer` - the layer to automatically make the object enter into.
		// `std::string objectName` - The name for the object.
		Object(Point position = Point(0, 0), Layer* layer = nullptr, const std::string& objectName = "");

		~Object();
	};

	struct Layer
	{
		// Callback function which gets automatically called by `Map::CallOnTicks()`.
		std::function<void()> OnTick = nullptr;
		
		// Pointers to the objects that are in this layer.
		std::vector<Object*> objects = {};
		
		// Tells `Camera`s whether or not to render this layer.
		// `True` - render this layer.
		// `Flase` - don't render this layer.
		bool visible = true;
		// The alpha channel shared across all of the objects in the layer.
		// While similar to `visible`, this is not optimized to simply cancel the rendering of a layer.
		uint8_t alpha = 255;
		// Foreground color overlay.
		RGBA frgba = { 0, 0, 0, 0 };
		// Background color overlay.
		RGBA brgba = { 0, 0, 0, 0 };

		// Add an object to the objects vector.
		int AddObject(Object* object);
		// Remove an object by its name, returns false if couldn't find the object.
		bool RemoveObject(const std::string& name);
		// Remove an object by its pointer, returns false if couldn't find the object.
		bool RemoveObject(Object* object);
		// Reset the `parentLayer` of the objects that are in this layer and have the variable set to this layer
		~Layer();
	};
	
	struct AudioSource
	{
		bool loaded;

		// PortAudio's opaque stream pointer
		PaStream* stream;
		// Current sample
		uint32_t cur = 0UL;
		// Amount of samples
		uint32_t frames = 0UL;
		// The end point in samples
		uint32_t endpointToPlay = 0UL;
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

		std::function<void()> OnEnd;

		// Load .wav file.
		// `fileName` - the name of the file.
		// Returns true if succeeded
		AudioSource(const std::string& fileName, std::function<void()> OnEnd = nullptr);

		// Play the sound from the beginning.
		// start - the start point in samples. 0 means the first sample.
		// length - the length in samples. 0 means the entire sound.
		// volume - a float between 0.0f to 1.0f.
		// Returns false if the audio source isn't loaded. Otherwise, true.
		// If the audio source is already playing, it will restart the playing.
		bool Play(uint32_t start = 0, uint32_t length = 0, float volume = 1.0f);
		// Pause playing the sound.
		void Pause();
		// Resume playing the sound.
		void Resume();
		// Stop playing the sound.
		void Stop();
	};
	
	struct Camera
	{
		std::string name = "";
		Point pos = Point(0, 0);
		// The resolution of the camera, in cells. Changing this could potentially result in a segmentation fault. To change the resoultion, call `Camera::Resize()`.
		UPoint res = UPoint(10, 10);
		std::vector<std::vector<Cell>> image = {};

		// Callback function which gets automatically called by `Map::CallOnTicks()`.
		std::function<void()> OnTick = nullptr;

		Camera(Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
		
		// Convert a vector of layers (can be simply taken from a map) into a cell-based image, which will get stored within `image`.
		void Render(const std::vector<Layer*>& layers);
		// This function will draw what was rendered (Camera::image's contents) into the "final" image (Engine::image), which will be printed at once with Engine::Print.
		// pos is the position desired to draw the rendered image to the final image.
		// left, top, right and bottom are a rectangle representing the area of the rendered image desired to draw to the final image.
		// Setting right/bottom to 0 will default to the rendered image's size.
		void Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0) const;
		// Resizes `image` and updates `res`,
		void Resize(UPoint res);
	};
	
	struct Map
	{
	public:
		// Pointers to the cameras that are in this map.
		std::vector<Camera*> cameras = {};
		// Pointers to the layers that are in this map.
		std::vector<Layer*> layers = {};
		// The index of the active camera from the `cameras` vector.
		// Used by `Render()`, `RenderReversed()` and `Draw()` for executing camera operations with a comfortable writing convention.
		size_t activeCameraI = -1;

		// Callback function which gets automatically called by `Map::CallOnTicks()`.
		std::function<void()> OnTick = NULL;

		int AddCamera(Camera* camera, bool asActiveCamera = false);
		int AddLayer(Layer* layer);

		// Render using the active camera with a comfortable writing convention.
		// Returns `true` if managed to render (`activeCameraI` is valid), otherwise, `false`.
		bool Render() const;
		// Render in a reversed order using the active camera with a comfortable writing convention.
		// Returns `true` if managed to render (`activeCameraI` is valid), otherwise, `false`.
		bool RenderReversed() const;
		// Draw the active camera's stored image to the engine's final image with a comfortable writing convention.
		// Returns `true` if managed to draw (`activeCameraI` is valid), otherwise, `false`.
		bool Draw(Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0) const;
		// Calls all of the OnTick callback functions of the `Camera`s, `Layer`s and `Object`s within this map, and this `Map`'s OnTick itself as well.
		void CallOnTicks() const;
	};

	// Prepare the terminal for printing and receiving input.
	void PrepareTerminal(UPoint imageSize);
	// Reset the terminal before exiting the game (turn it back to normal for further terminal use).
	void ResetTerminal();
	// Prepare the audio library (PortAudio) for AudioSources.
	void InitializeAudio();
	// Terminate (deallocate) the audio library (PortAudio).
	void TerminateAudio();
	// Print the engine's final image (`Engine::image`) to the console.
	void Print();

	// Prints text to the terminal, text will only appear if you are not printing.
	void Log(const std::string& text, RGB color);

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

	namespace Input::K
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
};