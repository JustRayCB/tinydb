#!/bin/bash

err(){
    echo "E: $*" >> /dev/stderr
}

TO_LAUNCH=$1
PATH_TO_DB=${2:-data/students.bin}
QUERIES=$3

if [[ $TO_LAUNCH == "run" ]]
then
    #soit le fichier existe
    if [[ -f "$PATH_TO_DB" ]]
    then
        if [[ $QUERIES == "" ]]
        then
            echo "On lance le programme sans queries"
            echo "$!"
            ./tinydb $PATH_TO_DB
        else
            if [[ -f $QUERIES ]]
            then
                echo "On lance le programme avec queries"
            else
                echo "Le fichier texte n'existe pas"
            fi
        fi
    else
        #soit le fichier n'existe pas
        mkdir data
        touch $PATH_TO_DB
        err "Le chemin vers la db n'existe pas"
    fi
elif [[ $TO_LAUNCH == "status" ]]
then
    echo "On est dans status"
    INSTANCE_COUNT=$(pgrep tinydb -c)
    INSTANCE_COUNT=$((INSTANCE_COUNT/5))
    echo "$INSTANCE_COUNT instance(s) running"
    ALL_INSTANCES=$(pgrep tinydb)
    COUNT=0
    for pid in $ALL_INSTANCES
    do
        if ! ((COUNT % 5))
        then
            echo -e "  > PID $pid"
            COUNT=$((COUNT+1))
        else 
            COUNT=$((COUNT+1))
        fi
    done
    

elif [[ $TO_LAUNCH == "sync" ]]
then
    echo "On est dans sync"
elif [[ $TO_LAUNCH == "shutdown" ]]
then
    echo "On est dans shutdown"
else
    echo -e "Mauvaise utilisation!\nUtilisez : './monitoring opt [chemin_vers_db] [fichier_requetes]'\nopt=run ou status ou sync ou shutdown\nLes crochets signifient que les arguments sont optionnels"
fi