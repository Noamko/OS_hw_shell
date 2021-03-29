#!/bin/bash

#Made by Ron Even

VERSION=1.5

HOSTNAME=$(hostname)

TMP_DIR=".tmpdir"
TMP_FILE="./.tmpfile"
CMD_FILE="./.cmd"

DEBUG=0
if [ "$1" == "-d" ]; then
        DEBUG=1
fi

COUNTER=0
FAILED_TESTS=""

build_command_sequence() {
  local separator="\n"
  local first="$1"
  shift
  printf "%s" "$first" "${@/#/$separator}" $separator
}

# Ugly hack to make sure $HOME comparisons will work on planet
# Original $HOME is /u/stud/<STUDENT_USERNAME>
# When cd-ing directly there, it redirects to /v/students/<STUDENT_USERNAME>
fix_path()
{
    if [ "$HOSTNAME" == "planet" ]
    then

        NEW_PATH=$(printf "$1" | sed "s/\/u\/stud/\/v\/students/g")
        echo "$NEW_PATH"
    else
        echo "$1"
    fi
}

validate() {
        COUNTER=$((COUNTER + 1))
        echo "*****************************************"
        printf "$COUNTER"
        printf "$1" > $CMD_FILE
        ../a.out <<< "`cat $CMD_FILE`" > $TMP_FILE 2>&1
        error=$?
        result=`cat $TMP_FILE`
        rm -f $TMP_FILE $CMD_FILE
        expected=`printf "$2"`
        expected=`fix_path "$expected"`

        if [ $error -ne 0 ]
        then
                 echo -e "\t\t❌ FAILED"
                 echo -e "$1\n"
                 echo -e "Return code was $error and not 0! Make sure the program doesn't crash"
                 FAILED_TESTS+="$COUNTER,"
                 return
        fi

        pgrep -u "$(whoami)" a.out > /dev/null
        forks_cleared=$?

        if [ "$result" == "$expected" ]
        then
                if [ $forks_cleared -eq 0 ]
                then
                    echo -e "\t\t❌ FAILED\n"
                    echo -e "$1\n"
                    echo "Forks left after shell exited!!"
                    FAILED_TESTS+="$COUNTER,"
                    return
                fi

                echo -e "\t\t✅ PASSED"
                #echo -e "$1"
        else
                echo -e "\t\t❌ FAILED\n"
                echo -e "$1\n"
                echo -e "Expected:\n$expected\n"
                if [ $DEBUG -eq 1 ]; then
                        echo -e "Expected as hex:"
                        echo -e "$expected" | hexdump -C
                        printf "\n"
                fi
                echo -e "Got:\n$result\n"
                if [ $DEBUG -eq 1 ]; then
                        echo -e "Got as hex:"
                        echo -e "$result" | hexdump -C
                fi

                FAILED_TESTS+="$COUNTER,"
        fi

        pkill -u "$(whoami)" a.out
}

declare_step() {
    echo "Testing $1..."
}


# Quick variables for prompt sequence and prompt end sequence
P="\$"
PEND="$P \n"

CHOOSE_VARIANT()
{
    if [ "$HOSTNAME" == "planet" ]
    then
        echo $2
    else
        echo $1
    fi
}

ERROR_NO_SUCH_FILE_OR_DIRECTORY()
{
    CHOOSE_VARIANT "ls: $1: No such file or directory" "ls: cannot access $1: No such file or directory"
}

if [ -f a.out ]
then
        ORIG_WD=$(pwd)
        mkdir -p $TMP_DIR
        cd $TMP_DIR
        CURRENT_WD=$(pwd)
        echo "Killing existing a.out shells"
        pkill -u "$(whoami)" a.out
        declare_step "basic commands"
        validate "$(build_command_sequence "touch file" "ls file" "exit")" "$P $P file\n$PEND"
        validate "$(build_command_sequence "ls file1" "exit")" "$P $(ERROR_NO_SUCH_FILE_OR_DIRECTORY file1)\n$PEND"
        validate "$(build_command_sequence "history" "exit")" "$P history RUNNING\n$PEND"

        declare_step "nonexistent commands"
        validate "$(build_command_sequence "wtf" "exit")" "$P exec failed\n$PEND"
        validate "$(build_command_sequence "wtf" "history" "exit")" "$P exec failed\n$P wtf DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "wtf" "history" "exit")" "$P exec failed\n$P wtf DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "wtf &" "sleep 1" "history" "exit")" "$P $P exec failed\n$P wtf DONE\nsleep 1 DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "wtf &" "sleep 1" "wtf2" "history" "exit")" "$P $P exec failed\n$P exec failed\n$P wtf DONE\nsleep 1 DONE\nwtf2 DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "sleep 1 &" "history" "exit")" "$P $P sleep 1 RUNNING\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "sleep 1 &" "sleep 2 &" "history" "exit")" "$P $P $P sleep 1 RUNNING\nsleep 2 RUNNING\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "sleep 1" "sleep 2 &" "history" "exit")" "$P $P $P sleep 1 DONE\nsleep 2 RUNNING\nhistory RUNNING\n$PEND"

        declare_step "cd valid usage commands"
        validate "$(build_command_sequence "cd /" "pwd" "exit")" "$P $P /\n$PEND"
        validate "$(build_command_sequence "cd ~" "pwd" "exit")" "$P $P $HOME\n$PEND"
        validate "$(build_command_sequence "cd ~/" "pwd" "exit")" "$P $P $HOME\n$PEND"

        declare_step "cd nonexistent paths commands"
        validate "$(build_command_sequence "cd /what" "pwd" "exit")" "$P chdir failed\n$P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd ~/the" "pwd" "exit")" "$P chdir failed\n$P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd -/hell" "pwd" "exit")" "$P An error occurred\n$P $CURRENT_WD\n$PEND"

        declare_step "valid substitutions commands"
        mkdir ~/.test ./.test
        validate "$(build_command_sequence "cd ~/.test" "cd .." "pwd" "exit")" "$P $P $P $HOME\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "cd ../" "pwd" "exit")" "$P $P $P $HOME\n$PEND"
        validate "$(build_command_sequence "cd /" "cd .." "pwd" "exit")" "$P $P $P /\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "pwd" "exit")" "$P $P $HOME/.test\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "cd -" "pwd" "exit")" "$P $P $P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "cd -" "cd -" "pwd" "exit")" "$P $P $P $P $HOME/.test\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "cd -" "cd -" "cd -" "pwd" "exit")" "$P $P $P $P $P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd ~/.test" "cd -" "cd -" "cd -" "cd -" "pwd" "exit")" "$P $P $P $P $P $P $HOME/.test\n$PEND"
        validate "$(build_command_sequence "cd ../../../../../../../../../../../../../../../../../" "pwd" "exit")" "$P $P /\n$PEND"
        validate "$(build_command_sequence "cd /dev/~" "pwd" "exit")" "$P chdir failed\n$P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd /dev/../~" "pwd" "exit")" "$P chdir failed\n$P $CURRENT_WD\n$PEND"
        validate "$(build_command_sequence "cd /dev/../" "pwd" "exit")" "$P $P /\n$PEND"
        validate "$(build_command_sequence "cd .test" "pwd" "exit")" "$P $P $CURRENT_WD/.test\n$PEND"
        validate "$(build_command_sequence "cd ./.test" "pwd" "exit")" "$P $P $CURRENT_WD/.test\n$PEND"

        declare_step "echo multiple arguments commands"
        validate "$(build_command_sequence "cd .test" "echo a b c d e" "exit")" "$P $P a b c d e\n$PEND"
        validate "$(build_command_sequence "cd .test" "echo a b c d e" "history" "exit")" "$P $P a b c d e\n$P cd .test DONE\necho a b c d e DONE\nhistory RUNNING\n$PEND"
        rm -r ~/.test ./.test

        declare_step "echo special cases"
        validate "$(build_command_sequence 'echo comp' "exit")" "$P comp\n$PEND"
        validate "$(build_command_sequence 'echo "struct"' "exit")" "$P struct\n$PEND"

        declare_step "jobs command cases"
        validate "$(build_command_sequence "jobs" "exit")" "$P $PEND"
        validate "$(build_command_sequence "sleep 1" "jobs" "exit")" "$P $P $PEND"
        validate "$(build_command_sequence "sleep 10 &" "jobs" "exit")" "$P $P sleep 10\n$PEND"
        validate "$(build_command_sequence "sleep 10 &" "sleep 100 &" "jobs" "history" "exit")" "$P $P $P sleep 10\nsleep 100\n$P sleep 10 RUNNING\nsleep 100 RUNNING\njobs DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "sleep 25 &" "sleep 1" "echo hello" "jobs" "history" "exit")" "$P $P $P hello\n$P sleep 25\n$P sleep 25 RUNNING\nsleep 1 DONE\necho hello DONE\njobs DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "jobs" "exit")" "$P $PEND"

        declare_step "cwd keeping"
        validate "$(build_command_sequence "cd /" "exit")" "$P $PEND"
        validate "$(build_command_sequence "cd a b" "exit")" "$P Too many arguments\n$PEND"
        validate "$(build_command_sequence "cd a b c d e" "history" "exit")" "$P Too many arguments\n$P cd a b c d e DONE\nhistory RUNNING\n$PEND"

        # Automatic submit tests
        declare_step "usual cases from submit system"
        mkdir myAwesomeDir
        touch myAwesomeDir/myAwesomefile.txt

        validate "$(build_command_sequence "ls myAwesomeDir" "history" "exit")" "$P myAwesomefile.txt\n$P ls myAwesomeDir DONE\nhistory RUNNING\n$PEND"
        validate "$(build_command_sequence "mkdir AB" "cd AB" "touch A.c" "touch B.c" "ls" "exit")" "$P $P $P $P $P A.c\nB.c\n$PEND"
        validate "$(build_command_sequence "blabla" "exit")" "$P exec failed\n$PEND"
        validate "$(build_command_sequence "sleep 2" "exit")" "$P $PEND"

        rm -r myAwesomeDir

        if [ "$(pwd)" != "$CURRENT_WD" ]
        then
            echo -e "\t\t❌ FAILED\n"
            echo "Shell didn't maintain current working directory!"
            echo -e "CWD should be: $CURRENT_WD"
            echo -e "but was: $(pwd)"
        fi

        pkill -9 -u $(whoami) sleep

        cd $ORIG_WD
        rm -r $TMP_DIR

        echo ""
        echo "************ 📝  SUMMARY  📝 ************"
        failed_count=$(echo $FAILED_TESTS | tr -cd ',' | wc -c)
        success_count=$((COUNTER-failed_count))
        echo "           $success_count/$COUNTER tests passed!"
        if [ $success_count -ne $COUNTER ]
        then
            echo "Failed tests are:"
            FAILED_TESTS=${FAILED_TESTS%?}
            echo $FAILED_TESTS
        fi

        echo "*****************************************"
else
        echo "❌ Couldn't find a.out file!"
fi

pkill -9 -u "$(whoami)" a.out

echo ""
echo "*****************************************"
echo "**  This script was made by Ron Even   **"
echo "**         script version: $VERSION         **"
echo "*****************************************"