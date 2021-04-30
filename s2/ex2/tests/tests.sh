#!/bin/bash

#Made by Ron Even

VERSION=1.4

TMP_FILE="./.tmpfile"

DEBUG=0
if [ "$1" == "-d" ]; then
        DEBUG=1
fi

COUNTER=0
FAILED_TESTS=""

print_single_invalid_output()
{
    echo -e "$1\n"
    if [ $DEBUG -eq 1 ]; then
            echo -e "Expected as hex:"
            echo -e "$1" | hexdump -C
            printf "\n"
    fi
}

print_actual_output()
{
    echo -e "Got:\n$1\n"
    if [ $DEBUG -eq 1 ]; then
            echo -e "Got as hex:"
            echo -e "$1" | hexdump -C
            echo -e "\n"
    fi
}

print_error_header_for_command()
{
    echo -e "\t\t❌ FAILED"
    echo -e "$1\n"
}

print_success_header()
{
    echo -e "\t\t✅ PASSED"
}

validate() {
        command="$1"
        COUNTER=$((COUNTER + 1))
        echo "*****************************************"
        printf "$COUNTER"
        eval "$command" > $TMP_FILE 2>&1
        result=$(cat $TMP_FILE)
        rm -f $TMP_FILE

        # Ignore first argument
        shift

        matched=0
        general_error=0

        for arg in "$@"
        do
            expected=`printf "$arg"`

            if [ "$result" == "$expected" ]
            then
                    print_success_header
                    matched=1
            fi
        done

        # If we reached here and matched=0 it means no matches were found. Print details
        if [ $matched -eq 0 -a $general_error -ne 1 ]
        then
            print_error_header_for_command "$command"
            print_actual_output "$result"

            if [ $# -gt 1 ]
            then
                echo -e "Expected (one of the $# following outputs):\n"
                for arg in "$@"
                do
                    expected=`printf "%s" "$arg"`

                    print_single_invalid_output "$expected"

                    if [ "$arg" != "${@: -1}" ]
                    then
                        echo -e "----- OR -----\n"
                    fi
                done
            else
                echo -e "Expected:\n"
                print_single_invalid_output "$expected"
            fi

            FAILED_TESTS+="$COUNTER,"
        fi
}

validate_test_files() {
    REQUIRED_FILES="Temp hostnamectl os-release tests.sh tests_ex21 tests_ex22"
    NOT_FOUND_REQUIRED_FILES=0
    for required in $REQUIRED_FILES
    do
        found=$(ls | grep -wc "$required")
        if [ $found -eq 0 ]
        then
            echo "❌ couldn't find $required"
            NOT_FOUND_REQUIRED_FILES=$[NOT_FOUND_REQUIRED_FILES + 1]
        fi
    done

    if [ $NOT_FOUND_REQUIRED_FILES -ne 0 ]
    then
        echo "$NOT_FOUND_REQUIRED_FILES files/directories couldn't be found in the tests directory (`pwd`)"
        echo "Please make sure they exist and run tests.sh again"
        exit 1
    fi
}

declare_step() {
    echo "Testing $1..."
}

declare_stage() {
    echo "           Testing $1             "
}

COUNT_CHARS_CMD="wc -c | tr -d ' '"

validate_test_files

echo -e "\n\n"
SCRIPT="ex21.sh"
TEST_DIR="tests_ex21"
declare_stage "./$SCRIPT"
if [ -f "./$SCRIPT" ]
then
        declare_step "Regular usage"
        validate "./$SCRIPT $TEST_DIR txt geek_duck" "Geek_duck was was a good programmer. He knew all kind of non-OOP languages.\ngeek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\nMaybe geek_duck is a geek duck, but it will pass the test.\n"
        validate "./$SCRIPT $TEST_DIR txt geek | $COUNT_CHARS_CMD" "278\n"
        validate "./$SCRIPT $TEST_DIR txt duck | $COUNT_CHARS_CMD" "403\n"
        validate "./$SCRIPT $TEST_DIR txt SOLID" "One day, the geek duck studied SOLID\n"

        declare_step "Case insensitivity"
        validate "./$SCRIPT $TEST_DIR txt gEeK_dUcK" "Geek_duck was was a good programmer. He knew all kind of non-OOP languages.\ngeek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\nMaybe geek_duck is a geek duck, but it will pass the test.\n"
        validate "./$SCRIPT $TEST_DIR txt gEEk | $COUNT_CHARS_CMD" "278\n"
        validate "./$SCRIPT $TEST_DIR txt DUck | $COUNT_CHARS_CMD" "403\n"
        validate "./$SCRIPT $TEST_DIR txt SoLiD" "One day, the geek duck studied SOLID\n"

        declare_step "Query subsequences"
        validate "./$SCRIPT $TEST_DIR txt gee" ""
        validate "./$SCRIPT $TEST_DIR txt duc" ""
        validate "./$SCRIPT $TEST_DIR txt SOL" ""

        declare_step "Extension subsequences"
        validate "./$SCRIPT $TEST_DIR tx geek | $COUNT_CHARS_CMD" "165\n"
        validate "./$SCRIPT $TEST_DIR tx duck | $COUNT_CHARS_CMD" "165\n"
        validate "./$SCRIPT $TEST_DIR tx SOLID | $COUNT_CHARS_CMD" "162\n"

        declare_step "Irrelevant extensions"
        validate "./$SCRIPT $TEST_DIR jpg geek_duck" ""
        validate "./$SCRIPT $TEST_DIR jpg gee" ""
        validate "./$SCRIPT $TEST_DIR jpg duc" ""

        declare_step "More arguments than required"
        validate "./$SCRIPT $TEST_DIR txt DUck im extra arguments you should ignore | $COUNT_CHARS_CMD" "403\n"

        declare_step "Invalid commands"
        validate "./$SCRIPT $TEST_DIR txt" "Not enough parameters"
        validate "./$SCRIPT $TEST_DIR" "Not enough parameters"

        declare_step "Example scenarios from moodle"
        validate "./$SCRIPT ./Temp/ txt Hello" "Hello World\n"
        validate "./$SCRIPT ./Temp/ txt world" "Hello World\nWhat a wonderful world\n"
        validate "./$SCRIPT ./Temp/ txt" "Not enough parameters\n"
else
        echo "❌ Couldn't find $SCRIPT file!"
        echo "Make sure the tests script is in the same directory as $SCRIPT"
fi

echo -e "\n\n"
SCRIPT="ex22.sh"
TEST_DIR="tests_ex22"
declare_stage "./$SCRIPT"
if [ -f "./$SCRIPT" ]
then
        declare_step "Max length boundaries"
        validate "./$SCRIPT $TEST_DIR txt geek_duck 17" ""
        validate "./$SCRIPT $TEST_DIR txt geek_duck 16" "geek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\n"
        validate "./$SCRIPT $TEST_DIR txt geek_duck 14" "geek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\n"
        validate "./$SCRIPT $TEST_DIR txt geek_duck 13" "Geek_duck was was a good programmer. He knew all kind of non-OOP languages.\ngeek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\n"
        validate "./$SCRIPT $TEST_DIR txt geek_duck 12" "Geek_duck was was a good programmer. He knew all kind of non-OOP languages.\ngeek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\nMaybe geek_duck is a geek duck, but it will pass the test.\n"
        validate "./$SCRIPT $TEST_DIR txt geek_duck 0" "Geek_duck was was a good programmer. He knew all kind of non-OOP languages.\ngeek_Duck liked C, Assembly and even Pascal, but his favorite language was (as you may guess)\nMaybe geek_duck is a geek duck, but it will pass the test.\n"

        validate "./$SCRIPT $TEST_DIR txt hospital 16" ""
        validate "./$SCRIPT $TEST_DIR txt hospital 15" "It was supposed to be a great day, but GD ended up at the hospital.\n"
        validate "./$SCRIPT $TEST_DIR txt hospital 13" "It was supposed to be a great day, but GD ended up at the hospital.\nSo, hospital. As you could already realize, our poor duck has no luck.\n"
        validate "./$SCRIPT $TEST_DIR txt hospital 9" "It was supposed to be a great day, but GD ended up at the hospital.\nAnd our poor geek duck is at the hospital\nSo, hospital. As you could already realize, our poor duck has no luck.\n"
        validate "./$SCRIPT $TEST_DIR txt hospital 0" "It was supposed to be a great day, but GD ended up at the hospital.\nAnd our poor geek duck is at the hospital\nSo, hospital. As you could already realize, our poor duck has no luck.\n"

        declare_step "Query subsequences"
        validate "./$SCRIPT $TEST_DIR txt gee 1" ""
        validate "./$SCRIPT $TEST_DIR txt duc 1" ""
        validate "./$SCRIPT $TEST_DIR txt SOL 1" ""

        declare_step "Extension subsequences"
        validate "./$SCRIPT $TEST_DIR tx hospital 1" ""
        validate "./$SCRIPT $TEST_DIR tx duck 1" ""

        declare_step "Irrelevant extensions"
        validate "./$SCRIPT $TEST_DIR jpg geek_duck 1" ""
        validate "./$SCRIPT $TEST_DIR jpg gee 1" ""
        validate "./$SCRIPT $TEST_DIR jpg duc 1" ""

        declare_step "Lexicographical order"
        validate "./$SCRIPT $TEST_DIR txt Quavid 1" "Just a teaser for you, so you will be eager to continue reading (or for the test to be better): Quavid-19\nSo back to our evolving story, our geek duck and Quavid-19.\nThey called it Quavid-19 (Funny? No? I agree, but we're telling a story here)\nMore about what happened with Quavid-19 on the next chapter\nSo, after Quavid-19 was very common, the minister of health in Duckland bought millions of vaccines.\n"

        declare_step "More arguments than required"
        validate "./$SCRIPT $TEST_DIR txt hospital 13 this line contains extra arguments" "It was supposed to be a great day, but GD ended up at the hospital.\nSo, hospital. As you could already realize, our poor duck has no luck.\n"

        declare_step "Invalid commands"
        validate "./$SCRIPT $TEST_DIR txt geek_duck" "Not enough parameters\n"
        validate "./$SCRIPT $TEST_DIR txt" "Not enough parameters\n"
        validate "./$SCRIPT $TEST_DIR" "Not enough parameters\n"

        declare_step "Example scenarios from moodle"
        validate "./$SCRIPT ./Temp/ txt Hello 1" "Hello World\nHello World?\nHello World\nHello World\nHello World\n"
        validate "./$SCRIPT ./Temp/ txt world 1" "Hello World\nWhat a wonderful world\nHello World?\nWhat a wonderful world!\nWorld\nHello World\nWhat a wonderful world\nHello World\nWhat a wonderful world\nWorld\nHello World\nWhat a wonderful world\nWorld!\n"
        validate "./$SCRIPT ./Temp/ txt world 4" "What a wonderful world\nWhat a wonderful world!\nWhat a wonderful world\nWhat a wonderful world\nWhat a wonderful world\n"
        validate "./$SCRIPT ./Temp/ txt" "Not enough parameters\n"
else
        echo "❌ Couldn't find $SCRIPT file!"
        echo "Make sure the tests script is in the same directory as $SCRIPT"
fi

echo -e "\n\n"
SCRIPT="ex23.sh"
declare_stage "./$SCRIPT"
if [ -f "./$SCRIPT" ]
then
        host_all=$(cat hostnamectl)
        system_all=$(cat os-release)
        declare_step "Simple valid cases"
        validate "./$SCRIPT host --machine_id --virtualization --boot_id" "beb217fbb4324b7d9959f78c279e6599\nqemu\n10f00cc5ca614b518a84d1793d0134bc\n"
        validate "./$SCRIPT system --pretty_name --home_url --name" "Ubuntu 20.04.1 LTS\nhttps://www.ubuntu.com/\nUbuntu\n"

        declare_step "No existing flags"
        validate "./$SCRIPT host --what --is --this --flag" "$host_all"
        validate "./$SCRIPT system --what --is --this --flag" "$system_all"
        validate "./$SCRIPT host --field_that_doesnt_exist" "$host_all"
        validate "./$SCRIPT system --field_that_doesnt_exist" "$system_all"

        declare_step "Valid flags for other group"
        validate "./$SCRIPT host --version" "$host_all"
        validate "./$SCRIPT system --boot_id" "$system_all"

        declare_step "Valid and invalid flags combinations"
        validate "./$SCRIPT host --machine_id --version --support_url" "beb217fbb4324b7d9959f78c279e6599\n"
        validate "./$SCRIPT system --machine_id --version --support_url" "20.04.1 LTS (Focal Fossa)\nhttps://help.ubuntu.com/\n"

        declare_step "host --name special case"
        validate "./$SCRIPT host --name" "computer-vm\n" "$host_all"

        declare_step "Duplicate flags"
        validate "./$SCRIPT host --static_hostname --static_hostname --static_hostname" "nixcraft-www-42\n"
        validate "./$SCRIPT host --static_hostname --kernel --static_hostname" "nixcraft-www-42\nLinux 4.10.0-42-generic\n"
        validate "./$SCRIPT host --static_hostname --why-are-you-asking-kitbag-questions --static_hostname" "nixcraft-www-42\n"
        validate "./$SCRIPT host --static_hostname --static_hostname --architecture" "nixcraft-www-42\nx86-64\n"
        validate "./$SCRIPT host --architecture --static_hostname --static_hostname" "x86-64\nnixcraft-www-42\n"
        validate "./$SCRIPT system --name --name --name" "Ubuntu\n"
        validate "./$SCRIPT system --name --pretty_name --name" "Ubuntu\nUbuntu 20.04.1 LTS\n"
        validate "./$SCRIPT system --name --why-are-you-asking-kitbag-questions --name" "Ubuntu\n"
        validate "./$SCRIPT system --name --name --pretty_name" "Ubuntu\nUbuntu 20.04.1 LTS\n"
        validate "./$SCRIPT system --pretty_name --name --name" "Ubuntu 20.04.1 LTS\nUbuntu\n"

        declare_step "Example scenarios from moodle"
        validate "./$SCRIPT host --static_hostname --icon_name --architecture" "nixcraft-www-42\ncomputer-vm\nx86-64\n"
        validate "./$SCRIPT host --static_hostname --icon_name --name --architecture" "nixcraft-www-42\ncomputer-vm\nx86-64\n" "nixcraft-www-42\ncomputer-vm\ncomputer-vm\nx86-64\n"
        validate "./$SCRIPT system --static_hostname --icon_name --name --architecture" "Ubuntu\n"
        validate "./$SCRIPT system --name --version --support_url" "Ubuntu\n20.04.1 LTS (Focal Fossa)\nhttps://help.ubuntu.com/\n"
        validate "./$SCRIPT host" "$host_all"
        validate "./$SCRIPT system" "$system_all"
        validate "./$SCRIPT" "Invalid input\n"
        validate "./$SCRIPT --name" "Invalid input\n"
else
        echo "❌ Couldn't find $SCRIPT file!"
        echo "Make sure the tests script is in the same directory as $SCRIPT"
fi


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
    echo "NOTE: Expected and actual results look the same?"
    echo "Maybe there is a hidden character or an excess \n your code prints."
    echo "Run \"$0 -d\" to see the exact ASCII bytes of the expected and actual results"
fi

echo "*****************************************"


echo ""
echo "*****************************************"
echo "**  This script was made by Ron Even   **"
echo "**         script version: $VERSION         **"
echo "*****************************************"