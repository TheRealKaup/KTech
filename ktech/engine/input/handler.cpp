/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

#include "handler.hpp"
#include "callback.hpp"

void KTech::Input::Handler::RemoveCallbacksSetToBeDeleted()
{
	for (size_t i = 0; i < m_callbacks.size();)
	{
		// `Callback`s which were set to `Callback::Status::awaitingDeletion` were set to be deleted by its `CallbacksGroup`
		if (m_callbacks[i]->status == Callback::Status::awaitingDeletion)
		{
			m_callbacks.erase(m_callbacks.begin() + i);
		}
		else
		{
			i++;
		}
	}
}