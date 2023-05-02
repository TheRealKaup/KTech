/*

	  || /   ----  ----  ----
	 |||/\   |     |  _  |--
	||||  \  \___  \__/  \___  kaup's console game engine
	
	Docs will be available at https://www.kaup.dev/kcge. Capybaright � 2021-2022 Kaup. All rights reserved.

*/

#pragma once

#include <iostream> // Printing, strings, et cetera
#include <string> // to_string
#include <vector> // Heavily based
#include <functional> // For sharing functions with the engine for it to call on events
#include <thread> // For sleeping, time management.
#include <chrono> // Time management
#include <fstream> // Loading textures
#include <sys/ioctl.h> // Linux..?
#include <unistd.h> // Linux..?
#include <signal.h> // Linux..?
#include <linux/input.h>
#include "Config.hpp"

namespace Engine
{
	struct RGB;
	struct RGBA;
	struct Vector2D;
	struct UVector2D;
	struct Pixel;
	struct SuperChar;
	struct Texture;
	struct Collider;
	struct Object;
	struct Layer;
	struct Camera;
	struct Map;
	// struct AudioSource;
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

	extern int lastDownKey;
	extern int lastUpKey;
	extern int lastDownVirtualKey;
	extern int lastUpVirtualKey;
	// When you register a key that requires shift (for example '!' and 'W'), it will set lastDownKeyRequiresShift and lastUpKeyRequiresShift when triggered.
	// Important: the engine will not check if shift is down if a key requires it. The user needs to check it themselves. The engien doesn't do it because it's way too complex, 
	// resource costly and mostly unnecessary.
	extern bool lastDownKeyRequiresShift;
	extern bool lastUpKeyRequiresShift;

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
	extern std::vector<std::vector<Pixel>> image;
	// Instead of allocating a string (that will be printed to the console) every print, it is kept for next prints.
	extern std::string stringImage;

	struct RGB
	{
		unsigned char r, g, b;
		RGB(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);
	};

	struct RGBA
	{
		unsigned char r, g, b;
		float a;
		RGBA(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0, float alpha = 1.0f);
	};

	struct Vector2D
	{
		int x, y;
		Vector2D(int xAxis = 0, int yAxis = 0);
	};

	struct UVector2D
	{
		unsigned int x, y;
		UVector2D(unsigned int xAxis = 0, unsigned int yAxis = 0);
	};

	struct Pixel
	{
		char32_t character;
		RGB frgb;
		RGB brgb;
		Pixel(char32_t character = ' ', RGB foreground = {0, 0, 0}, RGB background = { 0, 0, 0 });
	};

	struct SuperChar
	{
		char32_t character;
		RGBA frgba;
		RGBA brgba;
		SuperChar(char32_t character = ' ', RGBA foreground = {255, 255, 255, 1.0f}, RGBA background = {0, 0, 0, 0.0f});
	};

	struct Texture
	{
		std::vector<std::vector<SuperChar>> t = {};
		Vector2D pos = {0, 0};
		bool active = true;
		
		// The constructors are functions instead of actual constructors because of my own preference.
		// Visual Studio has a hard time correctly showing the parameters when filling them, so I decided that
		// this is the best solution for making the writing experience easier.
		
		// A rectangle of the same value. (Limited to a single color and character)
		void Block(UVector2D size, SuperChar value, Vector2D pos);
		// Load from a file. (Requires file)
		void File(std::string fileName, Vector2D pos);
		// Create a texture by writing it. (Limited to a single color)
		void Write(std::vector<std::string> stringVector, RGBA frgba, RGBA brgba, Vector2D pos);
	};

	struct Collider
	{
		// A simple collider is a rectangle.
		bool simple = true;
		// Used if the collider is complex.
		std::vector<std::vector<bool>> c = {};
		// Used by both simple and complex colliders.
		Vector2D pos = { 0, 0 };
		// Used if the collider is simple.
		UVector2D size = { 0, 0 };

		// Turn the collider on/off.
		bool active = true;
		// The collider type which is defined by Engine::colliderTypes
		unsigned char type;

		// For a simple collider
		Collider(UVector2D size = {0, 0}, Vector2D pos = {0, 0}, int type = 0);
		// For a complex collider
		Collider(std::vector<std::vector<bool>> collider, Vector2D pos = { 0, 0 }, int type = 0);
	};

	struct Object
	{
		Layer* parentLayer = NULL; 

		std::function<void()> OnTick = NULL;
		
		std::string name = "Unnamed Object";
		Vector2D pos = { 0, 0 };

		std::vector<Texture> textures = {};
		std::vector<Collider> colliders = {};

		Object* theOtherObject = NULL;
		Vector2D lastPush = { 0, 0 };
		int theColliderIndex = -1;
		int theOtherColliderIndex = -1;

		// This event can be called after calling Move. OnPushed means that another object pushed this one.
		std::function<void()> OnPushed = NULL;
		// This event can be called after calling Move. OnPush means that this objects pushed another object.
		std::function<void()> OnPush = NULL;
		// This event can be called after calling Move. OnBlocked means that another objects blocked this one.
		std::function<void()> OnBlocked = NULL;
		// This event can be called after calling Move. OnBlock means that this object blocked another object.
		std::function<void()> OnBlock = NULL;

		bool Move(Vector2D dir);

		void ExpandMovementTree(Vector2D dir, std::vector<Object*>* pushingObjects, std::vector<Object*>* objectsToPush, std::vector<unsigned>* pushingColliders, std::vector<unsigned>* collidersToPush, std::vector<Object*>* blockedObjects, std::vector<Object*>* blockingObjects, std::vector<unsigned>* blockedColliders, std::vector<unsigned>* blockingColliders);

		Object(Vector2D position = {0U, 0U}, std::string objectName = "Unnamed Object");
	};

	struct Layer
	{
		bool active = true;

		std::function<void()> OnTick = NULL;

		std::vector<Object*> objects = {};
		
		RGBA frgba = { 0, 0, 0, 0.0f };
		RGBA brgba = { 0, 0, 0, 0.0f };
		float opacity = 1.0f;

		int AddObject(Object* object);
		bool RemoveObject(int index);
		bool RemoveObject(std::string name);
	};
	/*
	struct AudioSource
	{
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
		unsigned char* data = nullptr;

		// Load .wav file.
		// volumes - list of volumes to set for the channels. nullptr = full volume.
		// fileName - the name of the file.
		// loopCount - times to loop the sound. 0/1 means no loop, plays once.
		AudioSource(LPCWSTR fileName);
		
		// Play the sound from the beginning.
		// start - the start point in samples. 0 means the first sample.
		// length - the length in samples . 0 means the entire sound.
		void Play(unsigned long start = 0, unsigned long length = 0, unsigned short loops = 0, std::vector<float> volumes = {});

		// Pause playing the sound.
		void Pause();

		// Resume playing the sound.
		void Resume();
		
		// Stop playing the sound.
		void Stop();

		// Change volume for all the channels.
		// volume - between 0.0f to 1.0f that will be the volume of all the channels.
		void ChangeVolume(float volume);

		// Change volumes of every specific channel.
		// volumes - a vector of floats between 0.0f to 1.0f, each element will be the volume of each corresponding channel.
		void ChangeVolumes(std::vector<float> volumes = {});
	};
	*/
	struct Camera
	{
		std::function<void()> OnTick = NULL;
		std::string name = "Unnamed Camera";
		Vector2D pos = { 0, 0 };
		UVector2D res = { 10U, 10U };
		std::vector<std::vector<Pixel>> image = {};

		Camera(Vector2D position = {0, 0}, UVector2D resolution = {0, 0}, std::string cameraName = "Unnamed Camera");
		
		void Render(std::vector<Layer*> layers);
		// This function will draw what was rendered (Camera::image's contents) into the "final" image (Engine::image), which will be printed at once with Engine::Print.
		// pos is the position desired to draw the rendered image to the final image.
		// left, top, right and bottom are a rectangle representing the area of the rendered image desired to draw to the final image.
		// Setting right/bottom to 0 will default to the rendered image's size.
		void Draw(Vector2D pos = {0, 0}, unsigned left = 0U, unsigned top = 0U, unsigned right = 0U, unsigned bottom = 0U);
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
		int Seconds();
		// Get an int which represents the time point in milliseconds
		int Milliseconds();
		// Get an int which represents the time point in microseconds
		int Microseconds();
		// Get an int which represents the time point in nanoseconds
		int Nanoseconds();
	};

	bool AreObjectsOverlapping(Engine::Object* objA, Engine::Object* objB, bool ignoreIrrelevancy = false);

	// Use ASCII. For Virtual-Key Codes set vk to true, which if set, will not try to translte from ASCII to VK.
	void RegisterInputHandler(int key, bool onKeyDown, std::function<void()> function, bool vk = false);

	// Storage, could be used for many things.
	extern std::vector<Object> storedObjects;
	extern std::vector<Layer> storedLayers;
	// If you don't want to store an object yourself, add it to this function which will store it for you. Add the returned pointer to the desired layer.
	// This is used by the engine when you load a map.
	Object* StoreObject(Object object);
	// If you don't want to store a layer yourself, add it to this function which will store it for you. Add the returned pointer to the desired map.
	// This is used by the engine when you load a map.
	Layer* StoreLayer(Layer layer);

	// Prepare the audio library (XAudio2) for AudioSources.
	void InitializeAudio();
	// Prepare for printing
	void PreparePrint(UVector2D imageSize);
	// Call OnKey functions according to key states.
	void ManageInputs();
	// Print the final image to the console.
	void Print();
	// Wait the precise amount of time to fill the frame according to the ticks per second value.
	void WaitUntilNextTick();
	// Give this function a map and it will call all the OnKey functions of the layers, objects and cameras (map itself included) in the map.
	// This way, you don't have to write the very long and ugly for loops to do so.
	void CallOnTicks(Map* map);
};
