from triggergraphs import EDBLayer, Program, Wizard


def test_basic():
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")
    wizard = Wizard()
    query = "locIn(belgium, europe)"
    new_program, old_id, new_id = wizard.rewrite_program(program, query)
    assert new_program.get_n_rules() == 3 # 2 locIn rules + 1 magic
    for i in range(new_program.get_n_rules()):
        print(new_program.get_rule(i))
