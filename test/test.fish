# Go to project root dir
cd (dirname (status -f))/..;

mkdir -p out

for i in test/*.txt
    for i in (cat $i)
        echo -n " + nm: $i: "

        $argv[1] $i >out/a 2>/dev/null; set st1 $status
        $argv[2] $argv[3] $i >out/b 2>/dev/null; set st2 $status

        if test $st1 != $st2
            set_color red; echo "KO ($st1 != $st2)"; set_color normal
            read -n1 ans
        else if test $st1 -eq 0
            diff -w out/a out/b > out/diff
            if test "$status" != 0
                set_color red; echo "KO"; set_color normal
                cat out/diff
                read -n1 ans
            else
                set_color green; echo "OK"; set_color normal
            end
        else
            set_color green; echo "OK"; set_color normal
        end
    end
end
