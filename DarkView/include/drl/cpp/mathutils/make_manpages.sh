#!/bin/tcsh

make
set execs="cml_drl vm_slice"
set execdir="../bin"

cd ../../manpages

foreach exec ($execs)
  $execdir/$exec -h > $exec.manpage
  eqn $exec.manpage > $exec.1
  man -t -p eqn ./$exec.manpage > $exec.ps
  ps2pdf $exec.ps $exec.pdf
end

cd ../


