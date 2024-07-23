#pragma once
#include "Game/Action.h"

namespace Game
{

class Attack : public Action
{
public:
    Attack(Avatar& actor, const Game& game, Avatar& target);
    void Render() const override;
    void Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
private:
    Avatar& target;
};


}
