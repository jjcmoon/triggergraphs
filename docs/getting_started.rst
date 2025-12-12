Getting Started
===============

This guide will help you get started with TriggerGraphs.

Installation
------------

Install TriggerGraphs using pip:

.. code-block:: bash

   pip install triggergraphs

Or install from source:

.. code-block:: bash

   git clone https://github.com/jjcmoon/triggergraphs
   cd triggergraphs
   pip install .

Basic Concepts
--------------

TriggerGraphs is built around several key concepts:

**Extensional Database (EDB)**
   The set of base facts in your knowledge base. These are the ground truths from which new facts are derived.

**Intensional Database (IDB)**
   The set of Datalog rules that define how to derive new facts from existing ones.

**Program**
   A collection of Datalog rules that make up your logic program.

**Reasoner**
   The engine that executes the reasoning process using forward chaining.

**Trigger Graph (TG)**
   The internal data structure that represents how facts were derived during reasoning.

**Querier**
   An interface for querying facts and derivation information from a trigger graph.

Your First Program
------------------

Let's create a simple program that reasons about family relationships:

.. code-block:: python

   import triggergraphs as tg

   # Step 1: Create an EDB layer
   edb = tg.EDBLayer()

   # Step 2: Add some base facts
   edb.add_csv_source("parent", [
       ["alice", "bob"],
       ["bob", "charlie"],
       ["charlie", "dave"]
   ])

   # Step 3: Create a program
   program = tg.Program(edb)

   # Step 4: Add rules
   program.add_rule("ancestor(X,Y) :- parent(X,Y)")
   program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")

   # Step 5: Create a reasoner and run it
   reasoner = tg.Reasoner("tgchase", edb, program)
   stats = reasoner.create_model()

   print(f"Reasoning completed in {stats['runtime_ms']:.2f}ms")
   print(f"Derived {stats['n_derivations']} facts")

   # Step 6: Query the results
   tg_graph = reasoner.get_TG()
   querier = tg.Querier(tg_graph)

   # Get all ancestor facts
   all_facts = querier.get_all_facts()
   print("\nAncestor facts:")
   for fact in all_facts.get("ancestor", []):
       print(f"  {fact}")

Understanding the Workflow
---------------------------

The typical workflow in TriggerGraphs follows these steps:

1. **Create EDBLayer**: Initialize the extensional database
2. **Add Facts**: Populate the EDB with base facts
3. **Create Program**: Initialize a program with the EDB
4. **Add Rules**: Define Datalog rules for reasoning
5. **Create Reasoner**: Set up the reasoning engine
6. **Execute Reasoning**: Call ``create_model()`` to materialize facts
7. **Query Results**: Use the Querier to extract derived facts

Datalog Syntax
--------------

Rules in TriggerGraphs follow standard Datalog syntax:

.. code-block:: prolog

   head(X,Y) :- body1(X,Z), body2(Z,Y)

Key syntax elements:

- **Variables**: Start with uppercase letters (``X``, ``Y``, ``Person``)
- **Constants**: Lowercase strings or quoted strings (``alice``, ``'Bob'``)
- **Predicates**: Relation names (``parent``, ``ancestor``)
- **Rules**: ``head :- body1, body2, ...``
- **Facts**: Rules with empty body (``parent(alice, bob)``)

Examples of valid rules:

.. code-block:: prolog

   # Transitivity
   reachable(X,Y) :- edge(X,Y)
   reachable(X,Z) :- edge(X,Y), reachable(Y,Z)

   # Negation (not all chase types support this)
   bachelor(X) :- person(X), ~married(X)

   # Multiple atoms in head is not supported in standard Datalog

Next Steps
----------

- Read the :doc:`user_guide` for detailed usage patterns
- Explore the :doc:`api` for complete API reference
- Check out :doc:`examples` for more complex use cases
