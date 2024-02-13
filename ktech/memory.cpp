#include "ktech.hpp"

using namespace KTech;

template<typename T>
ID<T> Memory::Container<T>::Add(T* structure)
{
	KTech::IO::Log("<Container::Add()> Start of function...", RGBColors::lime);
	T** tmp = new T*[size + 1]; // New array with one more cell
	size_t i = 0; // Outside of for loop's scope for later use
	for (; i < size; i++)
		tmp[i] = arr[i]; // Move the cells to the new array
	structure->id.i = i; // Update the index of the ID within the structure
	tmp[i] = structure; // Create a new cell for the added structure
	if (size > 0)
		delete[] arr; // Delete the old array
	arr = tmp; // Set the array as the new array
	size++; // Increase the size by one
	KTech::IO::Log("<Container::Add()> End of function, returning ID.", RGBColors::lime);
	return arr[i]->id; // Return new ID of structure
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
inline bool Memory::Container<T>::Remove(const ID<T>& id)
{
	size_t toRemove = IDToIndex(id); // Convert the ID to a valid index
	if (toRemove == size) // If the index is the size, then it's invalid (structure is missing)
		return false;
	T** tmp = new T*[size - 1]; // New array with one less cell
	size_t i = 0;
	for (; i < toRemove; i++)
		tmp[i] = arr[i]; // Move the cells to the new array
	// Skip one cell in the new array
	for (; i < size; i++)
		tmp[i - 1] = arr[i]; // Move the cells to the new array
	delete[] arr; // Delete the old array
	arr = tmp; // Set the array as the new array
	size--; // Decrease the size by one
	return true;
}

template<typename T>
inline bool Memory::Container<T>::Delete(const ID<T>& id)
{
	size_t toRemove = IDToIndex(id); // Convert the ID to a valid index
	if (toRemove == size) // If the index is the size, then it's invalid (structure is missing)
		return false;
	T** tmp = new T*[size - 1]; // New array with one less cell
	size_t i = 0;
	for (; i < toRemove; i++)
		tmp[i] = arr[i]; // Move the cells to the new array
	// Skip one cell in the new array
	for (; i < size; i++)
		tmp[i - 1] = arr[i]; // Move the cells to the new array
	arr[toRemove]->id = ID<T>(0, 0); // Reset the ID
	delete arr[toRemove]; // Delete the structure from memory
	// ^ The ID was reset to prevent `Remove()` from overwritting this reallocation, since it was called by
	// the destructure called by `delete`.
	delete[] arr; // Delete the old array
	arr = tmp; // Set the array as the new array
	size--; // Decrease the size by one
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