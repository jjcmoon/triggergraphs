import tempfile
import os
from triggergraphs import EDBLayer, Program, Reasoner, TG


def test_tg_methods():
    """Test all TG methods comprehensively"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"], ["b", "c"], ["c", "d"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")
    program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

    reasoner = Reasoner("tgchase", edb, program)
    reasoner.create_model()

    tg = reasoner.get_TG()
    assert isinstance(tg, TG)

    # Test node/edge/fact counts
    n_nodes = tg.get_n_nodes()
    n_edges = tg.get_n_edges()
    n_facts = tg.get_n_facts()

    assert isinstance(n_nodes, int) and n_nodes > 0
    assert isinstance(n_edges, int) and n_edges >= 0
    assert isinstance(n_facts, int) and n_facts > 0

    # Test node sizes
    for node_id in range(n_nodes):
        size = tg.get_node_size(node_id)
        assert isinstance(size, int) and size >= 0


def test_tg_dump_files():
    """Test dumping TG to files"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")

    reasoner = Reasoner("tgchase", edb, program)
    reasoner.create_model()

    tg = reasoner.get_TG()

    with tempfile.TemporaryDirectory() as tmpdir:
        tg.dump_files(tmpdir)
        assert len(os.listdir(tmpdir)) > 0


def test_tg_with_config():
    """Test TG with config file data"""
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")

    reasoner = Reasoner("tgchase", edb, program)
    reasoner.create_model()

    tg = reasoner.get_TG()

    assert tg.get_n_nodes() > 0
    assert tg.get_n_facts() > 0
