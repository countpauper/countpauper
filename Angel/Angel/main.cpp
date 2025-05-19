#include "Engine/Interpreter.h"
#include "Engine/Defaults.h"
#include "Logic/Expression.h"
#include "Interpreter/Source.h"
#include "Interpreter/Error.h"
#include <iostream>
#include <string>
#include <unistd.h>

namespace Angel
{

int IOLoop(const std::string_view prompt="")
{
	Engine::AngelInterpreter interpreter; 
	Logic::Knowledge knowledge;
	Engine::AddDefaults(knowledge);
    while (true)    // ctrl-C to exit
	{
		std::cout << prompt;
		std::string line;
        if (!std::getline(std::cin, line))
			break;
		try 
		{
			Interpreter::Source src(line);
			try 
			{
				Logic::Expression query = interpreter.InterpretQuery(src);
				auto response = knowledge.Infer(query);
				std::cout << response << std::endl;
			} 
			catch(Interpreter::Error&)
			{
				Interpreter::Source src(line);
				interpreter.Interpret(src, knowledge);
			}
		}
		catch(const Interpreter::Error& err)
		{
			std::cerr << err.what() << " at `" << err.Location().extract() << 
				"` [" << err.Location().from <<"]" << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
    }
	return 0;
}

int InteractiveConsole()
{
	std::cout << "Angel 0.0.1. ctlr-C to exit" << std::endl;
    IOLoop(">");
	return 0;
}

};

int main(int argc, char* argv[])
{
	// TODO interpret arguments and apply options
	if (isatty(fileno(stdin)))
		return Angel::InteractiveConsole();
	else 
		return Angel::IOLoop();
}

