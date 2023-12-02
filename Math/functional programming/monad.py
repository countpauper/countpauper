# from arjan codes: https://github.com/ArjanCodes/examples/blob/main/2023/monad/monad.py
from __future__ import annotations
from typing import Any, Callable, Generic, TypeVar

# generics ensure that the "unit" type with which the monad is initialized
# is the same type as the monad that is passed one of the two arguments to bind
T = TypeVar("T")
# The monad does allow the underlying value of the type to be changed to the other parameter
U = TypeVar("U")


class Monad(Generic[T]):
    # The "unit" static method is equivalent to the constructor. It contains a base value Monad out of any type of value
    def __init__(self, value: T) -> None:
        self.value = value

    # The bind function makes this an Endo Functor, it does expect the passed function to already return a monad
    def bind(self, func: Callable[[T], Monad[U]]) -> Monad[U]:
        return func(self.value)

    # My own addition. The map function is more like the endofunctor and will create a new unit Monad?
    # is this really equivalent?
    def map(self, func: Callable[[T], U]) -> Monad[U]:
        return Monad(func(self.value))

    @staticmethod
    def unit(value: Any) -> Monad[Any]:
        return Monad(value)

    def __repr__(self):
        return f"Monad({self.value})"

def add_one(x: int) -> int:
    return x + 1

def multiply_by_two(x: int) -> int:
    return x * 2

def main() -> None:
    monad = Monad(5)

    # Left identity
    # unit(x).bind(f) == f(x)
    assert monad.bind(lambda x: Monad(add_one(x))).value == add_one(5)
    assert monad.map(add_one).value == add_one(5)

    assert monad.bind(lambda x: Monad(add_one(x))).bind(lambda y: Monad(multiply_by_two(y))).value == multiply_by_two(add_one(5))
    assert monad.map(add_one).map(multiply_by_two).value == multiply_by_two(add_one(5))

    # Right identity
    # m.bind(unit) == m
    assert monad.bind(Monad.unit).value == monad.value
    assert monad.bind(Monad).value == monad.value

    # Associativity
    # m.bind(f).bind(g) == m.bind(lambda x: f(x).bind(g))
    assert (
        monad.bind(lambda x: Monad(add_one(x)))
        .bind(lambda x: Monad(multiply_by_two(x)))
        .value
        == monad.bind(
            lambda x: Monad(add_one(x)).bind(lambda x: Monad(multiply_by_two(x)))
        ).value
    )

    assert (
        monad.map(add_one)
        .map(multiply_by_two)
        .value
        == monad.map(lambda x: multiply_by_two(add_one(x)))
        .value
    )


if __name__ == "__main__":
    main()