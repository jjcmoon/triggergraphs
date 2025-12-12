API Reference
=============

Module-Level Functions
----------------------

.. function:: set_logging_level(level)

   Set the logging level for the library.

   :param level: Logging level from 0 (TRACE) to 5 (FATAL)
   :type level: int

   Logging levels:
      * 0 - TRACE: Most verbose
      * 1 - DEBUG: Debug information
      * 2 - INFO: Informational messages
      * 3 - WARNING: Warning messages (default)
      * 4 - ERROR: Error messages
      * 5 - FATAL: Fatal errors only

   Example::

      import triggergraphs as tg
      tg.set_logging_level(2)  # Set to INFO level

EDBLayer
--------

.. class:: EDBLayer(confPath="")

   The Extensional Database Layer manages base facts.

   :param confPath: Path to configuration file (optional)
   :type confPath: str

   .. method:: add_csv_source(predname, rows)

      Add an in-memory CSV source for a predicate.

      :param predname: Name of the predicate
      :type predname: str
      :param rows: List of fact tuples, where each tuple is a list of strings
      :type rows: list[list[str]]

      Example::

         edb.add_csv_source("person", [["alice"], ["bob"]])
         edb.add_csv_source("edge", [["a", "b"], ["b", "c"]])

   .. method:: replace_facts_csv_source(predname, rows)

      Replace all facts for an existing CSV source.

      :param predname: Name of the predicate
      :type predname: str
      :param rows: New list of fact tuples
      :type rows: list[list[str]]

   .. method:: add_source(predname, obj)

      Add a custom Python object as a data source.

      :param predname: Name of the predicate
      :type predname: str
      :param obj: Python object implementing the table interface
      :type obj: object

   .. method:: get_term_id(term)

      Get the numerical ID for a term string.

      :param term: The term string
      :type term: str
      :return: The term ID, or None if not found
      :rtype: int or None

   .. method:: get_n_terms()

      Get the total number of unique terms in the database.

      :return: Number of terms
      :rtype: int

   .. method:: get_n_predicates()

      Get the number of predicates in the EDB.

      :return: Number of predicates
      :rtype: int

   .. method:: get_predicates()

      Get a list of all EDB predicate names.

      :return: List of predicate names
      :rtype: list[str]

   .. method:: get_facts(predicate_name)

      Get all facts for a given predicate as term IDs.

      :param predicate_name: Name of the predicate
      :type predicate_name: str
      :return: List of fact tuples (as term IDs)
      :rtype: list[list[int]]

      Example::

         facts = edb.get_facts("person")
         # Returns: [[123], [456]] where 123 and 456 are term IDs

Program
-------

.. class:: Program(edb)

   A Datalog program containing rules.

   :param edb: The EDB layer to use
   :type edb: EDBLayer

   .. method:: add_rule(rule)

      Add a Datalog rule to the program.

      :param rule: The rule in Datalog syntax
      :type rule: str
      :return: The rule ID
      :rtype: int
      :raises RuntimeError: If the rule syntax is invalid

      Example::

         program.add_rule("ancestor(X,Y) :- parent(X,Y)")

   .. method:: load_from_file(path)

      Load rules from a file.

      :param path: Path to the rules file
      :type path: str
      :raises RuntimeError: If file does not exist

   .. method:: get_n_rules()

      Get the number of rules in the program.

      :return: Number of rules
      :rtype: int

   .. method:: get_rule(ruleIdx)

      Get a rule as a string.

      :param ruleIdx: Index of the rule
      :type ruleIdx: int
      :return: The rule string
      :rtype: str

   .. method:: get_predicate_name(predId)

      Get the name of a predicate by its ID.

      :param predId: Predicate ID
      :type predId: int
      :return: Predicate name
      :rtype: str

   .. method:: apply_magic_transform(query)

      Apply magic set transformation for a query.

      :param query: Query literal in Datalog syntax
      :type query: str
      :return: Tuple of (transformed_program, input_pred_id, output_pred_id)
      :rtype: tuple

      Example::

         new_prog, in_id, out_id = program.apply_magic_transform("ancestor(alice,X)")

Reasoner
--------

.. class:: Reasoner(typeChase, edb, program, queryCont=True, edbCheck=True, rewriteCliques=True, tgpath="", typeProv="NOPROV", delProofs=True)

   The reasoning engine that executes Datalog rules.

   :param typeChase: Type of chase algorithm ("tgchase", "tgchase_static", or "probtgchase")
   :type typeChase: str
   :param edb: The EDB layer
   :type edb: EDBLayer
   :param program: The program to execute
   :type program: Program
   :param queryCont: Enable query containment optimization
   :type queryCont: bool
   :param edbCheck: Check EDB during reasoning
   :type edbCheck: bool
   :param rewriteCliques: Enable clique rewriting
   :type rewriteCliques: bool
   :param tgpath: Path to save trigger graph (empty for in-memory)
   :type tgpath: str
   :param typeProv: Provenance type ("NOPROV", "NODEPROV", or "FULLPROV")
   :type typeProv: str
   :param delProofs: Delete proofs (for probabilistic chase)
   :type delProofs: bool

   .. method:: create_model(startStep=0, maxStep=2^64-1)

      Execute reasoning and materialize facts.

      :param startStep: Starting step number
      :type startStep: int
      :param maxStep: Maximum step number
      :type maxStep: int
      :return: Dictionary of statistics
      :rtype: dict

      Returns a dictionary with keys:
         * ``n_nodes``: Number of nodes in the trigger graph
         * ``n_edges``: Number of edges in the trigger graph
         * ``n_triggers``: Number of trigger firings
         * ``n_derivations``: Number of derived facts
         * ``steps``: Number of reasoning steps executed
         * ``max_mem_mb``: Maximum memory used (MB)
         * ``runtime_ms``: Runtime in milliseconds

   .. method:: get_TG()

      Get the trigger graph after reasoning.

      :return: The trigger graph
      :rtype: TG

TG (Trigger Graph)
------------------

.. class:: TG(reasoner)

   Represents the trigger graph structure.

   :param reasoner: The reasoner that created this TG
   :type reasoner: Reasoner

   .. method:: get_n_nodes()

      Get the number of nodes in the trigger graph.

      :return: Number of nodes
      :rtype: int

   .. method:: get_n_edges()

      Get the number of edges in the trigger graph.

      :return: Number of edges
      :rtype: int

   .. method:: get_n_facts()

      Get the total number of facts in the trigger graph.

      :return: Number of facts
      :rtype: int

   .. method:: get_node_size(nodeId)

      Get the number of facts in a specific node.

      :param nodeId: ID of the node
      :type nodeId: int
      :return: Number of facts in the node
      :rtype: int

   .. method:: add_node(predid, step, facts)

      Add a node to the trigger graph with facts.

      :param predid: Predicate ID
      :type predid: int
      :param step: Reasoning step
      :type step: int
      :param facts: List of fact tuples
      :type facts: list[list[str]]

   .. method:: dump_files(path="")

      Export the trigger graph to files.

      :param path: Directory path for output files
      :type path: str

Querier
-------

.. class:: Querier(tg)

   Query interface for extracting facts and provenance from a trigger graph.

   :param tg: The trigger graph to query
   :type tg: TG

   .. method:: get_all_facts()

      Get all derived facts organized by predicate.

      :return: Dictionary mapping predicate names to lists of fact tuples
      :rtype: dict[str, list[list[str]]]

      Example::

         facts = querier.get_all_facts()
         # Returns: {"ancestor": [["alice", "bob"], ["bob", "charlie"]], ...}

   .. method:: get_list_predicates()

      Get a list of all predicates in the trigger graph.

      :return: List of predicate names
      :rtype: list[str]

   .. method:: get_facts_coordinates_with_predicate(predName)

      Get facts and their coordinates (node ID and offset) for a predicate.

      :param predName: Predicate name
      :type predName: str
      :return: List of tuples: (fact_tuple, (node_id, offset))
      :rtype: list

   .. method:: get_node_details_predicate(predName)

      Get detailed node information for a predicate as JSON.

      :param predName: Predicate name
      :type predName: str
      :return: JSON string with node details
      :rtype: str

   .. method:: get_facts_in_TG_node(nodeId)

      Get all facts in a specific trigger graph node as JSON.

      :param nodeId: Node ID
      :type nodeId: int
      :return: JSON string with facts
      :rtype: str

   .. method:: get_derivation_tree(nodeId, factId)

      Get the derivation tree for a fact as JSON.

      :param nodeId: Node ID
      :type nodeId: int
      :param factId: Fact ID within the node
      :type factId: int
      :return: JSON string representing the derivation tree
      :rtype: str

   .. method:: get_derivation_tree_in_TupleSet(tupleSet, factId)

      Get derivation tree for a fact in a TupleSet.

      :param tupleSet: The tuple set
      :type tupleSet: TupleSet
      :param factId: Fact ID
      :type factId: int
      :return: JSON string representing the derivation tree
      :rtype: str

   .. method:: get_leaves(nodeId, factId)

      Get the base facts (leaves) used in a derivation.

      :param nodeId: Node ID
      :type nodeId: int
      :param factId: Fact ID
      :type factId: int
      :return: List of leaf literal sets
      :rtype: list

   .. method:: get_fact_in_TupleSet(tupleSet, factId)

      Get a specific fact from a TupleSet.

      :param tupleSet: The tuple set
      :type tupleSet: TupleSet
      :param factId: Fact ID
      :type factId: int
      :return: Fact as a list of terms
      :rtype: list[str]

   .. method:: get_predicate_name(predicateId)

      Get predicate name from its ID.

      :param predicateId: Predicate ID
      :type predicateId: int
      :return: Predicate name
      :rtype: str

   .. method:: get_term_name(termId)

      Get term string from its ID.

      :param termId: Term ID
      :type termId: int
      :return: Term string
      :rtype: str

TupleSet
--------

.. class:: TupleSet()

   Represents a set of tuples (facts).

   .. method:: get_n_facts()

      Get the number of facts in this tuple set.

      :return: Number of facts
      :rtype: int
