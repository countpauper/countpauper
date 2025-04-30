#include "Interpreter/TokenStream.h"
#include "Interpreter/Tokens.h"


namespace Interpreter
{

InputToken::InputToken() :
    token(0)
{
}

InputToken::InputToken(hash_t token, size_t from, size_t length) :
    token(token),
    reference(from, length)
{
}



}
