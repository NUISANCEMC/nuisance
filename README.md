# NUISANCE

For more information, such as tutorials, use cases, references etc, see the [NUISANCE web page](https://nuisance.hepforge.org).
You can get in touch with us on [our slack](https://nuisance-xsec.slack.com).

Core authors (alphabetical):<br/>
  [Luke Pickering](mailto:luke.pickering@stfc.ac.uk) (STFC, RAL) <br/>
  [Patrick Stowell](mailto:p.stowell@sheffield.ac.uk) (Sheffield) <br/>
  [Callum Wilkinson](mailto:cwilkinson@lbl.gov) (Lawrence Berkeley National Laboratory) <br/>
  [Clarence Wret](mailto:clarence.wret@imperial.ac.uk) (Imperial College)

Previous core authors: Ryan Terri (Queen Mary, University of London)

### Contributing
We welcome contributions from across the field, for instance adding new measurements made by experiments, extending generator use cases, adding new variables to the flat-trees, and so on.

Just branch or fork the repository, make your contributions there, and submit a pull request on this main repository, and we'll take a look. 

If you have any questions, or we're slow with responding to your PR, feel free to ping us on [our slack](https://nuisance-xsec.slack.com).

Thanks for your contribution!

### Compilation

The following instructions should be used to build the fitter after checking out.

1. Make sure environmental variables required for the generators you wish to build against are set.
2. In the top nuisance directory make a new build directory:

```
$ mkdir build && cd build
```

3. Configure the build with CMake:
```
$ cmake [-DEXPERIMENTS_ENABLED=ON -DT2K_ENABLED=ON ...] [-DNEUT_ENABLED=ON ...] ../
```

All arguments are optional. The build will search for configured generators and reweight libraries and enabled them if found. The experiment libraries (containing the publish data comparisons) take a few minutes to build and so can be disabled for convenience with `-DEXPERIMENTS_ENABLED=OFF`. Individual experimental libraries can then be re-enabled with e.g. `-DMINERvA_ENABLED=ON`. Generators or reweight libraries that are explicitly enabled are considered requirements and the configuring process will fail if they are not successfully set up. Explicitly disabling a dependency with e.g. `-DNOvARwgt_ENABLED=OFF` will disable the attempt to configure the given dependency.

List of optional dependencies: NEUT, NuWro, GENIE (v2 or v3), T2KReWeight, NOvARwgt, nusystematics

4. Build and install
```
$ make install
```

5. Source the setup script
```
$ source Linux/setup.sh
```

If you prefer, most configure variables can be entered through a cmake UI, such as
ccmake. e.g. "$ mkdir build && cd build && ccmake ../"

#### `nusystematics`

NUISANCE will build `nusystematics` for you if you configure with `-Dnusystematics_BUILTIN=ON`, this implies `-Dnusystematics_ENABLED=ON`.

### Adding Classes
    The fitter is designed to be easily extended by adding new measurement classes whilst keeping the input convertors and tuning functionality the same.
    The Devel module folder is setup with some examples of how to add new classes into the framework. Feel free to email me if there are difficulties adding new measurements.

### Running Fits
    Whilst running fits is relatively quick and simple, there are now a large range of possible options. Doxygen Documentation is being added to the $NUISANCE/doc/html folder.
    Refer thre for guidance on how to properly formulate a card file.

