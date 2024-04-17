#!/bin/bash

n=$(($RANDOM % 9900 + 100))
m=$(($RANDOM % 9 + 1))

if [ $# -eq 2 ] ; then
    n=$1
    m=$2
fi

answer=$(($m * $n))

lenght_n=${#n}
lenght_answer=${#answer}

tput clear

width=$(tput cols)
x_right=$(($width / 2 + $lenght_answer / 2))
y_razryad=3
y_n=$(($y_razryad + 1))
y_m=$(($y_n + 1))
y_answer=$(($y_m + 1))
x_info=$(($width / 3))
y_info=$(($y_answer + 2))

tput cup $y_n $(($x_right - $lenght_n))
printf "%d" $n

tput cup $y_m $(($x_right - 1))
printf "%d" $m

function clear_info
{
    tput cup $y_info 0
    for (( i = 0; i < width; i++ )); do
        printf " "
    done
}

function info_yes
{
    clear_info
    tput cup $y_info $x_info
    printf "Yes! You are right!"
}

function info_no
{
    printf "\b \b"
    clear_info
    tput cup $y_info $x_info
    printf "No! You are wrong! Try again!"
}

razryad=0

for (( x = x_right-1; x >= x_right - lenght_answer; x-- )); do
    idx=$(($lenght_n - ($x_right - $x)))

    if [ $razryad != 0 -a $idx -ge 0 ]; then
        tput cup $y_razryad $x
        read -n1 digit
        while [ $digit != $razryad ]; do
            info_no
            tput cup $y_razryad $x
            read -n1 digit
        done
        info_yes
    fi
    
    if [ $idx -ge 0 ]; then
        res=$((${n:idx:1} * $m + $razryad))
    else
        res=$razryad
    fi
    correct_digit=$(($res % 10))
    razryad=$(($res / 10))

    tput cup $y_answer $x
    read -n1 digit
    while [ $digit != $correct_digit ]; do
        info_no
        tput cup $y_answer $x
        read -n1 digit
    done
    info_yes

done

tput cup $(($y_info + 1)) 0
