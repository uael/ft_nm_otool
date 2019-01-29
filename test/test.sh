#!/usr/bin/env bash

# Go to project root dir
cd $(dirname $0)/..;

mkdir -p out

for i in $(cat $3); do
  echo -n " + $1 $i >a; $2 $i >b; diff a b :"

  $1 ${i} >out/a 2>out/a_err; st1=$?;
  $2 ${i} >out/b 2>out/b_err; st2=$?;

  if [[ ${st1} != ${st2} ]]; then
    if [[ ${st1} == "1" ]]; then
      echo "$(tput setaf 2)OK$(tput sgr0)"
    else
      echo "$(tput setaf 1)KO ($st1 != $st2)$(tput sgr0)"
      diff -w out/a out/b
    fi
  elif [[ ${st1} == "0" ]]; then
    diff -w out/a out/b > out/diff
    if [[ $? != "0" ]]; then
      echo "$(tput setaf 1)KO$(tput sgr0)"
      cat out/diff
      read -n1 ans
    else
      echo "$(tput setaf 2)OK$(tput sgr0)"
    fi
  else
    echo "$(tput setaf 2)OK$(tput sgr0)"
  fi
done
