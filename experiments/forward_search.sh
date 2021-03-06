#! /bin/bash

directory_name=$1
header_argument="--header"

echo "\begin{table}"
echo "\begin{center}"
echo "\begin{tabular}[!htb]{l|r|r|r}"

for i in `ls -1 $1`; do

    ../bin/qc --experiment=forward-search --input-file=$directory_name/$i $header_argument --latex --timeout=0.1

    if [ "$header_argument" == "--header" ]; then
        header_argument=""
    fi
done

echo "\hline"
echo "\end{tabular}"
echo "\end{center}"
echo "\caption{Run times of approximation algorithms for `basename $directory_name` data set with timeout=0.1}"
echo "\end{table}"

echo "\begin{table}"
echo "\begin{center}"
echo "\begin{tabular}[!htb]{l|r|r|r}"

for i in `ls -1 $1`; do

    ../bin/qc --experiment=forward-search --input-file=$directory_name/$i $header_argument --latex --timeout=1

    if [ "$header_argument" == "--header" ]; then
        header_argument=""
    fi
done

echo "\hline"
echo "\end{tabular}"
echo "\end{center}"
echo "\caption{Run times of approximation algorithms for `basename $directory_name` data set with timeout=1}"
echo "\end{table}"

echo "\begin{table}"
echo "\begin{center}"
echo "\begin{tabular}[!htb]{l|r|r|r}"

for i in `ls -1 $1`; do

    ../bin/qc --experiment=forward-search --input-file=$directory_name/$i $header_argument --latex --timeout=10

    if [ "$header_argument" == "--header" ]; then
        header_argument=""
    fi
done

echo "\hline"
echo "\end{tabular}"
echo "\end{center}"
echo "\caption{Run times of approximation algorithms for `basename $directory_name` data set with timeout=10}"
echo "\end{table}"
