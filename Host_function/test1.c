#include <wasmedge/wasmedge.h>
#include <stdio.h>
#include <mysql/mysql.h>

MYSQL mysql_conn; /* Connection handle */

typedef struct _wasmedge_mysql_context{
	char *host;
	char *user;
	char *passwd;
	char *db;
	//unsigned int32_t port; 
	//char unix_socket[];
	//unsigned long client_flag;
} wasmedge_mysql_context;


/* Host function body definition. */
WasmEdge_Result wasmedge_mysql_connect(void *Data, WasmEdge_MemoryInstanceContext *MemCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out){
        wasmedge_mysql_context *DataPtr = (wasmedge_mysql_context *)Data;
        //printf("get it!\n");
        //printf("%s\n",((*DataPtr).host));
	if( mysql_real_connect(&mysql_conn,(*DataPtr).host,(*DataPtr).user,
				(*DataPtr).passwd,(*DataPtr).db,MYSQL_PORT,NULL,0) != NULL)
	{
		printf("Connection success!\n");
	}
	else{
		printf("Connection fails!\n");
	}
	
	return WasmEdge_Result_Success;
}

WasmEdge_Result wasmedge_mysql_query(void *Data, WasmEdge_MemoryInstanceContext *MemCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out){
	
	int f1, f2, num_row, num_col;
	//char *ml = (char *)Data;
	if( mysql_query(&mysql_conn,Data) == 0)
	{
		MYSQL_RES *mysql_result; /* Result handle */
		MYSQL_ROW mysql_row; /* Row data */
		
		mysql_result = mysql_store_result(&mysql_conn);
		num_row = mysql_num_rows(mysql_result);
		/* Get the no. of row */
		num_col = mysql_num_fields(mysql_result);
		/* Get the no. of column */
		printf("row=%d, col=%d\n",num_row,num_col);

		for (f1 = 0; f1 < num_row; f1++)
		{
			mysql_row = mysql_fetch_row(mysql_result);
        		for (f2 = 0; f2 < num_col; f2++)
        		{
				/* Fetch one by one */
				printf("[Row %d, Col %d] ==> [%s]\n", f1, f2, mysql_row[f2]);
			}
		}
		mysql_free_result(mysql_result);
	}
	else
	{
		printf("Query fails\n");
	}
	
	
	return WasmEdge_Result_Success;
}


int main(int Argc, const char* Argv[]) {
	
	if(mysql_init(&mysql_conn)!=NULL)
	{
		/* Create the VM context. */
		WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(NULL, NULL);

		/* Create the import object. */
		WasmEdge_String ExportName = WasmEdge_StringCreateByCString("extern");
		WasmEdge_ImportObjectContext *ImpObj = WasmEdge_ImportObjectCreate(ExportName);
		enum WasmEdge_ValType ParamList[0] ;
		enum WasmEdge_ValType ReturnList[0] ;
		
		/* mysql-connect function*/
		wasmedge_mysql_context Data = {Argv[2],Argv[3],Argv[4],Argv[5]};
		WasmEdge_FunctionTypeContext *HostFType = WasmEdge_FunctionTypeCreate(ParamList, 0, ReturnList, 0);
		WasmEdge_FunctionInstanceContext *HostFunc = WasmEdge_FunctionInstanceCreate(HostFType, wasmedge_mysql_connect, &Data, 0);
		WasmEdge_FunctionTypeDelete(HostFType);
		WasmEdge_String HostFuncName = WasmEdge_StringCreateByCString("mysql-connect");
		WasmEdge_ImportObjectAddFunction(ImpObj, HostFuncName, HostFunc);
		WasmEdge_StringDelete(HostFuncName);
		
		/* mysql-query function*/
		//char *ml = "select * from tb_emp1";
		const char *ml = Argv[6];
		printf("%s\n",ml);
		//char *ml = "select";
		WasmEdge_FunctionTypeContext *HostFType1 = WasmEdge_FunctionTypeCreate(ParamList, 0, ReturnList, 0);
		WasmEdge_FunctionInstanceContext *HostFunc1 = WasmEdge_FunctionInstanceCreate(HostFType1, wasmedge_mysql_query, ml, 0);
		WasmEdge_FunctionTypeDelete(HostFType1);
		WasmEdge_String HostFuncName1 = WasmEdge_StringCreateByCString("mysql-query");
		WasmEdge_ImportObjectAddFunction(ImpObj, HostFuncName1, HostFunc1);
		WasmEdge_StringDelete(HostFuncName1);
		
		
		WasmEdge_VMRegisterModuleFromImport(VMCxt, ImpObj);

		/* The parameters and returns arrays. */
		WasmEdge_Value Params[0];
		WasmEdge_Value Returns[0];
		/* Function name. */
		WasmEdge_String FuncName = WasmEdge_StringCreateByCString("_start");
		/* Run the WASM function from file. */
		WasmEdge_Result Res = WasmEdge_VMRunWasmFromFile(
			VMCxt, Argv[1], FuncName, Params, 0, Returns, 0);

		if (WasmEdge_ResultOK(Res)) {
			printf("\nRuntime(c)=> OK\n");
		} 
		else {
			printf("\nRuntime(c)=> Error message: %s\n", WasmEdge_ResultGetMessage(Res));
		}

		/* Resources deallocations. */
		WasmEdge_VMDelete(VMCxt);
		WasmEdge_StringDelete(FuncName);
		WasmEdge_ImportObjectDelete(ImpObj);
	}
	else{
		printf("Initialization fails\n");
	}
	
	mysql_close(&mysql_conn);
	printf("quit\n");

	return 0;
}

