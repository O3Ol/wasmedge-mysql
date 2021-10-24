# Example
1. Compile `sqlite_host.wat` to `sqlite_host.wasm` as following:
    ```bash
    $ wat2wasm mysql.wat -o mysql.wasm
    
    ```

2. Compile the 'test1.c' file to 'test.out' and run the executable file:
    ```bash
    $ gcc test1.c -lwasmedge_c -lmysqlclient -o test.out
    $ ./test.out mysql.wasm [mysql.host] [mysql.user] [mysql.passwd] [mysql.database] "[the query command]"
    
    ```
    
    And in my localhost, I run and get the result such as following:
    ```bash
    $ gcc test1.c -lwasmedge_c -lmysqlclient -o test.out
    $ ./test.out mysql.wasm localhost wcy 123456 mydb "select * from tb_emp1"
    select * from tb_emp1
    Connection success!
    row=1, col=4
    [Row 0, Col 0] ==> [1]
    [Row 0, Col 1] ==> [李四]
    [Row 0, Col 2] ==> [18]
    [Row 0, Col 3] ==> [5432.1]
    
    Runtime(c)=> OK
    quit
    
    ```
