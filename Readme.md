# Tiger Compiler in C

This repo contains solutions accumulating from Chap1 to Chap12 of the book *Modern Compiler Implementation in C*.
The final compiler in under master.

## Author

bajzc: Chap1-12

WilliamQiufeng: Chap8-12

## Insight

We implemented nearly all features mentioned in the book, including type checking, static link and a complete graph coloring
algorithm. Not much testing was done other than running the testcases from the
[book](https://www.cs.princeton.edu/~appel/modern/testcases/).

We chose RISC-V as our target machine to be able to run on [xv6](https://github.com/bajzc/xv6-riscv). We wish to not only run the compiler's output, but the compiler itself on xv6, which is one of the main reasons this project is written in C.

## Try it
You need to have the necessary build tools installed on your system, including bison, flex, make (GNU) and gcc

```shell
git clone https://github.com/bajzc/tiger-in-c.git
cd tiger-in-c
cmake -B build -G 'Unix Makefiles'
make -C build tigerc
./build/tigerc
```

And the compiler named ``a.out`` will be inside the top dir, here is a sample program you can try:
```tiger
/* A program to solve the 8-queens problem */
let
  var N := 8

  type intArray = array of int

  var row := intArray [ N ] of 0
  var col := intArray [ N ] of 0
  var diag1 := intArray [N+N-1] of 0
  var diag2 := intArray [N+N-1] of 0

  function printboard() =
    (for i := 0 to N-1
     do (for j := 0 to N-1
         do print(if col[i]=j then " O" else " .");
         print("\n"));
     print("\n"))

  function try(c: int) =
    ( /*  for i:= 0 to c do print("."); print("\n"); flush();*/
    if c=N
    then printboard()
    else for r := 0 to N-1
         do if row[r]=0 & diag1[r+c]=0 & diag2[r+7-c]=0
            then (row[r] := 1; diag1[r+c] := 1; diag2[r+7-c] := 1;
                  col[c] := r;
                  try(c+1);
                  row[r] := 0; diag1[r+c] := 0; diag2[r+7-c] := 0)

    )
in try(0)
end
```

Copy it to a file somewhere, and compile it!
```shell
./tigerc path/to/the/program
```

And now you should have an assembly file like ``queen.tig.s``. However, this is not the file you can directly
execute out of the box. You might want to find some instructions about how to run it here: [xv6](https://github.com/bajzc/xv6-riscv)

Output from running ``queen.tig.s``:
```
 O . . . . . . .
 . . . . O . . .
 . . . . . . . O
 . . . . . O . .
 . . O . . . . .
 . . . . . . O .
 . O . . . . . .
 . . . O . . . .

 O . . . . . . .
 . . . . . O . .
 . . . . . . . O
 . . O . . . . .
 . . . . . . O .
 . . . O . . . .
 . O . . . . . .
 . . . . O . . .

 O . . . . . . .
 . . . . . . O .
 . . . O . . . .
 . . . . . O . .
 . . . . . . . O
 . O . . . . . .
 . . . . O . . .
 . . O . . . . .
 
...
```

### Note

This project is still under development for advanced topics (Garbage Collection, SSA, OOP and FP...)

## License
GPL-3
