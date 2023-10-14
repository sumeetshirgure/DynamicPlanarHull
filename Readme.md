[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.8396184.svg)](https://doi.org/10.5281/zenodo.8396184)


# Dynamic planar convex hull.
This repository contains header libraries implementing real time dynamic planar convex hull algorithms.

## Online setting
In the online setting, we have a stream of point insertion queries, and the objective is to quickly rebuild
the hull and discard any points trapped in the interior of the new hull.

This implementation is optimal in the sense that all queries take logarithmic time.
It was part of a term project in Computational Geometry, IIT Kharagpur Spring 2020.

An interface exists to answer point location queries, extremal direction queries, and tangent queries.
All these operations are performed in expected O(log(n)) time.
The expected time complexity comes from the fact that a randomised binary search tree ("treap") has been used for simplicity of implementation.
But in real life it's practically logarithmic as noted in the performance testing IPython notebook.

This implementation uses an idea presented in [1] of storing enough information in the tree node itself to guide the binary search,
but simplifies some of the case analysis by maintaining two sequences (and hence trees) corresponding to the "lower hull" and the
"upper hull" of the set of points inserted into the hull. See implementation reference (part of our term paper) attached in notes/online for details.
[1] F. P. Preparata. 1979. An optimal real-time algorithm for planar convex hulls. Commun. ACM 22, 7 (July 1979), 402–405. DOI:https://doi.org/10.1145/359131.359132

## Fully dynamic setting.
In the fully dynamic setting, we have a stream of point insertion and deletion queries, and the objective is to quickly rebuild
the hull without discarding any of the points in the interior.
Notably there exist real time algorithms with logarithmic complexity, but they tend to have bad constants in practice.

This implementation uses ideas presented in the book [2] and a blog post in [3] and answers queries in a time
proportional to the square of the logarithm of the total number of points, regardless of the size of the hull.

[2] "Computational Geometry - An Introduction." Franco P. Preparata, Michael Ian Shamos (1985) DOI:https://doi.org/10.1007/978-1-4612-1098-6

[3] https://sumeetshirgure.github.io

# Installation and requirements
This is a header only library, and is intended to be used as is.
There is no need to install headers to a specific location, but the Makefile contains a `make install` target to create a directory
called `dpch` in /usr/include where the header files can be stored.
Note that installing in /usr/include might require super user privileges.

To make sure your compiler finds the header files during usage, add a -I flag followed by wherever you have stored the header library.
Also note that most systems have their compiler search inside /usr/include, so if that's where the headers are situated the -I flag shouldn't be necessary.

Please refer to the makefile for more examples on compiler flags. The compiler must be C++20 compliant for the templates in this library to work.
Please see the documentation and the test driver code for more details on the API.

To build the user manual, go into the doc subdirectory and run `make pdf`. You should have pdflatex installed.
