#!/usr/bin/env bash

parser=./cmake-build-debug/Parser

expect_error="--expect-error(s)"

function run_tests_in () {
    total=0
    tests_passed=0

    #对ill_formed文件夹中所有进行测试，并输出log
    for file in ${1}*.ml
    do
        logfile="${file}.log"
        total=`expr ${total} + 1`
        ${parser}  < ${file} 1> ${logfile} 2> /tmp/ast
        err=$(cat "/tmp/ast")
        #如果该文件未报错，则提示
        if [[ ${2} == expect_error ]]
        then
            if [[ "${#err}" == 0 ]]
            then
                echo "Expecting error(s) in ${file} while nothing reported."
            else
                tests_passed=`expr ${tests_passed} + 1`
            fi
         else
            if [[ "${#err}" != 0 ]]
            then
                echo err
                echo "Expecting no error in ${file} while error(s) reported."
            else
                tests_passed=`expr ${tests_passed} + 1`
            fi
         fi

    done

    echo "In ${1} directory, ${tests_passed}/${total} test(s) passed."

}

run_tests_in */ill_formed/ expect_error

run_tests_in */valid_programs/
