from triggergraphs import EDBLayer, Program


def test_basic():
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")
    assert program.get_n_rules() == 4
    assert str(program.get_rule(0)) == "neigh(A1,A2) :- neighborOf(A1,A2)"
