from typing import Any, Callable

#arjan codes: https://github.com/ArjanCodes/examples/blob/main/2023/monad/functor.py
# A functor is a class of functions that
#  1) contain a value
#  2) Provide a function to operate on that value
# in this case the factor has a map function, which allows an arbitrary other function to be applied to the value
# An Endo Functor return the same form of object (eg class) from the function (2). In this
class EndoFunctor:
    def __init__(self, value: Any) -> None:
        self.value = value

    def map(self, func: Callable[[Any], Any]) -> "Functor":
        return EndoFunctor(func(self.value))


def add_one(x: int) -> int:
    return x + 1


def multiply_by_two(x: int) -> int:
    return x * 2


def main() -> None:
    f = EndoFunctor(5)

    # Mapping within the same category (Functor -> Functor)
    g = f.map(add_one)  # g is also a Functor instance

    # Preserving structure
    assert isinstance(g, EndoFunctor)

    # Preserving composition
    # NB: this doesn't mean that it's symetrical ie map(a).map(b) == map(b).map(a).
    # Only that for the monadic function `map`
    #  x.map(a).map(b) is the same as directly b(a(x))
    #  the order is reversed because a is executed first, then b which with the Functor is read left to right.
    assert (
        f.map(add_one).map(multiply_by_two).value
        == f.map(lambda x: multiply_by_two(add_one(x))).value
    )


if __name__ == "__main__":
    main()