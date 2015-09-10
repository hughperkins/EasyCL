#!/bin/bash

bin_dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dist_dir=$(dirname ${bin_dir})

export PATH=${bin_dir}:$PATH
export LD_LIBRARY_PATH=${dist_dir}/lib:${LD_LIBRARY_PATH}
if [[ $(uname) == Darwin ]]; then {
    export DYLD_LIBRARY_PATH=${dist_dir}/lib:${DYLD_LIBRARY_PATH}
} fi
export CPATH=${dist_dir}/include:${dist_dir}/include/easycl
export LDFLAGS="-L${dist_dir}/lib ${LDFLAGS}"

