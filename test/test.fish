# Go to project root dir
cd dirname (status -f)/..;

make SAN=1 DEBUG=1
mkdir -p out

for i in (cat test/*.txt)
    echo $i
    
    ./build/bin/ft_nm-san  $i > out/a
    set st1 $status;
    nm $i > out/b
    set st2 $status;

    if test $st1 != $st2
        break
    end

    if test $st1 -eq 0
        diff -B out/a out/b
        if test "$status" != 0
            break
        end
    end
end
