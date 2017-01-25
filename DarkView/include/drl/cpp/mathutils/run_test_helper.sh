#!/bin/tcsh

if ( $#argv == 0 ) then
  set bindir = ../../../bin
else
  set bindir = $argv[1]
endif
set bin = $bindir/cml_drl

$bin -d 2 -s ALL INT_SWISSROLL -x "[1 2]" PCA.1.map.results -o PCA.1.lowdim.results PCA.1.stats.results -m PCA
$bin -d 2 -s ALL INT_SWISSROLL -x "[1 2]" PCA.2.map.results -o PCA.2.lowdim.results PCA.2.stats.results -m PCA -e POWER -b 82127
$bin forward.in PCA.1.results -f PCA.1.map
$bin PCA.1.results PCA.2.results -f PCA.1.map
$bin reverse.in PCA.3.results -f PCA.1.map
$bin PCA.3.results PCA.4.results -f PCA.1.map
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL MDS.1.map.results -o MDS.1.lowdim.results MDS.1.stats.results -m MDS
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL LLE.1.map.results -o LLE.1.lowdim.results LLE.1.stats -m LLE -k 10 -e DQ
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL LLE.2.map.results -o LLE.2.lowdim.results LLE.2.stats -m LLE -k 20 -e DQ
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL LLE.3.map.results -o LLE.3.lowdim.results LLE.3.stats -m LLE -p 0.165 -e DQ
$bin forward.in LLE.1.results -f LLE.1.map
$bin LLE.1.results LLE.2.results -f LLE.1.map
$bin reverse.in LLE.3.results -f LLE.1.map
$bin LLE.3.results LLE.4.results -f LLE.1.map
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.1.map.results -o ISO.1.lowdim.results ISO.1.stats.results -m ISOMAP -k 3
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.2.map.results -o ISO.2.lowdim.results ISO.2.stats.results -m ISOMAP -k 10
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.9.map.results -o ISO.9.lowdim.results ISO.9.stats.results -m ISOMAP -p 0.145
$bin forward.in ISO.1.results -f ISO.1.map
$bin ISO.1.results ISO.2.results -f ISO.1.map
$bin reverse.in ISO.3.results -f ISO.1.map    
$bin ISO.3.results ISO.4.results -f ISO.1.map 
$bin forward.in ISO.11.results -f ISO.1.map -p 0.23
$bin ISO.11.results ISO.12.results -f ISO.1.map -p 0.23
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.3.map.results -o ISO.3.lowdim.results ISO.3.stats.results -m ISOMAP -k 10 -l RANDOM 0.25
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.4.map.results -o ISO.4.lowdim.results ISO.4.stats.results -m ISOMAP -k 10 -l STRING "[0:99]"
$bin -d 2 -s ALL -x "[1 2]" INT_SWISSROLL ISO.10.map.results -o ISO.10.lowdim.results ISO.10.stats.results -m ISOMAP -p 0.145 -l STRING "[0:99]"
$bin forward.in ISO.5.results -f ISO.3.map
$bin ISO.5.results ISO.6.results -f ISO.3.map
$bin reverse.in ISO.7.results -f ISO.3.map 
$bin ISO.7.results ISO.8.results -f ISO.3.map 
$bin -d "3 6 3 2" -s ALL -x "[1 2]" INT_SWISSROLL ATE.1.map.results -o ATE.1.lowdim.results ATE.1.stats.results -m AE -a 0.8 10 10 10 10
$bin -d "3 5 4 2" -s ALL -x "[1 2]" INT_SWISSROLL ATE.2.map.results -o ATE.2.lowdim.results ATE.2.stats.results -m AE -a 0.6 20 15 20 20
$bin forward.in ATE.1.results -f ATE.1.map
$bin ATE.1.results ATE.2.results -f ATE.1.map
$bin reverse.in ATE.3.results -f ATE.1.map
$bin ATE.3.results ATE.4.results -f ATE.1.map
$bin INT_SWISSROLL intrinsic.1.results -u
$bin INT_SWISSROLL intrinsic.2.results -u -d 2
$bin INT_SWISSROLL intrinsic.3.results -u -p 0.145
