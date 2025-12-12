from triggergraphs import EDBLayer, Program


def test_load_from_file():
    """Test loading rules from file"""
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    assert program.get_n_rules() == 0

    program.load_from_file("tests/data1/program.dlog")
    assert program.get_n_rules() == 4
    assert str(program.get_rule(0)) == "neigh(A1,A2) :- neighborOf(A1,A2)"


def test_add_rule():
    """Test adding and retrieving rules programmatically"""
    edb = EDBLayer()
    edb.add_csv_source("parent", [["a", "b"]])
    edb.add_csv_source("male", [["a"]])

    program = Program(edb)

    # Initially empty
    assert program.get_n_rules() == 0

    # Add simple rule
    rule_id = program.add_rule("p(X) :- q(X)")
    assert isinstance(rule_id, int)
    assert program.get_n_rules() == 1
    assert "p(" in str(program.get_rule(0))

    # Add recursive rule
    program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")
    assert program.get_n_rules() == 2

    # Add rule with multiple body atoms
    program.add_rule("father(X,Y) :- parent(X,Y), male(X)")
    assert program.get_n_rules() == 3


def test_magic_transform():
    """Test applying magic set transformation"""
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")

    query = "locIn(belgium, europe)"
    new_program, input_pred_id, output_pred_id = program.apply_magic_transform(query)

    assert new_program.get_n_rules() == 3
    assert input_pred_id != output_pred_id
    assert isinstance(new_program, Program)
