# Go to project root dir
cd dirname (status -f)/..;

make SAN=1 DEBUG=1
mkdir -p out

for i in (cat test/*.txt)
    echo $i
    
    ./build/bin/ft_nm-san  $i > out/a
    nm $i > out/b

    diff -B out/a out/b
    if test "$status" != 0
        break
    end
end
