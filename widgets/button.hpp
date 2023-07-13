#include "widget.hpp"

class Button : public Widget
{
public:
	bool down = false;

	std::function<void()> OnPress;
	
	Engine::RGBA unselectedRGBA, selectedRGBA, downRGBA;

private:
	void RemovePressColor()
	{
		if (selected)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(selectedRGBA);
		else
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(unselectedRGBA);
	}

	void InsideOnPress()
	{
		if (selected)
		{
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(downRGBA);
			
			Engine::Time::Invoke(std::bind(&Button::RemovePressColor, this), 100, Engine::Time::TimeMeasurement::milliseconds);

			if (OnPress)
				OnPress();
		}
	}

public:
	void Select()
	{
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(selectedRGBA);
		selected = true;
	}  

	void Deselect()
	{
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(unselectedRGBA);
		selected = false;
	}

	Button(Engine::Layer* layer,
		std::function<void()> OnPress,
		std::string key = kReturn,
		Engine::Point pos = { 0, 0 },
		const std::string& text = "Button",
		bool withFrame = false,
		Engine::RGBA unselectedRGBA = Engine::RGBA( 150, 150, 150, 255 ),
		Engine::RGBA selectedRGBA = Engine::RGBA( 255, 255, 255, 255 ),
		Engine::RGBA downRGBA = Engine::RGBA(150, 150, 255, 255))
		: OnPress(OnPress), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA), downRGBA(downRGBA)
	{
		obj.pos = pos;

		// Texture
		if (withFrame)
		{	
			obj.textures.resize(9);
			// text
			obj.textures[0].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
			// up-left corner
			obj.textures[1].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 0));
			// up-right corner
			obj.textures[2].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length(), 0));
			// bottom-left corner
			obj.textures[3].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 2));
			// bottom-right corner
			obj.textures[4].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length(), 2));
			// up frame
			obj.textures[5].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 0));
			// left frame
			obj.textures[6].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(0, 1));
			// bottom frame
			obj.textures[7].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 2));
			// right frame
			obj.textures[8].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(1 + text.length(), 1));
		}
		else
		{
			obj.textures.resize(1);
			// text
			obj.textures[0].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
		}

		// Input handlers
		Engine::Input::RegisterHandler(key, std::bind(&Button::InsideOnPress, this), true);
		
		// Add object
		layer->AddObject(&obj);
	}
};