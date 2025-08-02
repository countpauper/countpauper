#include "Engine/Interpreter.h"
#include "Engine/Defaults.h"
#include "Logic/Expression.h"
#include "Parser/Source.h"
#include "Parser/Error.h"
#include "Parser/Utils.h"
#include "Parser/Logging.h"
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
			Parser::Source src(line);
			if (Parser::rtrim(line).back()=='?')
			{
				Logic::Expression query = interpreter.InterpretQuery(src);
				Logging::Log<Logging::INFO>("? {}", Logic::to_string(query));
				auto response = knowledge.Infer(query);
				std::cout << response << std::endl;
			} 
			else 
			{
				Parser::Source src(line);
				interpreter.Interpret(src, knowledge);
			}
		}
		catch(const Parser::Error& err)
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

