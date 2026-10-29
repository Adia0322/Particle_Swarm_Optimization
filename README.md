# Particle-Swarm-Optimization
```
+------------------------------------------------------------------+
|     Version     |                     Content                    |
+------------------------------------------------------------------+
|  V1.00.20260801 |  initial release for PSO                       |
+------------------------------------------------------------------+
```
## Introduction
Particle Swarm Optimization (PSO) is a population-based optimization algorithm inspired by swarm intelligence, which searches for optimal solutions through iterative particle updates.  

### Features  
* Hybrid PSO: C core with Python iInterface  
* Cost function: ZNCC (Zero-mean Normalized Cross-Correlation) based image matching for position evaluation
> This project demonstrates target point displacement estimation using a high-efficiency and accurate PSO algorithm.

## Example
Estimate target point displacement between reference and deformed images.
```
Target point(x,y) = (426,320)
```
### Image
* reference image  
<img src="image/img1.jpg" width="80%" alt="reference image">  

* deformed image (x:+5, y:+5) 
<img src="image/img1_shefted_5_5.jpg" width="80%" alt="reference image">  

### Requirements

* Windows 11
* MinGW GCC
* Python 3.13.3

### How to run
* Build
```shell
# shell
mingw32-make all
```
* Run
```shell
# shell
python .\run_PSO.py
```
### Result  
```
Result: x=5.006, y=4.989, coefficient=0.998
```
