#pragma once
#include <string>
#include "Logic/Knowledge.h"

namespace Angel
{
namespace Parser
{

Logic::Knowledge Parse(const std::wstring& text);


}
}