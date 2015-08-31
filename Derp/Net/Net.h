#pragma once
namespace Net
{
	namespace IO { typedef unsigned version;  }
	namespace Connection { class Base;	class Directed;  class Undirected; class Recurrent; }
	namespace Activation { class Function; class State; }
	namespace Layer { class Base; class Input; class Output; class Hidden; class Visible; typedef unsigned Id; }
	namespace Data { class Inputs; class Outputs; class Base;  class Sample; class Set; }
	class Network;
}

#include "Function.h"
#include "Connection.h"
#include "Layer.h"
#include "Data.h"
#include "Network.h"
#include "Activation.h"
#include "Learning.h"