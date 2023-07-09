#include "engine.hpp"

void Engine::Texture::Simple(UPoint _size, CellA _value, Point _pos) {
	simple = true;
	pos = _pos;
	size = _size;
	value = _value;
}

void Engine::Texture::Rectangle(UPoint _size, CellA _value, Point _pos)
{
	simple = false;
	pos = _pos;
	
	t.resize(_size.y, std::vector<CellA>(_size.y, _value));
	for (std::vector<CellA>& row : t)
	{
		row.resize(_size.x);
		for (CellA& cell : row)
			cell = _value;
	}
}

bool Engine::Texture::File(const std::string& fileName, Point _pos) {
	simple = false;
	pos = _pos;
	
	std::ifstream file(fileName);
	if (!file.is_open())
		return false;
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
    
    return true;
}

void Engine::Texture::Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point _pos) {
	simple = false;
	pos = _pos;
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

void Engine::Texture::SetCell(CellA _value)
{
	if (simple)
		value = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x] = _value;
}

void Engine::Texture::SetForeground(RGBA _value)
{
	if (simple)
		value.f = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].f = _value;
}

void Engine::Texture::SetBackground(RGBA _value)
{
	if (simple)
		value.b = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].b = _value;
}

void Engine::Texture::SetCharacter(char _value)
{
	if (simple)
		value.c = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].c = _value;
}