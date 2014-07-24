from utility.format_exception import FormatException

class _MarketException(FormatException):
    pass

class TradeException(_MarketException):
    """Market does not trade in that product"""
    pass

class SupplyException(_MarketException):
    """Not enough supply"""
    pass

class CostException(SupplyException):
    """Not enough supply for that price"""
    pass

class DemandException(_MarketException):
    """Not enough supply"""
    pass

class PriceException(DemandException):
    """Not enough demand for that price"""
    pass
