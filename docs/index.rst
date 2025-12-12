TriggerGraphs Documentation
===========================

**TriggerGraphs** provides Python bindings to the GLog, a high-performance 
Datalog engine with support for provenance tracking.

Papers
------

The trigger graphs architecture is described in:

- `Materializing knowledge bases via trigger graphs <https://dl.acm.org/doi/abs/10.14778/3447689.3447699>`_
- `Probabilistic Reasoning at Scale: Trigger Graphs to the Rescue <https://dl.acm.org/doi/abs/10.1145/3588719>`_


Quick Example
-------------

.. code-block:: python

   import triggergraphs as tg

   # Create an extensional database
   edb = tg.EDBLayer()

   # Add facts
   edb.add_csv_source("parent", [
       ["alice", "bob"],
       ["bob", "charlie"]
   ])

   # Create a program with rules
   program = tg.Program(edb)
   program.add_rule("ancestor(X,Y) :- parent(X,Y)")
   program.add_rule("ancestor(X,Z) :- parent(X,Y), ancestor(Y,Z)")

   # Run reasoning
   reasoner = tg.Reasoner("tgchase", edb, program)
   stats = reasoner.create_model()

   # Query results
   tg_graph = reasoner.get_TG()
   querier = tg.Querier(tg_graph)
   facts = querier.get_all_facts()


Contents
--------

.. toctree::
   :maxdepth: 2

   Home <self>
   getting_started
   user_guide
   api
   examples