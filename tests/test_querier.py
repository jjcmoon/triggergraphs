import json

from triggergraphs import Program, EDBLayer, Reasoner, Querier


def test_basic():
    edb = EDBLayer("tests/data1/edb.conf")
    program = Program(edb)
    program.load_from_file("tests/data1/program.dlog")

    reasoner = Reasoner("tgchase", edb, program, typeProv="NODEPROV", edbCheck=False, queryCont=False)
    tg = reasoner.get_TG()
    reasoner.create_model(0)

    querier = Querier(tg)
    nodes = querier.get_node_details_predicate("neigh")
    nodes = json.loads(nodes)

    # Collect all answers to the query
    answers = set()
    for node in nodes:
        facts = querier.get_facts_in_TG_node(int(node['id']))
        print("facts", facts)
        answers.update(map(tuple, json.loads(facts)))
    
    expected = {("belgium", "france"), ("france", "germany"), ("belgium", "germany")}
    expected = expected | {(x, y) for (y, x) in expected}
    assert answers == expected
    assert False
