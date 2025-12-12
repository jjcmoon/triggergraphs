User Guide
==========

This guide provides detailed information on using TriggerGraphs effectively.

Working with EDBLayer
---------------------

The ``EDBLayer`` is the foundation of your knowledge base. It manages the extensional database (base facts).

Creating an EDBLayer
~~~~~~~~~~~~~~~~~~~~

You can create an EDBLayer with or without a configuration file:

.. code-block:: python

   import triggergraphs as tg

   # Empty EDBLayer
   edb = tg.EDBLayer()

   # With configuration file
   edb = tg.EDBLayer("edb.conf")

Adding Data Sources
~~~~~~~~~~~~~~~~~~~

**CSV Sources (In-Memory)**

The simplest way to add facts is using CSV data:

.. code-block:: python

   edb.add_csv_source("person", [
       ["alice"],
       ["bob"],
       ["charlie"]
   ])

   edb.add_csv_source("friend", [
       ["alice", "bob"],
       ["bob", "charlie"]
   ])

**Replacing Facts**

You can replace the facts in an existing predicate:

.. code-block:: python

   edb.replace_facts_csv_source("person", [
       ["alice"],
       ["dave"]
   ])

**Custom Python Sources**

For more complex scenarios, you can implement custom data sources:

.. code-block:: python

   # obj should implement the table interface
   edb.add_source("predicate_name", obj)

Querying the EDBLayer
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Get all predicates
   predicates = edb.get_predicates()

   # Get number of predicates
   n_preds = edb.get_n_predicates()

   # Get facts for a specific predicate
   facts = edb.get_facts("person")
   # Returns: [[term_id_1], [term_id_2], ...]

   # Get term ID for a string
   term_id = edb.get_term_id("alice")

   # Get number of unique terms
   n_terms = edb.get_n_terms()

Working with Programs
---------------------

A ``Program`` contains the Datalog rules that define your reasoning logic.

Creating and Populating Programs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Create a program
   program = tg.Program(edb)

   # Add rules one at a time
   rule_id = program.add_rule("ancestor(X,Y) :- parent(X,Y)")

   # Load rules from a file
   program.load_from_file("rules.dl")

Inspecting Programs
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Get number of rules
   n_rules = program.get_n_rules()

   # Get a specific rule
   rule_str = program.get_rule(0)  # First rule

   # Get predicate name by ID
   pred_name = program.get_predicate_name(pred_id)

Magic Set Transformation
~~~~~~~~~~~~~~~~~~~~~~~~

For more efficient query answering, you can apply magic set transformation:

.. code-block:: python

   # Transform program for a specific query
   new_program, input_pred_id, output_pred_id = program.apply_magic_transform("ancestor(alice,X)")

   # Use the transformed program for reasoning
   reasoner = tg.Reasoner("tgchase", edb, new_program)

Working with Reasoners
----------------------

The ``Reasoner`` executes the reasoning process.

Chase Algorithms
~~~~~~~~~~~~~~~~

TriggerGraphs supports several chase algorithm variants:

.. code-block:: python

   # Basic trigger graph chase (no provenance)
   reasoner = tg.Reasoner("tgchase", edb, program, typeProv="NOPROV")

   # With node-level provenance
   reasoner = tg.Reasoner("tgchase", edb, program, typeProv="NODEPROV")

   # With full provenance tracking
   reasoner = tg.Reasoner("tgchase", edb, program, typeProv="FULLPROV")

   # Static trigger graph chase
   reasoner = tg.Reasoner("tgchase_static", edb, program)

   # Probabilistic trigger graph chase
   reasoner = tg.Reasoner("probtgchase", edb, program)

Configuration Options
~~~~~~~~~~~~~~~~~~~~~

The ``Reasoner`` constructor accepts several configuration parameters:

.. code-block:: python

   reasoner = tg.Reasoner(
       typeChase="tgchase",        # Chase algorithm type
       edb=edb,                     # EDB layer
       program=program,             # Program
       queryCont=True,              # Query containment optimization
       edbCheck=True,               # Check EDB during reasoning
       rewriteCliques=True,         # Rewrite cliques
       tgpath="",                   # Path to save TG to disk
       typeProv="NODEPROV",         # Provenance type
       delProofs=True               # Delete proofs (for prob chase)
   )

Running the Reasoner
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Run reasoning from start
   stats = reasoner.create_model()

   # Run from a specific step
   stats = reasoner.create_model(startStep=5)

   # Run with a maximum number of steps
   stats = reasoner.create_model(maxStep=100)

   # Inspect statistics
   print(f"Nodes: {stats['n_nodes']}")
   print(f"Edges: {stats['n_edges']}")
   print(f"Triggers: {stats['n_triggers']}")
   print(f"Derivations: {stats['n_derivations']}")
   print(f"Steps: {stats['steps']}")
   print(f"Runtime (ms): {stats['runtime_ms']}")
   print(f"Max memory (MB): {stats['max_mem_mb']}")

Working with Trigger Graphs
----------------------------

After reasoning, you can access the ``TG`` (Trigger Graph) to inspect the reasoning process.

Accessing the TG
~~~~~~~~~~~~~~~~

.. code-block:: python

   tg_graph = reasoner.get_TG()

Inspecting the TG
~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Get graph statistics
   n_nodes = tg_graph.get_n_nodes()
   n_edges = tg_graph.get_n_edges()
   n_facts = tg_graph.get_n_facts()

   # Get size of a specific node
   node_size = tg_graph.get_node_size(node_id)

Exporting the TG
~~~~~~~~~~~~~~~~

.. code-block:: python

   # Export to files
   tg_graph.dump_files("output_directory")

Querying Results
----------------

The ``Querier`` provides methods to extract facts and derivation information.

Creating a Querier
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   querier = tg.Querier(tg_graph)

Getting All Facts
~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Get all facts as a dictionary
   all_facts = querier.get_all_facts()
   # Returns: {"predicate_name": [[term1, term2, ...], ...], ...}

   for pred_name, facts in all_facts.items():
       print(f"{pred_name}:")
       for fact in facts:
           print(f"  {fact}")

Querying Specific Predicates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # Get list of all predicates in the TG
   predicates = querier.get_list_predicates()

   # Get facts with coordinates for a predicate
   facts_coords = querier.get_facts_coordinates_with_predicate("ancestor")
   # Returns list of ((fact_tuple, (node_id, offset))

   for (fact, (node_id, offset)) in facts_coords:
       print(f"Fact {fact} at node {node_id}, offset {offset}")

Provenance Queries
~~~~~~~~~~~~~~~~~~

If you used provenance tracking, you can query derivation trees:

.. code-block:: python

   # Get derivation tree for a fact
   tree_json = querier.get_derivation_tree(node_id, fact_id)

   # Get leaves (base facts) used in a derivation
   leaves = querier.get_leaves(node_id, fact_id)

   # Get node details for a predicate
   node_details = querier.get_node_details_predicate("ancestor")

Working with TupleSets
~~~~~~~~~~~~~~~~~~~~~~

Some query operations return ``TupleSet`` objects:

.. code-block:: python

   # Get number of facts in a tuple set
   n_facts = tuple_set.get_n_facts()

   # Get a specific fact from a tuple set
   fact = querier.get_fact_in_TupleSet(tuple_set, fact_id)

   # Get derivation tree for a fact in a tuple set
   tree = querier.get_derivation_tree_in_TupleSet(tuple_set, fact_id)

ID Translation
~~~~~~~~~~~~~~

Facts are stored internally using numeric IDs. The Querier can translate them:

.. code-block:: python

   # Get predicate name from ID
   pred_name = querier.get_predicate_name(pred_id)

   # Get term name from ID
   term_name = querier.get_term_name(term_id)

Logging Configuration
---------------------

Control logging verbosity:

.. code-block:: python

   import triggergraphs as tg

   # Set logging level
   # 0 = TRACE, 1 = DEBUG, 2 = INFO, 3 = WARNING, 4 = ERROR, 5 = FATAL
   tg.set_logging_level(2)  # INFO level (default: WARNING)

Best Practices
--------------

**Memory Management**

- For large datasets, consider using file-based storage instead of in-memory
- Use appropriate chase algorithms (``tgchase_static`` for smaller graphs)
- Monitor ``max_mem_mb`` in reasoning statistics

**Performance**

- Apply magic set transformation for specific queries
- Use ``queryCont=True`` for query containment optimization
- Disable unnecessary features (e.g., provenance) if not needed

**Debugging**

- Start with ``set_logging_level(1)`` to see detailed logs
- Inspect ``stats`` returned by ``create_model()`` to understand performance
- Use ``get_rule()`` to verify rules were parsed correctly

**Rule Design**

- Avoid Cartesian products in rules (ensure join conditions)
- Consider rule ordering for readability
- Test with small datasets first
