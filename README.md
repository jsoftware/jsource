## J: From C to C++20

J is an [array programming language](https://en.wikipedia.org/wiki/Array_programming) created by Ken Iverson and Roger Hui. 

TODO: add image later

This is a fork of `jsoftware/jsource` and we will be porting it to C++20.

### Goals

* [ ] Compile with GCC 10+
* [ ] Compile with Clang 11+
* [ ] Remove all (most) of the macros
* [ ] Clang-format the code base
* [ ] Get both build / tests running in parallel
* [ ] Monitor compile & run time perf while refactoring

### Comparison of Languages

Calculating the first 10 odd numbers:
**Python**:
```python
[1 + 2 * i for i in range(10)]
```
**Haskell**:
```hs 
map (1+) $ map (2*) [0..9]
```
**APL**:
```apl
1+2×⍳10
```
**J**:
```ijs
1+2*i.10
```
