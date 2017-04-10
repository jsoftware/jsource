#!/bin/bash
# rm all *.o for clean builds - makefile dependencies are not set 
find $jbld/jout -name "*.o" -type f -delete
find $jgit/jsrc -name "*.o" -type f -delete
find $jgit/dllsrc -name "*.o" -type f -delete



