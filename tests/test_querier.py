import json
from triggergraphs import EDBLayer, Program, Reasoner, Querier


def test_querier_methods():
    """Test all major querier methods"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"], ["b", "c"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")

    reasoner = Reasoner("tgchase", edb, program, typeProv="NODEPROV")
    reasoner.create_model()

    tg = reasoner.get_TG()
    querier = Querier(tg)

    # Test get_all_facts
    all_facts = querier.get_all_facts()
    assert isinstance(all_facts, dict)
    assert "path" in all_facts
    assert len(all_facts["path"]) > 0

    # Test get_list_predicates
    predicates = querier.get_list_predicates()
    assert isinstance(predicates, list)
    assert "path" in predicates

    # Test get_facts_coordinates_with_predicate
    coords = querier.get_facts_coordinates_with_predicate("path")
    assert isinstance(coords, list)

    # Test get_node_details_predicate
    details = json.loads(querier.get_node_details_predicate("path"))
    assert isinstance(details, list)

    if len(details) > 0:
        node_id = int(details[0]['id'])

        # Test get_facts_in_TG_node
        facts = json.loads(querier.get_facts_in_TG_node(node_id))
        assert isinstance(facts, list)


def test_querier_with_config():
    """Test querier with config file data"""
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")

    reasoner = Reasoner("tgchase", edb, program, typeProv="NODEPROV")
    reasoner.create_model()

    tg = reasoner.get_TG()
    querier = Querier(tg)

    nodes = json.loads(querier.get_node_details_predicate("neigh"))
    answers = set()
    for node in nodes:
        facts = json.loads(querier.get_facts_in_TG_node(int(node['id'])))
        answers.update(map(tuple, facts))

    expected = {("belgium", "france"), ("france", "germany"), ("belgium", "germany")}
    expected = expected | {(x, y) for (y, x) in expected}
    assert answers == expected


def test_querier_term_and_predicate_names():
    """Test term and predicate name lookups"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["alice", "bob"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")

    reasoner = Reasoner("tgchase", edb, program)
    reasoner.create_model()

    tg = reasoner.get_TG()
    querier = Querier(tg)

    # Test term name lookup
    alice_id = edb.get_term_id("alice")
    bob_id = edb.get_term_id("bob")
    assert querier.get_term_name(alice_id) == "alice"
    assert querier.get_term_name(bob_id) == "bob"

    # Test predicate name lookup
    predicates = querier.get_list_predicates()
    for i in range(len(predicates)):
        name = querier.get_predicate_name(i)
        assert isinstance(name, str)


def test_querier_provenance():
    """Test provenance-related querier methods"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["a", "b"], ["b", "c"]])

    program = Program(edb)
    program.add_rule("path(X,Y) :- edge(X,Y)")
    program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

    reasoner = Reasoner("tgchase", edb, program, typeProv="FULLPROV", delProofs=False)
    reasoner.create_model()

    tg = reasoner.get_TG()
    querier = Querier(tg)

    details = json.loads(querier.get_node_details_predicate("path"))
    if len(details) > 0:
        node_id = int(details[0]['id'])

        # Test get_derivation_tree
        tree = querier.get_derivation_tree(node_id, 0)
        assert isinstance(tree, str)

        # Test get_leaves
        leaves = querier.get_leaves(node_id, 0)
        assert isinstance(leaves, list)
