#include "engine.hpp"

// ---=== Engine Variables ===---

// Data
std::vector<Engine::Object> Engine::storedObjects = {};
std::vector<Engine::Layer> Engine::storedLayers = {};
char Engine::input[8] = {0};
winsize Engine::terminalSize;
// Time
float Engine::potentialfps = 0.0f;
float Engine::fps = 0.0f;
Engine::TimePoint Engine::thisTickStartTP;
int Engine::tps = 12;
long int Engine::deltaTime = 0;
Engine::TimePoint Engine::engineStartTP;
long int Engine::totalTicks = 0;
// Config
std::function<void()> Engine::GlobalOnTick = NULL;
bool Engine::running = true;
// Physics
std::vector<std::vector<unsigned char>> Engine::colliderTypes = {
	{ 0, 1, 2 }, // Heavy - 0
	{ 0, 1, 2 }, // Normal - 1
	{ 2, 2, 2 } // Overlappable - 2
};
// Console
std::vector<std::vector<Engine::Pixel>> Engine::image = {};
std::string Engine::stringImage = "";

// ---=== Engine Functions ===---

Engine::Object* Engine::StoreObject(Object object)
{
	storedObjects.push_back(object);
	return &storedObjects[storedObjects.size() - 1];
}
Engine::Layer* Engine::StoreLayer(Layer layer)
{
	storedLayers.push_back(layer);
	return &storedLayers[storedLayers.size() - 1];
}

void Engine::Print()
{
	// Get terminal size
	ioctl(fileno(stdout), TIOCGWINSZ, &Engine::terminalSize);

	// Obtain the maximum length for the stringImage
	if (image.size() == 0)
		return;

	// Resize the stringImage if needed
	unsigned maxStringSize = image.size() * 4; // reserved for '\n'
	for (unsigned y = 0; y < image.size(); y++)
		maxStringSize += image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (stringImage.size() < maxStringSize) // resize
		stringImage.resize(maxStringSize);

	// Write the image to stringImage
	unsigned int l = 0;
	unsigned char lfr = 0, lfg = 0, lfb = 0, lbr = 0, lbg = 0, lbb = 0;

	// "&& y < size.ws_row" - fit into the terminal, in the case that it is too small
	for (unsigned y = 0; y < image.size() && y < terminalSize.ws_row; y++)
	{
		if (y != 0) {
			stringImage[l] = '\n';
			l++;
		}
		// First char in the line (optimizations)
		lfr = image[y][0].frgb.r;
		lfg = image[y][0].frgb.g;
		lfb = image[y][0].frgb.b;
		stringImage[l] = '\033';
		stringImage[l + 1] = '[';
		stringImage[l + 2] = '3';
		stringImage[l + 3] = '8';
		stringImage[l + 4] = ';';
		stringImage[l + 5] = '2';
		stringImage[l + 6] = ';';
		stringImage[l + 7] = lfr / 100 + '0';
		stringImage[l + 8] = (lfr % 100) / 10 + '0';
		stringImage[l + 9] = lfr % 10 + '0';
		stringImage[l + 10] = ';';
		stringImage[l + 11] = lfg / 100 + '0';
		stringImage[l + 12] = (lfg % 100) / 10 + '0';
		stringImage[l + 13] = lfg % 10 + '0';
		stringImage[l + 14] = ';';
		stringImage[l + 15] = lfb / 100 + '0';
		stringImage[l + 16] = (lfb % 100) / 10 + '0';
		stringImage[l + 17] = lfb % 10 + '0';
		stringImage[l + 18] = 'm';
		lbr = image[y][0].brgb.r;
		lbg = image[y][0].brgb.g;
		lbb = image[y][0].brgb.b;
		stringImage[l + 19] = '\033';
		stringImage[l + 20] = '[';
		stringImage[l + 21] = '4';
		stringImage[l + 22] = '8';
		stringImage[l + 23] = ';';
		stringImage[l + 24] = '2';
		stringImage[l + 25] = ';';
		stringImage[l + 26] = lbr / 100 + '0';
		stringImage[l + 27] = (lbr % 100) / 10 + '0';
		stringImage[l + 28] = lbr % 10 + '0';
		stringImage[l + 29] = ';';
		stringImage[l + 30] = lbg / 100 + '0';
		stringImage[l + 31] = (lbg % 100) / 10 + '0';
		stringImage[l + 32] = lbg % 10 + '0';
		stringImage[l + 33] = ';';
		stringImage[l + 34] = lbb / 100 + '0';
		stringImage[l + 35] = (lbb % 100) / 10 + '0';
		stringImage[l + 36] = lbb % 10 + '0';
		stringImage[l + 37] = 'm';
		stringImage[l + 38] = image[y][0].character;
		l += 39;
		for (unsigned x = 1; x < image[y].size() && x < terminalSize.ws_col; x++)
		{
			// foreground
			if (image[y][x].character != ' ')
			{
				if (image[y][x].frgb.r != lfr || image[y][x].frgb.g != lfg || image[y][x].frgb.b != lfb)
				{
					lfr = image[y][x].frgb.r;
					lfg = image[y][x].frgb.g;
					lfb = image[y][x].frgb.b;

					stringImage[l] = '\033';
					stringImage[l + 1] = '[';
					stringImage[l + 2] = '3';
					stringImage[l + 3] = '8';
					stringImage[l + 4] = ';';
					stringImage[l + 5] = '2';
					stringImage[l + 6] = ';';
					stringImage[l + 7] = lfr / 100 + '0';
					stringImage[l + 8] = (lfr % 100) / 10 + '0';
					stringImage[l + 9] = lfr % 10 + '0';
					stringImage[l + 10] = ';';
					stringImage[l + 11] = lfg / 100 + '0';
					stringImage[l + 12] = (lfg % 100) / 10 + '0';
					stringImage[l + 13] = lfg % 10 + '0';
					stringImage[l + 14] = ';';
					stringImage[l + 15] = lfb / 100 + '0';
					stringImage[l + 16] = (lfb % 100) / 10 + '0';
					stringImage[l + 17] = lfb % 10 + '0';
					stringImage[l + 18] = 'm';
					l += 19;
				}
			}
			// background
			if (image[y][x].brgb.r != lbr || image[y][x].brgb.g != lbg || image[y][x].brgb.b != lbb)
			{
				lbr = image[y][x].brgb.r;
				lbg = image[y][x].brgb.g;
				lbb = image[y][x].brgb.b;

				if (lbr == 0 && lbg == 0 && lbb == 0)
				{
					stringImage[l] = '\033';
					stringImage[l + 1] = '[';
					stringImage[l + 2] = '4';
					stringImage[l + 3] = '8';
					stringImage[l + 4] = ';';
					stringImage[l + 5] = '2';
					/*stringImage[l + 6] = ';';
					stringImage[l + 7] = '0';
					stringImage[l + 8] = ';';
					stringImage[l + 9] = '0';
					stringImage[l + 10] = ';';
					stringImage[l + 11] = '0';*/
					stringImage[l + 6] = 'm';
					l += 7;
				}
				else
				{
					stringImage[l] = '\033';
					stringImage[l + 1] = '[';
					stringImage[l + 2] = '4';
					stringImage[l + 3] = '8';
					stringImage[l + 4] = ';';
					stringImage[l + 5] = '2';
					stringImage[l + 6] = ';';
					stringImage[l + 7] = lbr / 100 + '0';
					stringImage[l + 8] = (lbr % 100) / 10 + '0';
					stringImage[l + 9] = lbr % 10 + '0';
					stringImage[l + 10] = ';';
					stringImage[l + 11] = lbg / 100 + '0';
					stringImage[l + 12] = (lbg % 100) / 10 + '0';
					stringImage[l + 13] = lbg % 10 + '0';
					stringImage[l + 14] = ';';
					stringImage[l + 15] = lbb / 100 + '0';
					stringImage[l + 16] = (lbb % 100) / 10 + '0';
					stringImage[l + 17] = lbb % 10 + '0';
					stringImage[l + 18] = 'm';
					l += 19;
				}
			}
			stringImage[l] = image[y][x].character;
			l++;
		}
		stringImage[l] = '\033';
		stringImage[l + 1] = '[';
		stringImage[l + 2] = '0';
		stringImage[l + 3] = 'm';
		l += 4;
	}
	// ".substr(0, l)" - print until the end which was set by the writing sequence above
	std::cout << "\033[0;0H" << stringImage.substr(0, l);
}

void SignalHandler(int signal) {
	if (signal == SIGWINCH) {
		system("clear"); // Unfortunately
		Engine::Print();
	}
	if (signal == SIGINT) { // I just find it so cool these signals work :)
		// Show cursor and reset colors
		std::cout << "\033[?25h\033[0m";
		Engine::running = false;

		// Reenable canonical mode and echo
		termios terminalAttributes;
		tcgetattr(0, &terminalAttributes);
		terminalAttributes.c_lflag &= ICANON;
		terminalAttributes.c_lflag &= ECHO;
		tcsetattr(0, TCSADRAIN, &terminalAttributes);
	}
}

void Engine::PreparePrint(Engine::UVector2D imageSize)
{
	// Set cursor at 0,0, Hide cursor, clear screen
	std::cout << "\033[0;0H\033[?25l\033[2J";

	// Disable canonical mode and echo
	termios terminalAttributes;
	tcgetattr(0, &terminalAttributes);
	terminalAttributes.c_lflag &= ~ICANON;
	terminalAttributes.c_lflag &= ~ECHO;
	terminalAttributes.c_cc[VMIN] = 1;
	terminalAttributes.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &terminalAttributes);

	// Size the image
	image.resize(imageSize.y);
	for (size_t y = 0; y < image.size(); y++)
		image[y].resize(imageSize.x);
	unsigned maxStringSize = image.size() * 4; // reserved for '\n'
	for (unsigned y = 0; y < image.size(); y++)
		maxStringSize += image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (stringImage.size() < maxStringSize)
		stringImage.resize(maxStringSize);

	// Resize signal
	signal(SIGWINCH, SignalHandler);
	// Exit signal
	signal(SIGINT, SignalHandler);
}

void Engine::WaitUntilNextTick()
{
	Engine::deltaTime = NowInMicroseconds() - Engine::thisTickStartTP.Microseconds();
	Engine::potentialfps = 1000000.0f / Engine::deltaTime;
	std::this_thread::sleep_for(std::chrono::microseconds(1000000 / Engine::tps - Engine::deltaTime));
	Engine::deltaTime = NowInMicroseconds() - Engine::thisTickStartTP.Microseconds();
	Engine::fps = 1000000.0f / Engine::deltaTime;
}

void Engine::CallOnTicks(Engine::Map* map)
{
	if (Engine::GlobalOnTick)
		Engine::GlobalOnTick();

	if (map->OnTick)
		map->OnTick();

	for (size_t l = 0; l < map->layers.size(); l++)
	{
		if (map->layers[l]->OnTick)
			map->layers[l]->OnTick();

		for (size_t o = 0; o < map->layers[l]->objects.size(); o++)
		{
			if (map->layers[l]->objects[o]->OnTick)
				map->layers[l]->objects[o]->OnTick();
		}
	}
	for (size_t c = 0; c < map->cameras.size(); c++)
	{
		if (map->cameras[c]->OnTick)
			map->cameras[c]->OnTick();
	}
}
