from triggergraphs import EDBLayer, Program, Reasoner


def test_basic_reasoning():
    """Test basic reasoning, statistics, and TG retrieval"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"], ["b", "c"], ["c", "d"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")
    program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

    reasoner = Reasoner("tgchase", edb, program)
    stats = reasoner.create_model()

    # Verify complete statistics
    expected_keys = ["n_nodes", "n_edges", "n_triggers", "n_derivations", "steps", "max_mem_mb", "runtime_ms"]
    for key in expected_keys:
        assert key in stats
    assert stats["n_nodes"] > 0
    assert stats["steps"] >= 1

    # Verify TG retrieval
    tg = reasoner.get_TG()
    assert tg is not None


def test_reasoner_with_config():
    """Test reasoner with config file data"""
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")

    reasoner = Reasoner("tgchase", edb, program)
    stats = reasoner.create_model()

    assert stats["n_nodes"] > 0
    assert stats["steps"] > 0


def test_provenance_types():
    """Test different provenance tracking modes"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")

    # NOPROV
    reasoner1 = Reasoner("tgchase", edb, program, typeProv="NOPROV")
    assert reasoner1.create_model()["n_nodes"] > 0

    # NODEPROV
    reasoner2 = Reasoner("tgchase", edb, program, typeProv="NODEPROV")
    assert reasoner2.create_model()["n_nodes"] > 0

    # FULLPROV
    reasoner3 = Reasoner("tgchase", edb, program, typeProv="FULLPROV", delProofs=False)
    assert reasoner3.create_model()["n_nodes"] > 0


def test_reasoner_optional_parameters():
    """Test reasoner with optional parameters"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")

    # Test various parameter combinations
    for qc, ec, rc in [(False, True, True), (True, False, True), (True, True, False)]:
        reasoner = Reasoner("tgchase", edb, program, queryCont=qc, edbCheck=ec, rewriteCliques=rc)
        assert reasoner.create_model()["n_nodes"] > 0


def test_reasoner_step_limits():
    """Test reasoning with step limits"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"], ["b", "c"], ["c", "d"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")
    program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

    reasoner = Reasoner("tgchase", edb, program)
    stats = reasoner.create_model(startStep=0, maxStep=1)

    assert stats["steps"] <= 1
