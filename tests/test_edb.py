from triggergraphs import EDBLayer


def test_basic():
    edb = EDBLayer("tests/data1/edb.conf")
    predicates = edb.get_predicates()
    assert set(predicates) == {"locatedIn", "neighborOf"}
    assert edb.get_n_predicates() == 2