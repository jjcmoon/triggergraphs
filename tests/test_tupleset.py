from triggergraphs import TupleSet


def test_empty_tupleset():
    """Test creating empty TupleSet"""
    t = TupleSet()
    assert t.get_n_facts() == 0
