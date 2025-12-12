Examples
========

This page provides complete, runnable examples demonstrating TriggerGraphs usage.

Example 1: Family Relationships
--------------------------------

This example shows transitive closure reasoning for family relationships.

.. code-block:: python

   import triggergraphs as tg

   # Create EDB and add family facts
   edb = tg.EDBLayer()
   edb.add_csv_source("parent", [
       ["alice", "bob"],
       ["bob", "charlie"],
       ["charlie", "dave"],
       ["eve", "frank"]
   ])

   # Create program with transitivity rule
   program = tg.Program(edb)
   program.add_rule("ancestor(X,Y) :- parent(X,Y)")
   program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")

   # Run reasoning
   reasoner = tg.Reasoner("tgchase", edb, program)
   stats = reasoner.create_model()

   print(f"Derived {stats['n_derivations']} facts in {stats['runtime_ms']:.2f}ms")

   # Query results
   querier = tg.Querier(reasoner.get_TG())
   facts = querier.get_all_facts()

   print("\nAncestor relationships:")
   for fact in facts.get("ancestor", []):
       print(f"  {fact[0]} is ancestor of {fact[1]}")

Example 2: Graph Reachability
------------------------------

Computing reachable nodes in a directed graph.

.. code-block:: python

   import triggergraphs as tg

   # Create a directed graph
   edb = tg.EDBLayer()
   edges = [
       ["a", "b"],
       ["b", "c"],
       ["c", "d"],
       ["b", "e"],
       ["e", "f"],
       ["x", "y"]  # Disconnected component
   ]
   edb.add_csv_source("edge", edges)

   # Reachability rules
   program = tg.Program(edb)
   program.add_rule("reachable(X,Y) :- edge(X,Y)")
   program.add_rule("reachable(X,Z) :- edge(X,Y), reachable(Y,Z)")

   # Compute reachability
   reasoner = tg.Reasoner("tgchase", edb, program)
   stats = reasoner.create_model()

   # Query and analyze
   querier = tg.Querier(reasoner.get_TG())
   reachable = querier.get_all_facts()["reachable"]

   # Find all nodes reachable from 'a'
   from_a = [dest for src, dest in reachable if src == "a"]
   print(f"Nodes reachable from 'a': {sorted(set(from_a))}")

Example 3: Using Provenance
----------------------------

Tracking how facts were derived using provenance.

.. code-block:: python

   import triggergraphs as tg
   import json

   # Set up data
   edb = tg.EDBLayer()
   edb.add_csv_source("parent", [
       ["alice", "bob"],
       ["bob", "charlie"]
   ])

   program = tg.Program(edb)
   program.add_rule("ancestor(X,Y) :- parent(X,Y)")
   program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")

   # Enable FULL provenance tracking
   reasoner = tg.Reasoner("tgchase", edb, program, typeProv="FULLPROV")
   reasoner.create_model()

   # Query with provenance
   querier = tg.Querier(reasoner.get_TG())
   facts_coords = querier.get_facts_coordinates_with_predicate("ancestor")

   # Get derivation tree for each fact
   for fact, (node_id, offset) in facts_coords:
       tree_json = querier.get_derivation_tree(node_id, offset)
       tree = json.loads(tree_json)
       print(f"\nFact: ancestor({fact[0]}, {fact[1]})")
       print(f"Derivation tree: {json.dumps(tree, indent=2)}")

Example 4: Same Generation
---------------------------

Computing pairs of people in the same generation using recursion.

.. code-block:: python

   import triggergraphs as tg

   edb = tg.EDBLayer()

   # Family tree
   edb.add_csv_source("parent", [
       # Generation 1 -> 2
       ["grandpa", "alice"],
       ["grandpa", "bob"],
       # Generation 2 -> 3
       ["alice", "charlie"],
       ["alice", "dave"],
       ["bob", "eve"]
   ])

   program = tg.Program(edb)

   # Same generation rules
   # Siblings are in the same generation
   program.add_rule("sameGen(X,Y) :- parent(Z,X), parent(Z,Y), X != Y")
   # Symmetry
   program.add_rule("sameGen(X,Y) :- sameGen(Y,X)")
   # If parents are in same gen, children are too
   program.add_rule("sameGen(X,Y) :- parent(P1,X), parent(P2,Y), sameGen(P1,P2), X != Y")

   reasoner = tg.Reasoner("tgchase", edb, program)
   reasoner.create_model()

   querier = tg.Querier(reasoner.get_TG())
   same_gen = querier.get_all_facts()["sameGen"]

   print("Same generation pairs:")
   seen = set()
   for p1, p2 in same_gen:
       pair = tuple(sorted([p1, p2]))
       if pair not in seen:
           print(f"  {p1} and {p2}")
           seen.add(pair)

Example 5: Magic Set Transformation
------------------------------------

Using magic sets for efficient query answering.

.. code-block:: python

   import triggergraphs as tg

   # Large graph
   edb = tg.EDBLayer()
   edges = [(f"n{i}", f"n{i+1}") for i in range(100)]
   edb.add_csv_source("edge", edges)

   # Original program
   program = tg.Program(edb)
   program.add_rule("path(X,Y) :- edge(X,Y)")
   program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

   # Query: what nodes are reachable from n0?
   query = "path(n0, X)"

   # Apply magic set transformation
   transformed_prog, input_id, output_id = program.apply_magic_transform(query)

   # Add input fact for the query
   edb.add_csv_source(program.get_predicate_name(input_id), [["n0"]])

   # Run with transformed program (more efficient)
   reasoner = tg.Reasoner("tgchase", edb, transformed_prog)
   stats = reasoner.create_model()

   querier = tg.Querier(reasoner.get_TG())
   results = querier.get_all_facts()

   output_pred = program.get_predicate_name(output_id)
   reachable_from_n0 = results.get(output_pred, [])

   print(f"Nodes reachable from n0: {len(reachable_from_n0)} nodes")
   print(f"Reasoning time: {stats['runtime_ms']:.2f}ms")

Example 6: Working with Statistics
-----------------------------------

Monitoring reasoning performance and memory usage.

.. code-block:: python

   import triggergraphs as tg

   # Enable INFO logging
   tg.set_logging_level(2)

   edb = tg.EDBLayer()

   # Create a moderately sized graph
   n = 50
   edges = []
   for i in range(n):
       edges.append([f"n{i}", f"n{(i+1)%n}"])  # Cycle
       if i % 5 == 0:
           edges.append([f"n{i}", f"n{(i+2)%n}"])  # Shortcuts

   edb.add_csv_source("edge", edges)

   program = tg.Program(edb)
   program.add_rule("path(X,Y) :- edge(X,Y)")
   program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

   reasoner = tg.Reasoner("tgchase", edb, program)
   stats = reasoner.create_model()

   # Analyze statistics
   print("Reasoning Statistics:")
   print(f"  Nodes in TG: {stats['n_nodes']}")
   print(f"  Edges in TG: {stats['n_edges']}")
   print(f"  Triggers fired: {stats['n_triggers']}")
   print(f"  Facts derived: {stats['n_derivations']}")
   print(f"  Reasoning steps: {stats['steps']}")
   print(f"  Runtime: {stats['runtime_ms']:.2f} ms")
   print(f"  Peak memory: {stats['max_mem_mb']:.2f} MB")

   tg_graph = reasoner.get_TG()
   print(f"\nTrigger Graph Info:")
   print(f"  Total nodes: {tg_graph.get_n_nodes()}")
   print(f"  Total edges: {tg_graph.get_n_edges()}")
   print(f"  Total facts: {tg_graph.get_n_facts()}")

Example 7: Multi-Predicate Rules
---------------------------------

Using rules that involve multiple predicates.

.. code-block:: python

   import triggergraphs as tg

   edb = tg.EDBLayer()

   # Different types of relationships
   edb.add_csv_source("employee", [
       ["alice", "acme"],
       ["bob", "acme"],
       ["charlie", "techcorp"]
   ])

   edb.add_csv_source("manager", [
       ["alice", "bob"]
   ])

   edb.add_csv_source("friend", [
       ["bob", "charlie"]
   ])

   program = tg.Program(edb)

   # Colleague: works at same company
   program.add_rule(
       "colleague(X,Y) :- employee(X,C), employee(Y,C), X != Y"
   )

   # Can introduce: friend of colleague
   program.add_rule(
       "canIntroduce(X,Y,Z) :- colleague(X,Y), friend(Y,Z)"
   )

   # Work connection: either colleague or manager
   program.add_rule("workConnection(X,Y) :- colleague(X,Y)")
   program.add_rule("workConnection(X,Y) :- manager(X,Y)")
   program.add_rule("workConnection(X,Y) :- manager(Y,X)")

   reasoner = tg.Reasoner("tgchase", edb, program)
   reasoner.create_model()

   querier = tg.Querier(reasoner.get_TG())
   facts = querier.get_all_facts()

   print("Work connections:")
   for x, y in facts.get("workConnection", []):
       print(f"  {x} - {y}")

   print("\nPossible introductions:")
   for x, y, z in facts.get("canIntroduce", []):
       print(f"  {x} can introduce {y} to {z}")

Example 8: Incremental Updates
-------------------------------

Replacing facts and re-running reasoning.

.. code-block:: python

   import triggergraphs as tg

   edb = tg.EDBLayer()

   # Initial graph
   edb.add_csv_source("edge", [
       ["a", "b"],
       ["b", "c"]
   ])

   program = tg.Program(edb)
   program.add_rule("path(X,Y) :- edge(X,Y)")
   program.add_rule("path(X,Z) :- edge(X,Y), path(Y,Z)")

   # First reasoning
   reasoner = tg.Reasoner("tgchase", edb, program)
   reasoner.create_model()

   querier = tg.Querier(reasoner.get_TG())
   facts1 = querier.get_all_facts()["path"]
   print(f"Initial paths: {len(facts1)}")

   # Update the graph
   edb.replace_facts_csv_source("edge", [
       ["a", "b"],
       ["b", "c"],
       ["c", "d"],
       ["d", "e"]
   ])

   # Create new reasoner with updated data
   reasoner2 = tg.Reasoner("tgchase", edb, program)
   reasoner2.create_model()

   querier2 = tg.Querier(reasoner2.get_TG())
   facts2 = querier2.get_all_facts()["path"]
   print(f"Updated paths: {len(facts2)}")

   new_paths = set(map(tuple, facts2)) - set(map(tuple, facts1))
   print(f"New paths discovered: {len(new_paths)}")

Example 9: Exporting Results
-----------------------------

Exporting the trigger graph and results to files.

.. code-block:: python

   import triggergraphs as tg
   import os

   edb = tg.EDBLayer()
   edb.add_csv_source("parent", [
       ["alice", "bob"],
       ["bob", "charlie"]
   ])

   program = tg.Program(edb)
   program.add_rule("ancestor(X,Y) :- parent(X,Y)")
   program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")

   reasoner = tg.Reasoner("tgchase", edb, program)
   reasoner.create_model()

   # Export trigger graph
   tg_graph = reasoner.get_TG()
   output_dir = "tg_output"
   os.makedirs(output_dir, exist_ok=True)
   tg_graph.dump_files(output_dir)

   print(f"Trigger graph exported to {output_dir}/")
   print(f"Files created: {os.listdir(output_dir)}")

   # Also export facts to a custom format
   querier = tg.Querier(tg_graph)
   all_facts = querier.get_all_facts()

   with open(f"{output_dir}/facts.txt", "w") as f:
       for pred, facts in all_facts.items():
           for fact in facts:
               f.write(f"{pred}({', '.join(fact)})\n")

   print("Facts exported to facts.txt")
