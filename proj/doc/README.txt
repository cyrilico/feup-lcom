The Doxyfile that generates caller graphs requires the EXTRACT_ALL flag set to YES, which creates 
documentation for all files and members - even ones that don't have any comments.
Because of that, we created two Doxyfiles
	1. Doxyfile: creates the 'useful' documentation, without caller graphs and only properly commented members and files are added 
	2. DoxyfileGraph: creates the documentation with caller graphs, to prove that the caller graph in the report is in fact ours (requires
	Graphviz)

Both Doxyfiles work automatically and output to /nographs and /withgraphs, respectively
However, you can still easily edit both the input and output paths as we've moved the corresponding flags to the top
