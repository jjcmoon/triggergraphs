from triggergraphs import EDBLayer, Program


def test_basic():
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")
    assert program.get_n_rules() == 4
    assert str(program.get_rule(0)) == "neigh(A1,A2) :- neighborOf(A1,A2)"


def test_magic():
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")
    query = "locIn(belgium, europe)"
    new_program, old_id, new_id = program.apply_magic_transform(query)
    assert new_program.get_n_rules() == 3 # 2 locIn rules + 1 magic
    for i in range(new_program.get_n_rules()):
        print(new_program.get_rule(i))
