## 作业

`x86.py` 这个程序让你看到不同的线程交替如何导致或避免竞态条件。
请参阅 `README` 文件，了解程序如何工作及其基本输入的详细信息，然后回答以下问题。



## 问题

1. 开始，我们来看一个简单的程序，“loop.s”。
   首先，阅读这个程序，看看你是否能理解它：cat loop.s。
   然后，用这些参数运行它： 

   `./x86.py -p loop.s -t 1 -i 100 -R dx` 

   这指定了一个单线程，每 100 条指令产生一个中断，并且追踪寄存器%dx。
   你能弄清楚 %dx 在运行过程中的值吗？
   你有答案之后，运行上面的代码并使用-c 标志来检查你的答案。
   注意答案的左边显示了右侧指令运行后寄存器的值（或内存的值）。

```shell
./x86.py -p loop.s -t 1 -i 100 -R dx 

   dx          Thread 0
    0
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
```



2. 现在运行相同的代码，但使用这些标志：
   `./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx `
   这指定了两个线程，并将每个%dx 寄存器初始化为 3。
   %dx 会看到什么值？使用-c 标志运行以查看答案。
   多个线程的存在是否会影响计算？这段代码有竞态条件吗？



dx = -1；

多个线程的存在不会影响计算，这段代码没有竟态条件。

```shell
   dx          Thread 0                Thread 1
    3
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    1                            1000 sub  $1,%dx
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1                            1002 jgte .top
   -1                            1003 halt
```



3. 现在运行以下命令：
   `./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx `
   这使得中断间隔非常小且随机。使用不同的种子和-s 来查看不同的交替。
   中断频率是否会改变这个程序的行为？



中断频率不改变这个程序的行为。

改变种子和不同seed值：

- `-r -s 10 `，dx=-1
- `-r -s 23`，dx=-1

```shell
   dx          Thread 0                Thread 1
    3
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    3   ------ Interrupt ------  ------ Interrupt ------
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    2   ------ Interrupt ------  ------ Interrupt ------
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    2   ------ Interrupt ------  ------ Interrupt ------
    1                            1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    0                            1000 sub  $1,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1   1001 test $0,%dx
   -1   1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1   1003 halt
    0   ----- Halt;Switch -----  ----- Halt;Switch -----
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1                            1002 jgte .top
   -1                            1003 halt
```



4. 接下来我们将研究一个不同的程序（looping-race-nolock.s）。
   **该程序访问位于内存地址 2000 的共享变量。**
   简单起见，我们称这个变量为 x。
   使用单线程运行它，并确保你了解它的功能，如下所示：
   `./x86.py -p looping-race-nolock.s -t 1 -M 2000 `
   在整个运行过程中，x（即内存地址为 2000）的值是多少？使用-c 来检查你的答案。

```shell
 2000          Thread 0
    0
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1006 halt
```



5. 现在运行多个迭代和线程：
   `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000 `
   你明白为什么每个线程中的代码循环 3 次吗？x 的最终值是什么？



为什么循环3次？

因为线程 bx 每次 -1，要减三次才为0，等于 0 时才跳出循环；

x 的最终值是6（查看参数，此时的 `interrupt frequency=50`），即三次循环中未发生中断导致的线程切换。

```shell
 2000          Thread 0                Thread 1
    0
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1000 mov 2000, %ax
    1   1001 add $1, %ax
    2   1002 mov %ax, 2000
    2   1003 sub  $1, %bx
    2   1004 test $0, %bx
    2   1005 jgt .top
    2   1000 mov 2000, %ax
    2   1001 add $1, %ax
    3   1002 mov %ax, 2000
    3   1003 sub  $1, %bx
    3   1004 test $0, %bx
    3   1005 jgt .top
    3   1006 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----
    3                            1000 mov 2000, %ax
    3                            1001 add $1, %ax
    4                            1002 mov %ax, 2000
    4                            1003 sub  $1, %bx
    4                            1004 test $0, %bx
    4                            1005 jgt .top
    4                            1000 mov 2000, %ax
    4                            1001 add $1, %ax
    5                            1002 mov %ax, 2000
    5                            1003 sub  $1, %bx
    5                            1004 test $0, %bx
    5                            1005 jgt .top
    5                            1000 mov 2000, %ax
    5                            1001 add $1, %ax
    6                            1002 mov %ax, 2000
    6                            1003 sub  $1, %bx
    6                            1004 test $0, %bx
    6                            1005 jgt .top
    6                            1006 halt
```



6. 现在以随机中断间隔运行：
   `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0`
   然后改变随机种子，设置-s 1，然后-s 2 等。
   只看线程交替，你能说出 x 的最终值是什么吗？
   中断的确切位置是否重要？在哪里发生是安全的？中断在哪里会引起麻烦？
   换句话说，临界区究竟在哪里？



只看线程交替，是可以分析出 x 的最终值的。
中断的确切位置很重要。

```assembly
# assumes %bx has loop count in it

.main
.top
# critical section
mov 2000, %ax  # get 'value' at address 2000
add $1, %ax    # increment it
mov %ax, 2000  # store it back

# see if we're still looping
sub  $1, %bx
test $0, %bx
jgt .top

halt
```

中断发生在两个 mov 之间就可能引起麻烦，其余的地方是安全的。



7. 现在使用固定的中断间隔来进一步探索程序。运行：
   `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1 `
   看看你能否猜测共享变量 x 的最终值是什么。当你改用-i 2，-i 3 等标志呢？
   对于哪个中断间隔，程序会给出“正确的”最终答案？



​		i=1, x=1;

​		i=2,x=1;

​		i=3,x=2;



8. 现在为更多循环运行相同的代码（例如 set -a bx = 100）。
   使用-i 标志设置哪些中断 间隔会导致“正确”结果？
   哪些间隔会导致令人惊讶的结果？



​		3的倍数间隔会得到正确结果。



9. 我们来看本作业中最后一个程序（wait-for-me.s）。
   像这样运行代码：
   `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 `
   这将线程 0 的%ax 寄存器设置为 1，并将线程 1 的值设置为 0，在整个运行过程中观察%ax 和内存位置 2000 的值。
   代码的行为应该如何？线程使用的 2000 位置的值如何？它的最终值是什么？



​		线程0 将内存2000设置为1，然后停止；
​		线程1 一直循环运行，直到内存2000为1。



10. 现在改变输入：
    `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 `
    线程行为如何？线程 0 在做什么？
    改变中断间隔（例如，-i 1000，或者可能使用随机间隔）会如何改变追踪结果？程序是否高效地使用了 CPU？



​		现在线程0继续循环运行，等待线程1将内存2000更改为1。
​		程序没有高效使用 CPU，花了很多时间在不停的 test

