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
center=$(($width / 2))

x_left=$(($center - $lenght_n / 2))
x_right=$(($x_left + $lenght_n - 1))
y_top=3
y_bot=$(($y_top + 3))
x_word=$(($width / 3))
y_word=$(($y_bot + 2))

tput cup $(($y_top + 1)) $x_left
printf "%d" $n

tput cup $(($y_top + 2)) $x_right
printf "%d" $m

razryad=0
x_left_n=$x_left

# 1
x=$x_right

n_digit=${n:$(($x - $x_left)):1}
printf "\n"
echo $n $n_digit $x $x_left $(($x - $x_left))
printf "\n"
res=$(($n_digit * $m + $razryad))
correct_digit=$(($res % 10))
razryad=$(($res / 10))

tput cup $y_bot $x
read -n1 digit

while [ $digit != $correct_digit ]; do
    printf "\b "
    tput cup $y_word $x_word
    printf "No! You are wrong! Try again!"

    tput cup $y_bot $x
    read -n1 digit
done

tput cup $y_word $x_word
printf "Yes! You are right!"

x_left=$(($x_right - $lenght_answer + 1))
x_right=$(($x_right - 1))

for (( x=x_right; x >= x_left; x-- )); do

    tput cup 1 1
    echo $razryad

    tput cup $y_top $x
    read -n1 digit

    while [ $digit != $razryad ]; do
        printf "\b "
        tput cup $y_word $x_word
        printf "No! You are wrong! Try again!"

        tput cup $y_top $x
        read -n1 digit
    done

    tput cup $y_word $x_word
    printf "Yes! You are right!"

    # цифра произведения
    idx=$(($x - $x_left_n))
    if [ $idx -lt 0 ]; then
        res=$razryad
    else
        n_digit=${n:idx:1}
        res=$(($n_digit * $m + $razryad))
    fi
    correct_digit=$(($res % 10))
    razryad=$(($res / 10))

    tput cup 1 1
    echo $correct_digit $res $n_digit

    tput cup $y_bot $x
    read -n1 digit

    while [ $digit != $correct_digit ]; do
        printf "\b "
        tput cup $y_word $x_word
        printf "No! You are wrong! "

        tput cup $y_bot $x
        read -n1 digit
    done

    tput cup $y_word $x_word
    printf "Yes! You are right!"

done

tput cup $(($y_word + 1)) 0
printf "\n"
