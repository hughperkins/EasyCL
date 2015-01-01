#!/bin/bash

for name in $(ls test* unittests); do {
    eval ./$name
    if [[ $? != 0 ]]; then {
        echo FAIL: $name
        exit 1
    } fi
} done

