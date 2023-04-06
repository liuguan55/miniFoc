cppcheck -j8 --template="{severity}\t{file}:{line}\t{id}: {message}" --enable=all ../src/ --driver-file=cppcheck_res.txt --suppress=unusedFunction --suppress=preprocessorErrorDirective --force
