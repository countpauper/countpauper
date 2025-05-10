#include "Interpreter/TokenStream.h"
#include "Interpreter/Terms.h"


namespace Interpreter
{

InputToken::InputToken() :
    token(0)
{
}

InputToken::InputToken(hash_t token, SourceSpan ref) :
    token(token),
    reference(ref)
{
}

InputToken::operator std::string() const 
{
    return std::format("%x [%d-%d]", token, reference.from, reference.from + reference.length);
}

bool InputToken::operator==(const InputToken& other) const 
{ 
    return token == other.token && reference == other.reference; 
}
}
