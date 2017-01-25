#!/bin/tcsh

if ( $#argv == 0 ) then
  set bindir = ../../../bin
else
  set bindir = ../$argv[1]
endif
  
set mdiff_f = $bindir/mdiff

cd test
rm -f *.results
rm -f *.valgrind
rm -f results.log valgrind.log

source ../run_test_helper.sh $bindir
cd ../compare
set files = `echo *.results`
cd ../test

foreach test ( $files )
    echo "#############################################"
    echo "      $test"
    echo "#############################################"
    if ( -e $mdiff_f ) then
        $mdiff_f -s $test ../compare/$test >> results.log
    else
        echo "ERROR $test ../compare/$test" >> results.log
    endif
    tail -n 1 results.log
end

awk '$1!="$bin"{print $0}$1=="$bin"{print "valgrind",$0,">&",NR".valgrind"}' \
         ../run_test_helper.sh > run_valgrind.sh

source run_valgrind.sh
grep 'ERROR SUMM' *.valgrind | awk -F: '{print $1,$3}' | sed 's/(suppressed//g' >> valgrind.log
grep 'No such file' *.valgrind | awk -F: '{print $1,$4}' >> valgrind.log

cd ../
