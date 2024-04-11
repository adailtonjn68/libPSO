# libPSO

## Description

libPSO is library in C designed to provide an easy to use API to run Particle
Swarm Optimization (PSO). Also, it was created with the new C23 standard in mind.


## Installation 

To install this library in Linux, download this repository "cd" into it,
and run the following
```bash
sudo make install
```

By default, the installation path is "/usr/local/lib/" for .so file and 
"/usr/local/include/" for the .h file.
If another directory is desired, use
```
sudo make INSTALL_DIR=your_desired_directory install
```

## Examples

There are examples on how to use this library in "Examples/" directory.
To compile them, run
```
make
```

Note: this won't compile the shared object.

## Contact

This project was created by Adailton Braga.  
If you have any question, get in touch by e-mail: adailtonjn68@gmail.com
