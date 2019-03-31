#pragma once

#include "Position.h"
#include "Identity.h"
#include "Body.h"
#include "Game.h"

namespace Game
{

	class Object : public Identity
	{
	public:
		Object();
		virtual ~Object() = default;
		virtual void Turn() = 0;
		virtual void Activate(const Game& game) = 0;
		virtual void Render() const = 0;
		unsigned Tag() const;
		virtual bool Prone() const;
		void Move(int dx, int dy);
		Position GetPosition() const override;
		std::wstring Description() const override;
		Body body;    // TODO: different body parts for different objects/creatures
	protected:
		Position position;
		std::wstring name;
	};
}   // ::Game

