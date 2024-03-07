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

#include "ktech.hpp"

using namespace KTech;

template<typename T>
ID<T> Memory::Container<T>::Add(T* structure)
{
	KTech::IO::Log("<Container::Add()> Start of function...", RGBColors::lime);
	T** tmp = new T*[size + 1];
	KTech::IO::Log("<Container::Add()> Created new extended array " + std::to_string((size_t)tmp), RGBColors::lime);
	KTech::IO::Log("<Container::Add()> Move to new array", RGBColors::lime);
	for (size_t i = 0; i < size; i++)
		tmp[i] = arr[i];
	KTech::IO::Log("<Container::Add()> Add given structure", RGBColors::lime);
	structure->id.i = size;
	tmp[size] = structure;
	if (size > 0)
	{
		KTech::IO::Log("<Container::Add()> Delete old array " + std::to_string((size_t)arr), RGBColors::lime);
		delete[] arr;
	}
	KTech::IO::Log("<Container::Add()> Update info", RGBColors::lime);
	arr = tmp;
	size++;
	KTech::IO::Log("<Container::Add()> End of function, returning ID.", RGBColors::lime);
	return arr[size - 1]->id;
}

template<typename T>
inline bool Memory::Container<T>::Remove(const ID<T>& id)
{
	KTech::IO::Log("<Container::Remove()> Start of function...", RGBColors::lime);
	KTech::IO::Log("<Container::Remove()> Convert given ID to index", RGBColors::lime);
	size_t toRemove = IDToIndex(id);
	if (toRemove == size)
	{
		KTech::IO::Log("<Container::Remove()> End of function.", RGBColors::lime);
		return false;
	}
	T** tmp = new T*[size - 1];
	KTech::IO::Log("<Container::Remove()> Created new smaller array " + std::to_string((size_t)tmp), RGBColors::lime);
	KTech::IO::Log("<Container::Remove()> Moving to new array", RGBColors::lime);
	size_t i = 0;
	for (; i < toRemove; i++)
		tmp[i] = arr[i];
	for (i++; i < size; i++)
		tmp[i - 1] = arr[i];
	KTech::IO::Log("<Container::Remove()> Delete old array " + std::to_string((size_t)arr), RGBColors::lime);
	delete[] arr;
	KTech::IO::Log("<Container::Remove()> Update info", RGBColors::lime);
	size--;
	arr = tmp;
	KTech::IO::Log("<Container::Remove()> End of function.", RGBColors::lime);
	return true;
}

template<typename T>
inline size_t Memory::Container<T>::IDToIndex(const ID<T>& id)
{
	for (size_t i = (id.i < size ? id.i : size - 1);; i--)
	{
		if (arr[i]->id == id)
			return i;
		if (i == 0)
			return size;
	}
}

template<typename T>
inline size_t Memory::Container<T>::IDToIndex(ID<T>& id)
{
	for (size_t i = (id.i < size ? id.i : size - 1);; i--)
	{
		if (arr[i]->id == id)
		{	
			id.i = i;
			return i;
		}
		if (i == 0)
			break;
	}
	id.i = 0;
	return size;
}

template<typename T>
bool Memory::Container<T>::Exists(const ID<T>& id)
{
	if (IDToIndex(id) == size)
		return false;
	return true;
}

template<typename T>
bool Memory::Container<T>::Exists(ID<T>& id)
{
	if (IDToIndex(id) == size)
		return false;
	return true;
}

template<typename T>
T* Memory::Container<T>::operator[](ID<T>& id)
{
	for (size_t i = (id.i < size ? id.i : size -1);; i--)
	{
		if (arr[i]->id == id)
		{
			id.i = i;
			return arr[i];
		}
		if (i == 0)
			break;
	}
	id.i = 0;
	return nullptr;
}

template<typename T>
T* Memory::Container<T>::operator[](const ID<T>& id)
{
	for (size_t i = (id.i < size ? id.i : size -1);; i--)
	{
		if (arr[i]->id == id)
			return arr[i];
		if (i == 0)
			break;
	}
	return nullptr;
}

// Explicit instantiation
template class Memory::Container<Object>;
template class Memory::Container<Layer>;
template class Memory::Container<Camera>;
template class Memory::Container<Map>;
template class Memory::Container<Widget>;
template class Memory::Container<UI>;

void KTech::Memory::CallOnTicks()
{
	for (size_t i = 0; i < maps.size; i++)
		maps.arr[i]->OnTick();
	for (size_t i = 0; i < cameras.size; i++)
		cameras.arr[i]->OnTick();
	for (size_t i = 0; i < layers.size; i++)
		layers.arr[i]->OnTick();
	for (size_t i = 0; i < objects.size; i++)
		objects.arr[i]->OnTick();
}