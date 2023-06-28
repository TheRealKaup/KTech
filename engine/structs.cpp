#include "engine.hpp"

Engine::TimePoint::TimePoint()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}
void Engine::TimePoint::SetToNow()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}
long Engine::TimePoint::Seconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}
long Engine::TimePoint::Milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}
long Engine::TimePoint::Microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}
long Engine::TimePoint::Nanoseconds()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

// Engine::Texture::Texture() {};
void Engine::Texture::Rectangle(UPoint size, CellA value, Point pos) {
	this->pos = pos;
	t.resize(size.y);
	for (size_t y = 0; y < t.size(); y++)
	{
		t[y].resize(size.x, value);
		for (size_t x = 0; x < t[y].size(); x++)
			t[y][x] = value;
	}
}
void Engine::Texture::File(const std::string& fileName, Point pos) {
	this->pos = pos;
	std::ifstream file(fileName);
	if (!file.is_open())
		return;
	std::string line;

	CellA value;

	for (size_t y = 0; std::getline(file, line); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		t.resize(t.size() + 1);
		for (size_t x = 0, j = 0; x < line.length(); x++, j++)
		{
			if (j == 9)
				j = 0;
			if (j == 0)
				value.f.r = (unsigned char)line[x];
			else if (j == 1)
				value.f.g = (unsigned char)line[x];
			else if (j == 2)
				value.f.b = (unsigned char)line[x];
			else if (j == 3)
				value.f.a = (unsigned char)line[x];
			else if (j == 4)
				value.b.r = (unsigned char)line[x];
			else if (j == 5)
				value.b.g = (unsigned char)line[x];
			else if (j == 6)
				value.b.b = (unsigned char)line[x];
			else if (j == 7)
				value.b.a = (unsigned char)line[x];
			else if (j == 8)
			{
				value.c = line[x];
				t[y].push_back(value);
			}
		}
	}
}
void Engine::Texture::Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point pos) {
	this->pos = pos;
	t.resize(stringVector.size());
	for (size_t y = 0; y < stringVector.size(); y++)
	{
		t[y].resize(stringVector[y].length());
		for (size_t x = 0; x < stringVector[y].length(); x++)
		{
			t[y][x].c = stringVector[y][x];
			t[y][x].f = frgba;
			t[y][x].b = brgba;
		}
	}
}

void Engine::Texture::SetCell(CellA value)
{
	for (unsigned y = 0; y < t.size(); y++)
		for (unsigned x = 0; x < t[y].size(); x++)
			t[y][x] = value;
}
void Engine::Texture::SetForeground(RGBA value)
{
	for (unsigned y = 0; y < t.size(); y++)
		for (unsigned x = 0; x < t[y].size(); x++)
			t[y][x].f = value;
}
void Engine::Texture::SetBackground(RGBA value)
{
	for (unsigned y = 0; y < t.size(); y++)
		for (unsigned x = 0; x < t[y].size(); x++)
			t[y][x].b = value;
}
void Engine::Texture::SetCharacter(char value)
{
	for (unsigned y = 0; y < t.size(); y++)
		for (unsigned x = 0; x < t[y].size(); x++)
			t[y][x].c = value;
}

Engine::Object::Object(Point pos, const std::string& name) : pos(pos), name(name) {}

Engine::Object::~Object()
{
	if (parentLayer)
		parentLayer->RemoveObject(this);
}