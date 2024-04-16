#!/bin/bash

folder=$(dirname $0) # $PWD $BASH_SOURCE

if [ $# -eq 1 ] ; then
    folder=${1}
fi

for file in ${folder}/edu-cmc-skmodel*; do # 23-6[0-9][0-9]-[0-9][0-9]
    if [ -d $file ]; then
        continue
    fi

    fname=$(basename $file)
    if [ $fname != ${fname%%.*}.pub ]; then
        mv ${folder}/$fname ${folder}/${fname%%.*}.pub
        file=${folder}/${fname%%.*}.pub
    fi

    if [ -x $file ]; then
        chmod -x $file
    fi

    if [ -z "$(ssh-keygen -l -f $file 2>/dev/null)" ]; then
        converted_key=$(ssh-keygen -i -f $file 2>/dev/null)
        
        if [ -n "$converted_key" ]; then
            echo $converted_key > $file
            echo "$file has been converted"
        else
            echo "$file is incorrect"
            file=""
        fi
    fi

    if [ -n "$file" ]; then
        fname=$(basename $file)
        new_folder=${folder}/${fname%-*}

        if [ ! -d "$new_folder" ]; then
            mkdir $new_folder
        fi

        mv $file $new_folder
    fi
done
