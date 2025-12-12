from triggergraphs import EDBLayer


def test_config_file():
    """Test loading EDB from config file and retrieving data"""
    edb = EDBLayer("tests/data1/edb.conf")

    # Check predicates
    assert edb.get_n_predicates() == 2
    assert set(edb.get_predicates()) == {"locatedIn", "neighborOf"}

    # Check terms
    assert edb.get_n_terms() == 4
    for term in ["belgium", "france", "germany", "europe"]:
        assert edb.get_term_id(term) is not None

    # Check facts
    for fact in edb.get_facts("locatedIn"):
        assert isinstance(fact, list)
        assert len(fact) == 2
        assert all(isinstance(term_id, int) for term_id in fact)

    assert len(edb.get_facts("neighborOf")) == 3


def test_empty_edb():
    """Test creating an empty EDBLayer"""
    edb = EDBLayer()
    assert edb.get_n_predicates() == 0
    assert edb.get_predicates() == []
    assert edb.get_n_terms() == 0


def test_add_csv_source():
    """Test adding and replacing CSV data programmatically"""
    edb = EDBLayer()

    # Add data
    parent_data = [["alice", "bob"], ["bob", "charlie"]]
    edb.add_csv_source("parent", parent_data)

    assert edb.get_n_predicates() == 1
    assert edb.get_predicates() == ["parent"]
    assert len(edb.get_facts("parent")) == 2

    # Add more predicates
    edb.add_csv_source("sibling", [["alice", "david"]])
    assert edb.get_n_predicates() == 2

    # Replace facts
    new_data = [["eve", "frank"], ["frank", "grace"]]
    edb.replace_facts_csv_source("parent", new_data)
    assert len(edb.get_facts("parent")) == 2


def test_get_term_id():
    """Test term ID lookup"""
    edb = EDBLayer()
    edb.add_csv_source("edge", [["alice", "bob"], ["bob", "charlie"]])

    alice_id = edb.get_term_id("alice")
    bob_id = edb.get_term_id("bob")

    assert alice_id is not None
    assert bob_id is not None
    assert alice_id != bob_id
    assert edb.get_term_id("nonexistent") is None

