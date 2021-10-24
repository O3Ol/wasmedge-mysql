    (module
      (type $t0 (func ))
      (import "extern" "mysql-connect" (func $m-connect (type $t0)))
      (import "extern" "mysql-query" (func $m-query (type $t0)))
      (func (export "_start") 
        call $m-connect
        call $m-query
        )
    )
