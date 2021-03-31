---
title: Hopfield Network Simulator
---

# Table of Contents

1.  [Introduction](#orgfdd7153)
2.  [Usage](#org180da05)
    1.  [Example usage](#orgaa31fd5)
    2.  [Argument breakdown](#org22cf74d)



<a id="orgfdd7153"></a>

# Introduction

The library provides basic algorithms and data structures to perform simulations of spiking-neuron Hopfield Networks. It deals with the most elementary case of binary outputs.

It was originally developed for the MSc dissertation:

*Lorenzo Mella, "High-performance Hopfield Network Simulator for Memory Capacity Analysis", [the University of Edinburgh - School of Informatics](https://www.ed.ac.uk/informatics)*.


<a id="org180da05"></a>

# Usage


<a id="orgaa31fd5"></a>

## Example usage

For a demonstration of how the library may be used, inspect the script (actually, C source code) `hn_basic_simulation/hn_basic_simulation.c` and run the executable `hn_basic_simulation/hn_basic_simulation`.


<a id="org22cf74d"></a>

## Argument breakdown

**Example**: `hn_basic_simulation -N 1000 -M 500 -w weights.bin -p patterns.bin -s output.bin -m MODE_RANDOM -t 0.25`

-   `-w` sets the binary file with the list of the initial synaptic weights of the units (i.e., neurons) composing Network.
-   `-N` specifies the number of the units, to read the file correctly.
-   `-p` sets the binary file with the list of pattens to be applied to the Network to attempt memorisation.
-   `-M` specifies the number of provided patterns, to read the file correctly.
-   `-s` sets the filename for the binary field including the simulation results. If the file already exists, it will be overwritten.
-   `-m` the "mode" of selection of the next neuron to update: they can either be cyclically updated according to their index, or selected at random (loose uniform distribution, with the local implementation of `rand()`).
-   `-t` sets the error rate ("threshold") tolerated in comparing any memorised pattern with the provided original.

The options `--help` (`-h`) and `--version` (`-v`) are also available.

