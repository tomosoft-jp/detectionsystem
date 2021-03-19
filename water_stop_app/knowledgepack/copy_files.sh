#!/bin/bash

usage(){
    echo "Usage: copy_files.sh <path_to_sensiml_sensortile_sdk_root>"
}

if test $# -ne 1;
    then
    usage
    exit 1
fi

repo_path=$1
app_name="sensortile_ai_app"

\cp -rf knowledgepack_project/* $repo_path/App/$app_name/knowledgepack

\cp -f $repo_path/App/$app_name/knowledgepack/inc/sml_output.h $repo_path/App/$app_name/inc/
\cp -f $repo_path/App/$app_name/knowledgepack/src/sml_output.c $repo_path/App/$app_name/src/

pushd $repo_path/App/$app_name/GCC_Project
mkdir -p output/bin
cd output
mkdir depend
popd