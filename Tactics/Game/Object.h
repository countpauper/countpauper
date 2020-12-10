#pragma once

#include "Game/Position.h"
#include "Identity.h"
#include "Engine/Identification.h"
#include "Body.h"
#include "Game.h"

namespace Game
{

	class Object : public Identity, public Engine::Identification
	{
	public:
		Object();
		virtual ~Object() = default;
		virtual void Turn() = 0;
		virtual void Activate(const Game& game) = 0;
		virtual void Render() const = 0;
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

