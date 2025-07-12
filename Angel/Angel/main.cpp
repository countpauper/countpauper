#include "Engine/Interpreter.h"
#include "Engine/Defaults.h"
#include "Logic/Expression.h"
#include "Interpreter/Source.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"
#include "Interpreter/Logging.h"
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
			if (Interpreter::rtrim(line).back()=='?')
			{
				Logic::Expression query = interpreter.InterpretQuery(src);
				Logging::Log<Logging::INFO>("? {}", Logic::to_string(query));
				auto response = knowledge.Infer(query);
				std::cout << response << std::endl;
			} 
			else 
			{
				Interpreter::Source src(line);
				interpreter.Interpret(src, knowledge);
			}
		}
		catch(const Interpreter::Error& err)
		{
			std::cerr << err.what() << " at `" << err.Location().extract() << 
				"` " << std::string(err.Location())  << std::endl;
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
	if (isatty(fileno(stdin)))
		return Angel::InteractiveConsole();
	else 
		return Angel::IOLoop();
}

