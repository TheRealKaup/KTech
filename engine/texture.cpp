#include "engine.hpp"

void Engine::Texture::Simple(UPoint _size, CellA _value, Point _pos) {
	simple = true;
	pos_r = _pos;
	size = _size;
	value = _value;
}

void Engine::Texture::Rectangle(UPoint _size, CellA _value, Point _pos)
{
	simple = false;
	pos_r = _pos;
	t.clear();
	t.resize(_size.y, std::vector<CellA>(_size.x, _value));
}

Engine::UPoint Engine::Texture::File(const std::string& fileName, Point _pos)
{
	simple = false;
	pos_r = _pos;

	Log("<Engine::Texture::File()> Opening file " + fileName + "." , RGB(128, 128, 255));
	// Open file
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		Log("<Engine::Texture::File()> Error! Failed to open file " + fileName + ".", RGB(255, 0, 0));
		return UPoint(0, 0);
	}

	Log("<Engine::Texture::File()> Reading size of texture.", RGB(128, 128, 255));
	// Get the texture size (first 8 bytes of the file)
	UPoint maxTextureSize;
	file.read((char*)&maxTextureSize, 8);
	Log("<Engine::Texture::File()> Size of texture in file: { " + std::to_string(maxTextureSize.x) + ", " + std::to_string(maxTextureSize.y) + " }.", RGB(128, 128, 255));
	if (maxTextureSize.y > 9999)
	{
		Log("<Engine::Texture::File()> Warning! Y size of texture in file is too big. Scaling Y down to 9999.", RGB(255, 0, 0));
		maxTextureSize.y = 9999;
	}
	if (maxTextureSize.x > 9999)
	{
		Log("<Engine::Texture::File()> Warning! X size of texture in file is too big. Scaling X down to 9999.", RGB(255, 0, 0));
		maxTextureSize.x = 9999;
	}

	Log("<Engine::Texture::File()> Resizing texture to fit texture in file.", RGB(128, 128, 255));
	// Resize the texture
	t.clear();
	t.resize(maxTextureSize.y, std::vector<CellA>(maxTextureSize.x));

	Log("<Engine::Texture::File()> Reading rest of file and writing to texture.", RGB(128, 128, 255));
	// Read and write to the texture	
	for (size_t y = 0; y < t.size(); y++)
	{
		for (size_t x = 0; x < t[y].size(); x++)
		{
			if (file.read((char*)&t[y][x], 9).eof())
			{
				y = t.size();
				break;
			}
		}
	}

	// Print old texture file
	Log("<Engine::Texture::File()> Printing texture:", RGB(128, 128, 255));
	for (std::vector<CellA>& row : t)
	{
		for (CellA& cell : row)
		{
			std::cout << "\033[38;2;" << std::to_string(cell.f.r * cell.f.a / 255) << ';' << std::to_string(cell.f.g * cell.f.a / 255) << ';'<< std::to_string(cell.f.b * cell.f.a / 255) << 'm'
				<< "\033[48;2;" << std::to_string(cell.b.r * cell.b.a / 255) << ';' << std::to_string(cell.b.g * cell.b.a / 255) << ';'<< std::to_string(cell.b.b * cell.b.a / 255) << 'm' << cell.c;
		}
		std::cout << "\033[m" << std::endl;
	}
	std::cout << std::flush;

	Log("<Engine::Texture::File()> Returning.", RGB(128, 128, 255));
    return maxTextureSize;
}

void Engine::Texture::Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point _pos) {
	simple = false;
	pos_r = _pos;
	t.resize(stringVector.size());
	for (size_t y = 0; y < stringVector.size(); y++)
	{
		t[y].resize(stringVector[y].length());
		for (size_t x = 0; x < stringVector[y].length(); x++)
			t[y][x] = CellA(stringVector[y][x], frgba, brgba);
	}	
}

void Engine::Texture::Resize(UPoint newSize, CellA _value)
{
	if (simple)
	{
		size = newSize;
		value = _value;
	}
	else
	{
		t.resize(newSize.y);
		for (std::vector<CellA>& row : t)
			row.resize(newSize.x, _value);
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

void Engine::Texture::SetAlpha(uint8_t _value)
{
	if (simple)
	{
		value.f.a = _value;
		value.b.a = _value;
	}
	else
	{
		for (size_t y = 0; y < t.size(); y++)
		{
			for (size_t x = 0; x < t[y].size(); x++)
			{
				t[y][x].f.a = _value;
				t[y][x].b.a = _value;
			}
		}
	}
}

Engine::UPoint Engine::Texture::GetSize()
{
	// If the texture is simple, then `size` already represents the texture's size.
	if (simple)
		return size;
	// Otherwise, go over the entire texture to get the maximum X size, and use that to tell the size.
	UPoint size(0, t.size());
	for (size_t y = 0; y < t.size(); y++)
		if (t[y].size() > size.x)
			size.x = t[y].size();
	return size;
}

void Engine::Texture::ExportToFile(const std::string& fileName)
{
	// Create/open file
	std::ofstream file(fileName);
	// Get the size of the texture
	UPoint maxTextureSize(0, t.size());
	for (size_t y = 0; y < t.size(); y++)
		if (t[y].size() > maxTextureSize.x)
			maxTextureSize.x = t[y].size();
	// Write the size of the texture at the start of the file
	file.write((char*)&maxTextureSize, 8);
	// Write the texture itself
	for (size_t y = 0; y < maxTextureSize.y; y++)
	{
		for (size_t x = 0; x < maxTextureSize.x; x++)
		{
			if (x < t[y].size())
				file.write((char*)&t[y][x], 9);
			else
				file.write("\0\0\0\0\0\0\0\0\0", 9);
		}
	}
}