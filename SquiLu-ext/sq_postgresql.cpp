#ifdef WITH_POSTGRESQL

#include "squirrel.h"
#include "libpq-fe.h"
//#include "pg_type.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

#define BOOLOID 16
#define BYTEAOID 17
#define CHAROID 18
#define INT8OID 20
#define INT2OID 21
#define INT4OID 23
#define FLOAT4OID 700
#define FLOAT8OID 701
#define TIMESTAMPOID 1114
#define TIMESTAMPTZOID 1184
#define VARCHAROID 1043

#include "dynamic_library.h"

/*SquiLu
local pgsql_functions = [
    ["ConnStatusType", "PQstatus", "const PGconn *conn"],
    ["void", "PQfinish", "PGconn *conn"],
    ["PGresult *", "PQprepare", @"PGconn *conn,
                        const char *stmtName,
                        const char *query,
                        int nParams,
                        const Oid *paramTypes"],
    ["void", "PQfreemem", "void *ptr"],
    ["char *", "PQescapeLiteral", "PGconn *conn, const char *str, size_t length"],
    ["char *", "PQescapeIdentifier", "PGconn *conn, const char *str, size_t length"],
    ["unsigned char *", "PQescapeByteaConn", "PGconn *conn, const unsigned char *str, size_t length"],
    ["unsigned char *", "PQunescapeBytea", "const unsigned char *str, size_t length"],
    ["PGresult *", "PQdescribePrepared", "PGconn *conn, const char *stmtName"],
    ["int", "PQnparams", "const PGresult *res"],
    ["Oid", "PQparamtype", "const PGresult *res, int param_number"],
    ["PGresult *", "PQexecPrepared", @"PGconn *conn,
                         const char *stmtName,
                         int nParams,
                         const char * const *paramValues,
                         const int *paramLengths,
                         const int *paramFormats,
                         int resultFormat"],
    ["PGresult *", "PQexec", "PGconn *conn, const char *command"],
    ["PGresult *", "PQexecParams", @"PGconn *conn,
                       const char *command,
                       int nParams,
                       const Oid *paramTypes,
                       const char * const *paramValues,
                       const int *paramLengths,
                       const int *paramFormats,
                       int resultFormat"],
    ["const char*", "PQgetvalue", @"const PGresult *res,
                         int row_number,
                         int column_number"],
    ["int", "PQntuples", "const PGresult *res"],
    ["char *", "PQcmdTuples", "const PGresult *res"],
    ["int", "PQnfields", "const PGresult *res"],
    ["int", "PQgetisnull", @"const PGresult *res,
                        int row_number,
                        int column_number"],
    ["void", "PQclear", "const PGresult *res"],
    ["int", "PQfnumber", "const PGresult *res, const char *column_name"],
    ["char *", "PQfname", "const PGresult *res, int column_number"],
    ["ExecStatusType", "PQresultStatus", "const PGresult *res"],
    ["char *", "PQerrorMessage", "const PGconn *conn"],
    ["void", "PQreset", "PGconn *conn"],
    ["PGresult *", "PQgetResult", "PGconn *conn"],
    ["int", "PQsetnonblocking", "PGconn *conn, int arg"],
    ["Oid", "PQftype", "const PGresult *res, int column_number"],
    ["int", "PQserverVersion", "const PGconn *conn"],

    // Large-object access routines
    ["int", "lo_open", "PGconn *conn, Oid lobjId, int mode"],
    ["int", "lo_close", "PGconn *conn, int fd"],
    ["int", "lo_read", "PGconn *conn, int fd, char *buf, size_t len"],
    ["int", "lo_write", "PGconn *conn, int fd, const char *buf, size_t len"],
    ["int", "lo_lseek", "PGconn *conn, int fd, int offset, int whence"],
    ["Oid",	"lo_creat", "PGconn *conn, int mode"],
    ["Oid",	"lo_create", "PGconn *conn, Oid lobjId"],
    ["int", "lo_tell", "PGconn *conn, int fd"],
    ["int", "lo_truncate", "PGconn *conn, int fd, size_t len"],
    ["int", "lo_unlink", "PGconn *conn, Oid lobjId"],
    ["Oid",	"lo_import", "PGconn *conn, const char *filename"],
    ["Oid",	"lo_import_with_oid", "PGconn *conn, const char *filename, Oid lobjId"],
    ["int", "lo_export", "PGconn *conn, Oid lobjId, const char *filename"],

    //next entry should be the last one
    //to make valid the test made on load_libpq function
    ["PGconn *", "PQconnectdb", "const char *conninfo"],
];

function write_pgsql_functions_declaration(){
    foreach(k,v in pgsql_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_pgsql_functions_load(){
    foreach(k,v in pgsql_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libpq.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libpq;

//@write_pgsql_functions_declaration();
// generated-code:begin
typedef ConnStatusType (*PQstatus_t)(const PGconn *conn);
static PQstatus_t dlPQstatus = 0;
typedef void (*PQfinish_t)(PGconn *conn);
static PQfinish_t dlPQfinish = 0;
typedef PGresult * (*PQprepare_t)(PGconn *conn,
                        const char *stmtName,
                        const char *query,
                        int nParams,
                        const Oid *paramTypes);
static PQprepare_t dlPQprepare = 0;
typedef void (*PQfreemem_t)(void *ptr);
static PQfreemem_t dlPQfreemem = 0;
typedef char * (*PQescapeLiteral_t)(PGconn *conn, const char *str, size_t length);
static PQescapeLiteral_t dlPQescapeLiteral = 0;
typedef char * (*PQescapeIdentifier_t)(PGconn *conn, const char *str, size_t length);
static PQescapeIdentifier_t dlPQescapeIdentifier = 0;
typedef unsigned char * (*PQescapeByteaConn_t)(PGconn *conn, const unsigned char *str, size_t length);
static PQescapeByteaConn_t dlPQescapeByteaConn = 0;
typedef unsigned char * (*PQunescapeBytea_t)(const unsigned char *str, size_t length);
static PQunescapeBytea_t dlPQunescapeBytea = 0;
typedef PGresult * (*PQdescribePrepared_t)(PGconn *conn, const char *stmtName);
static PQdescribePrepared_t dlPQdescribePrepared = 0;
typedef int (*PQnparams_t)(const PGresult *res);
static PQnparams_t dlPQnparams = 0;
typedef Oid (*PQparamtype_t)(const PGresult *res, int param_number);
static PQparamtype_t dlPQparamtype = 0;
typedef PGresult * (*PQexecPrepared_t)(PGconn *conn,
                         const char *stmtName,
                         int nParams,
                         const char * const *paramValues,
                         const int *paramLengths,
                         const int *paramFormats,
                         int resultFormat);
static PQexecPrepared_t dlPQexecPrepared = 0;
typedef PGresult * (*PQexec_t)(PGconn *conn, const char *command);
static PQexec_t dlPQexec = 0;
typedef PGresult * (*PQexecParams_t)(PGconn *conn,
                       const char *command,
                       int nParams,
                       const Oid *paramTypes,
                       const char * const *paramValues,
                       const int *paramLengths,
                       const int *paramFormats,
                       int resultFormat);
static PQexecParams_t dlPQexecParams = 0;
typedef const char* (*PQgetvalue_t)(const PGresult *res,
                         int row_number,
                         int column_number);
static PQgetvalue_t dlPQgetvalue = 0;
typedef int (*PQntuples_t)(const PGresult *res);
static PQntuples_t dlPQntuples = 0;
typedef char * (*PQcmdTuples_t)(const PGresult *res);
static PQcmdTuples_t dlPQcmdTuples = 0;
typedef int (*PQnfields_t)(const PGresult *res);
static PQnfields_t dlPQnfields = 0;
typedef int (*PQgetisnull_t)(const PGresult *res,
                        int row_number,
                        int column_number);
static PQgetisnull_t dlPQgetisnull = 0;
typedef void (*PQclear_t)(const PGresult *res);
static PQclear_t dlPQclear = 0;
typedef int (*PQfnumber_t)(const PGresult *res, const char *column_name);
static PQfnumber_t dlPQfnumber = 0;
typedef char * (*PQfname_t)(const PGresult *res, int column_number);
static PQfname_t dlPQfname = 0;
typedef ExecStatusType (*PQresultStatus_t)(const PGresult *res);
static PQresultStatus_t dlPQresultStatus = 0;
typedef char * (*PQerrorMessage_t)(const PGconn *conn);
static PQerrorMessage_t dlPQerrorMessage = 0;
typedef void (*PQreset_t)(PGconn *conn);
static PQreset_t dlPQreset = 0;
typedef PGresult * (*PQgetResult_t)(PGconn *conn);
static PQgetResult_t dlPQgetResult = 0;
typedef int (*PQsetnonblocking_t)(PGconn *conn, int arg);
static PQsetnonblocking_t dlPQsetnonblocking = 0;
typedef Oid (*PQftype_t)(const PGresult *res, int column_number);
static PQftype_t dlPQftype = 0;
typedef int (*PQserverVersion_t)(const PGconn *conn);
static PQserverVersion_t dlPQserverVersion = 0;
typedef int (*lo_open_t)(PGconn *conn, Oid lobjId, int mode);
static lo_open_t dllo_open = 0;
typedef int (*lo_close_t)(PGconn *conn, int fd);
static lo_close_t dllo_close = 0;
typedef int (*lo_read_t)(PGconn *conn, int fd, char *buf, size_t len);
static lo_read_t dllo_read = 0;
typedef int (*lo_write_t)(PGconn *conn, int fd, const char *buf, size_t len);
static lo_write_t dllo_write = 0;
typedef int (*lo_lseek_t)(PGconn *conn, int fd, int offset, int whence);
static lo_lseek_t dllo_lseek = 0;
typedef Oid (*lo_creat_t)(PGconn *conn, int mode);
static lo_creat_t dllo_creat = 0;
typedef Oid (*lo_create_t)(PGconn *conn, Oid lobjId);
static lo_create_t dllo_create = 0;
typedef int (*lo_tell_t)(PGconn *conn, int fd);
static lo_tell_t dllo_tell = 0;
typedef int (*lo_truncate_t)(PGconn *conn, int fd, size_t len);
static lo_truncate_t dllo_truncate = 0;
typedef int (*lo_unlink_t)(PGconn *conn, Oid lobjId);
static lo_unlink_t dllo_unlink = 0;
typedef Oid (*lo_import_t)(PGconn *conn, const char *filename);
static lo_import_t dllo_import = 0;
typedef Oid (*lo_import_with_oid_t)(PGconn *conn, const char *filename, Oid lobjId);
static lo_import_with_oid_t dllo_import_with_oid = 0;
typedef int (*lo_export_t)(PGconn *conn, Oid lobjId, const char *filename);
static lo_export_t dllo_export = 0;
typedef PGconn * (*PQconnectdb_t)(const char *conninfo);
static PQconnectdb_t dlPQconnectdb = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libpq);

static bool load_libpq(const char *libname)
{
    if(dlPQconnectdb) return true;
    if(libpq.open(libname))
    {
        //@write_pgsql_functions_load();
// generated-code:begin
dlPQstatus = (PQstatus_t) libpq.dlsym("PQstatus");
if(!dlPQstatus) return false;
dlPQfinish = (PQfinish_t) libpq.dlsym("PQfinish");
if(!dlPQfinish) return false;
dlPQprepare = (PQprepare_t) libpq.dlsym("PQprepare");
if(!dlPQprepare) return false;
dlPQfreemem = (PQfreemem_t) libpq.dlsym("PQfreemem");
if(!dlPQfreemem) return false;
dlPQescapeLiteral = (PQescapeLiteral_t) libpq.dlsym("PQescapeLiteral");
if(!dlPQescapeLiteral) return false;
dlPQescapeIdentifier = (PQescapeIdentifier_t) libpq.dlsym("PQescapeIdentifier");
if(!dlPQescapeIdentifier) return false;
dlPQescapeByteaConn = (PQescapeByteaConn_t) libpq.dlsym("PQescapeByteaConn");
if(!dlPQescapeByteaConn) return false;
dlPQunescapeBytea = (PQunescapeBytea_t) libpq.dlsym("PQunescapeBytea");
if(!dlPQunescapeBytea) return false;
dlPQdescribePrepared = (PQdescribePrepared_t) libpq.dlsym("PQdescribePrepared");
if(!dlPQdescribePrepared) return false;
dlPQnparams = (PQnparams_t) libpq.dlsym("PQnparams");
if(!dlPQnparams) return false;
dlPQparamtype = (PQparamtype_t) libpq.dlsym("PQparamtype");
if(!dlPQparamtype) return false;
dlPQexecPrepared = (PQexecPrepared_t) libpq.dlsym("PQexecPrepared");
if(!dlPQexecPrepared) return false;
dlPQexec = (PQexec_t) libpq.dlsym("PQexec");
if(!dlPQexec) return false;
dlPQexecParams = (PQexecParams_t) libpq.dlsym("PQexecParams");
if(!dlPQexecParams) return false;
dlPQgetvalue = (PQgetvalue_t) libpq.dlsym("PQgetvalue");
if(!dlPQgetvalue) return false;
dlPQntuples = (PQntuples_t) libpq.dlsym("PQntuples");
if(!dlPQntuples) return false;
dlPQcmdTuples = (PQcmdTuples_t) libpq.dlsym("PQcmdTuples");
if(!dlPQcmdTuples) return false;
dlPQnfields = (PQnfields_t) libpq.dlsym("PQnfields");
if(!dlPQnfields) return false;
dlPQgetisnull = (PQgetisnull_t) libpq.dlsym("PQgetisnull");
if(!dlPQgetisnull) return false;
dlPQclear = (PQclear_t) libpq.dlsym("PQclear");
if(!dlPQclear) return false;
dlPQfnumber = (PQfnumber_t) libpq.dlsym("PQfnumber");
if(!dlPQfnumber) return false;
dlPQfname = (PQfname_t) libpq.dlsym("PQfname");
if(!dlPQfname) return false;
dlPQresultStatus = (PQresultStatus_t) libpq.dlsym("PQresultStatus");
if(!dlPQresultStatus) return false;
dlPQerrorMessage = (PQerrorMessage_t) libpq.dlsym("PQerrorMessage");
if(!dlPQerrorMessage) return false;
dlPQreset = (PQreset_t) libpq.dlsym("PQreset");
if(!dlPQreset) return false;
dlPQgetResult = (PQgetResult_t) libpq.dlsym("PQgetResult");
if(!dlPQgetResult) return false;
dlPQsetnonblocking = (PQsetnonblocking_t) libpq.dlsym("PQsetnonblocking");
if(!dlPQsetnonblocking) return false;
dlPQftype = (PQftype_t) libpq.dlsym("PQftype");
if(!dlPQftype) return false;
dlPQserverVersion = (PQserverVersion_t) libpq.dlsym("PQserverVersion");
if(!dlPQserverVersion) return false;
dllo_open = (lo_open_t) libpq.dlsym("lo_open");
if(!dllo_open) return false;
dllo_close = (lo_close_t) libpq.dlsym("lo_close");
if(!dllo_close) return false;
dllo_read = (lo_read_t) libpq.dlsym("lo_read");
if(!dllo_read) return false;
dllo_write = (lo_write_t) libpq.dlsym("lo_write");
if(!dllo_write) return false;
dllo_lseek = (lo_lseek_t) libpq.dlsym("lo_lseek");
if(!dllo_lseek) return false;
dllo_creat = (lo_creat_t) libpq.dlsym("lo_creat");
if(!dllo_creat) return false;
dllo_create = (lo_create_t) libpq.dlsym("lo_create");
if(!dllo_create) return false;
dllo_tell = (lo_tell_t) libpq.dlsym("lo_tell");
if(!dllo_tell) return false;
dllo_truncate = (lo_truncate_t) libpq.dlsym("lo_truncate");
if(!dllo_truncate) return false;
dllo_unlink = (lo_unlink_t) libpq.dlsym("lo_unlink");
if(!dllo_unlink) return false;
dllo_import = (lo_import_t) libpq.dlsym("lo_import");
if(!dllo_import) return false;
dllo_import_with_oid = (lo_import_with_oid_t) libpq.dlsym("lo_import_with_oid");
if(!dllo_import_with_oid) return false;
dllo_export = (lo_export_t) libpq.dlsym("lo_export");
if(!dllo_export) return false;
dlPQconnectdb = (PQconnectdb_t) libpq.dlsym("PQconnectdb");
if(!dlPQconnectdb) return false;
// generated-code:end

        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static const SQChar *PostgreSQL_TAG = _SC("PostgreSQL");

static SQRESULT get_pgsql_instance(HSQUIRRELVM v, SQInteger idx, PGconn **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_TAG,SQFalse)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("database is closed"));
	return _rc_;
}

#define GET_pgsql_INSTANCE_AT(idx) \
	PGconn *self=NULL; \
	if((_rc_ = get_pgsql_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_INSTANCE() GET_pgsql_INSTANCE_AT(1)

static const SQChar *PostgreSQL_Result_TAG = _SC("PostgreSQL_Result");
static const SQChar *_curr_row_key = _SC("_curr_row");

static SQRESULT get_pgsql_result_instance(HSQUIRRELVM v, SQInteger idx, PGresult **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_Result_TAG,SQFalse)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("PGresult is closed"));
	return _rc_;
}

#define GET_pgsql_result_INSTANCE_AT(idx) \
	PGresult *self=NULL; \
	if((_rc_ = get_pgsql_result_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_result_INSTANCE() GET_pgsql_result_INSTANCE_AT(1)

static SQRESULT sq_pgsql_result_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	PGresult *self = ((PGresult *)p);
	if (self) dlPQclear(self);
	return 0;
}

static SQRESULT sq_pgsql_result_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	dlPQclear(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgresult is closed"
	return 0;
}

static SQRESULT sq_pgsql_result_col_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	sq_pushinteger(v, dlPQnfields(self));
	return 1;
}

static SQRESULT sq_pgsql_result_row_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	sq_pushinteger(v, dlPQntuples(self));
	return 1;
}

static SQRESULT sq_pgsql_result_col_name(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQ_GET_INTEGER(v, 2, col);
	sq_pushstring(v, dlPQfname(self, col), -1);
	return 1;
}

static SQRESULT sq_pgsql_result_col_type(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQ_GET_INTEGER(v, 2, col);
	sq_pushinteger(v, dlPQftype(self, col));
	return 1;
}

static SQRESULT sq_pgsql_result_col_index(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQ_GET_STRING(v, 2, name);
	sq_pushinteger(v, dlPQfnumber(self, name));
	return 1;
}

static SQRESULT sq_pgsql_result_eof(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        sq_pushbool(v, curr_row < dlPQntuples(self));
    }
	else sq_pushbool(v, SQTrue);
	return 1;
}

static SQRESULT sq_pgsql_result_next_row(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    sq_push(v, -1); //make a copy
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(++curr_row < dlPQntuples(self)){
            sq_poptop(v);
            sq_pushinteger(v, curr_row);
            sq_set(v, 1);
            sq_pushbool(v, SQTrue);
            return 1;
        }
    }
	sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_pgsql_result_col_value(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQObjectType ptype = sq_gettype(v, 2);
	int col = -1;
	if(ptype == OT_STRING){
	    SQ_GET_STRING(v, 2, col_name);
	    col = dlPQfnumber(self, col_name);
	}
	else
	{
	    SQ_GET_INTEGER(v, 2, idx);
	    col = idx;
	}
	if(col < 0) return sq_throwerror(v, _SC("invalid col index/name"));

    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(curr_row < dlPQntuples(self)){
            sq_pushstring(v, dlPQgetvalue(self, curr_row, col), -1);
            return 1;
        }
    }
	return SQ_ERROR;
}

static SQRESULT sq_pgsql_result_row_as_array(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_pgsql_result_INSTANCE();
	SQ_OPT_INTEGER(v, 2, row, -1);
    if(row < 0){
        sq_pushstring(v, _curr_row_key, -1);
        if(sq_get(v, 1) == SQ_OK){
            sq_getinteger(v, -1, &row);
        }
    }
    int row_count = dlPQntuples(self);
    if(row < 0 || row >= row_count) return sq_throwerror(v, _SC("invalid row (" _PRINT_INT_FMT ")"), row);

    int col_count = dlPQnfields(self);
    sq_newarray(v, col_count);
    for(int i=0; i < col_count; ++i){
        sq_pushinteger(v, i);
        sq_pushstring(v, dlPQgetvalue(self, row, i), -1);
        sq_rawset(v, -3);
    }
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_result_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_result_methods[] =
{
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(eof,  1, _SC("x")),
	_DECL_FUNC(next_row,  1, _SC("x")),
	_DECL_FUNC(col_count,  1, _SC("x")),
	_DECL_FUNC(row_count,  1, _SC("x")),
	_DECL_FUNC(col_name,  2, _SC("xi")),
	_DECL_FUNC(col_index,  2, _SC("xs")),
	_DECL_FUNC(col_value,  2, _SC("x i|s")),
	_DECL_FUNC(col_type,  2, _SC("x i|s")),
	_DECL_FUNC(row_as_array,  -1, _SC("xi")),
	{0,0}
};
#undef _DECL_FUNC

struct PgSqlStatement {
    PGconn *db;
    char name[64];
    int param_count;
    int isGetPrepared;
	void **param_values;
	int *param_sizes;
	int *param_types;
};

static const SQChar *PostgreSQL_Statement_TAG = _SC("PostgreSQL_Statement");

static SQRESULT get_pgsql_statement_instance(HSQUIRRELVM v, SQInteger idx, PgSqlStatement **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_Statement_TAG,SQFalse)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("PGstatement is closed"));
	return _rc_;
}

#define GET_pgsql_statement_INSTANCE_AT(idx) \
	PgSqlStatement *self=NULL; \
	if((_rc_ = get_pgsql_statement_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_statement_INSTANCE() GET_pgsql_statement_INSTANCE_AT(1)

static SQRESULT sq_pgsql_statement_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	PgSqlStatement *self = ((PgSqlStatement *)p);
	if (self){
        if(!self->isGetPrepared)
        {
            char sql[128];
            snprintf(sql, sizeof(sql), "DEALLOCATE %s", self->name);
            PGresult *qres = dlPQexec(self->db, sql);
            dlPQclear(qres);
        }
        if(self->param_count)
        {
            SQUnsignedInteger the_size;
            if(self->param_values)
            {
                    the_size = self->param_count * sizeof(self->param_values);
                    sq_free(self->param_values, the_size);
            }
            if(self->param_sizes)
            {
                    the_size = self->param_count * sizeof(self->param_sizes);
                    sq_free(self->param_sizes, the_size);
            }
            if(self->param_types)
            {
                    the_size = self->param_count * sizeof(self->param_types);
                    sq_free(self->param_types, the_size);
            }
        }
        sq_free(self, sizeof(PgSqlStatement));
	}
	return 0;
}

static SQRESULT sq_pgsql_statement_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_statement_INSTANCE();
	sq_pgsql_statement_releasehook(self, 0, v);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgstatement is closed"
	return 0;
}

/*
static SQRESULT sq_pgsql_statement_exec(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_pgsql_statement_INSTANCE();
	SQ_OPT_INTEGER(v, 3, result_type, 0);
	SQInteger psize = sq_getsize(v, 2);
	void **param_values;
	int *param_sizes;
	int *param_types;
	bool bresult = false;
	SQBool bval;
	PGresult *qres;
    SQUnsignedInteger param_values_size = psize * sizeof(param_values);
    SQUnsignedInteger param_sizes_size = psize * sizeof(param_sizes);

	if(self->result)
    {
        dlPQclear(self->result);
        self->result = NULL;
    }

	param_values = (void **)sq_malloc(param_values_size);
    memset(param_values, 0, param_values_size);
	param_sizes = (int *)sq_malloc(param_sizes_size);
    memset(param_sizes, 0, param_sizes_size);
	param_types = (int *)sq_malloc(param_sizes_size);
    memset(param_types, 0, param_sizes_size);

    for(SQInteger i=0; i < psize; ++i)
    {
        sq_pushinteger(v, i);
        if(sq_get(v, 2) == SQ_OK)
        {
            switch(sq_gettype(v, -1))
            {
            case OT_NULL:
                param_values[i] = NULL;
                param_sizes[i] = 0;
                param_types[i] = 0;
                sq_poptop(v);
                break;
            case OT_BOOL:
                sq_getbool(v, -1, &bval);
                param_values[i] = (void*)(bval == SQTrue ? "1" : "0");
                param_sizes[i] = 1;
                param_types[i] = BOOLOID;
                sq_poptop(v);
                break;
            case OT_INTEGER:
                param_values[i] = (void*)sq_tostring(v, -1);
                param_sizes[i] = (int)sq_getsize(v, -1);
                param_types[i] = INT4OID;
                break;
            case OT_FLOAT:
                param_values[i] = (void*)sq_tostring(v, -1);
                param_sizes[i] = (int)sq_getsize(v, -1);
                param_types[i] = FLOAT8OID;
                break;
            case OT_STRING:
                param_values[i] = (void*)sq_tostring(v, -1);
                param_sizes[i] = (int)sq_getsize(v, -1);
                param_types[i] = VARCHAROID;
                break;
            default:

                goto cleanup;
            }
        }
    }

	qres = dlPQexecPrepared(self->db, self->name, psize, (const char**)param_values, param_sizes, NULL, result_type);
	bresult = dlPQresultStatus(qres) == PGRES_COMMAND_OK;
	if(bresult) self->result = qres;
	else dlPQclear(qres);
cleanup:
	sq_free(param_values, param_values_size);
	sq_free(param_sizes, param_sizes_size);
	sq_settop(v, _top_);
	sq_pushbool(v, bresult);
	return 1;
}
*/

#define SQ_EXEC_DML 1
#define SQ_EXEC_SCALAR 2
#define SQ_EXEC_QUERY 3

/*
static SQRESULT sq_pgsql_statement_exec(HSQUIRRELVM v, int exec_type){
	SQ_FUNC_VARS(v);
	GET_pgsql_statement_INSTANCE();
	SQ_OPT_INTEGER(v, 3, result_type, 0);
	SQInteger psize = sq_getsize(v, 2);
	if(psize != self->param_count)
    {
        return sq_throwerror(v, _SC("Wrong number of paramters, exptexted %d"), self->param_count);
    }
	int result = SQ_ERROR;
	SQBool bval;
	const SQChar *str_val;
	SQInteger *int_val_ptr;
	SQFloat *float_val_ptr;
	PGresult *qres;
    SQUnsignedInteger param_values_size = psize * sizeof(self->param_values);
    SQUnsignedInteger param_sizes_size = psize * sizeof(self->param_sizes);
    SQUnsignedInteger param_types_size = psize * sizeof(self->param_types);
	if(!self->param_values)
    {
        //only allocate once
        self->param_values = (void **)sq_malloc(param_values_size);
        self->param_sizes = (int *)sq_malloc(param_sizes_size);
        self->param_types = (int *)sq_malloc(param_types_size);
    }

    memset(self->param_values, 0, param_values_size);
    memset(self->param_sizes, 0, param_sizes_size);
    memset(self->param_types, 0, param_types_size);

    sq_reservestack(v, psize*2);

    for(SQInteger i=0; i < psize; ++i)
    {
        sq_pushinteger(v, i);
        if(sq_get(v, 2) == SQ_OK)
        {
            switch(sq_gettype(v, -1))
            {
            case OT_NULL:
                self->param_values[i] = NULL;
                self->param_sizes[i] = 0;
                self->param_types[i] = 0;
                sq_poptop(v);
                break;
            case OT_BOOL:
                sq_getbool(v, -1, &bval);
                self->param_values[i] = (void*)(bval == SQTrue ? "1" : "0");
                self->param_sizes[i] = 1;
                param_types[i] = BOOLOID;
                sq_poptop(v);
                break;
            case OT_INTEGER:
                sq_getinteger_ptr(v, -1, &int_val_ptr);
                self->param_values[i] = (void*)int_val_ptr;
                self->param_sizes[i] = (int)sizeof(SQInteger);
                param_types[i] = INT8OID;
                break;
            case OT_FLOAT:
                sq_getfloat_ptr(v, -1, &float_val_ptr);
                self->param_values[i] = (void*)float_val_ptr;
                self->param_sizes[i] = (int)sizeof(SQFloat);
                param_types[i] = FLOAT8OID;
                break;
            case OT_STRING:
                //sq_tostring(v, -1);
                sq_getstring(v, -1, &str_val);
                self->param_values[i] = (void*)str_val;
                self->param_sizes[i] = (int)sq_getsize(v, -1);
                self->param_types[i] = VARCHAROID;
                break;
            default:
                result = sq_throwerror(v, _SC("Unknow parameter type at pos %d"), i);
                goto cleanup;
            }
        }
    }
*/

static SQRESULT sq_pgsql_statement_exec(HSQUIRRELVM v, int exec_type){
	SQ_FUNC_VARS(v);
	GET_pgsql_statement_INSTANCE();
	SQ_OPT_INTEGER(v, 3, result_type, 0);
	SQInteger psize = sq_getsize(v, 2);
	if(psize != self->param_count)
    {
        return sq_throwerror(v, _SC("Wrong number of paramters, exptexted %d"), self->param_count);
    }
	int result = SQ_ERROR;
	SQBool bval;
	const SQChar *str_val;
	PGresult *qres;
    SQUnsignedInteger param_values_size = psize * sizeof(self->param_values);
    SQUnsignedInteger param_sizes_size = psize * sizeof(self->param_sizes);
	if(!self->param_values)
    {
        //only allocate once
        self->param_values = (void **)sq_malloc(param_values_size);
        self->param_sizes = (int *)sq_malloc(param_sizes_size);
    }

    memset(self->param_values, 0, param_values_size);
    memset(self->param_sizes, 0, param_sizes_size);

    sq_reservestack(v, psize*2);

    for(SQInteger i=0; i < psize; ++i)
    {
        sq_pushinteger(v, i);
        if(sq_get(v, 2) == SQ_OK)
        {
            switch(sq_gettype(v, -1))
            {
            case OT_NULL:
                sq_poptop(v);
                break;
            case OT_BOOL:
                sq_getbool(v, -1, &bval);
                self->param_values[i] = (void*)(bval == SQTrue ? "1" : "0");
                self->param_sizes[i] = 1;
                sq_poptop(v);
                break;
            case OT_INTEGER:
            case OT_FLOAT:
            case OT_STRING:
                sq_tostring(v, -1);
                sq_getstring(v, -1, &str_val);
                self->param_values[i] = (void*)str_val;
                self->param_sizes[i] = (int)sq_getsize(v, -1);
                break;
            default:
                result = sq_throwerror(v, _SC("Unknow parameter type at pos %d"), (int)i);
                goto cleanup;
            }
        }
    }

	qres = dlPQexecPrepared(self->db, self->name, psize,
                         (const char**)self->param_values, self->param_sizes, NULL, result_type);
	result = dlPQresultStatus(qres);
	if(result == PGRES_COMMAND_OK || result == PGRES_TUPLES_OK)
    {
        if(exec_type == SQ_EXEC_DML)
        {
            sq_pushinteger(v, atoi(dlPQcmdTuples(qres)));
            dlPQclear(qres);
        }
        else if(exec_type == SQ_EXEC_SCALAR)
        {
            int ntuples = dlPQntuples(qres);
            int nfields = dlPQnfields(qres);
            if(exec_type == SQ_EXEC_SCALAR && (ntuples == 1) && (nfields > 0))
            {
                result = atoi(dlPQgetvalue(qres, 0, 0));
                sq_pushinteger(v, result);
                dlPQclear(qres);
            }
            else
            {
                sq_pushnull(v);
            }
        }
        else if(exec_type == SQ_EXEC_QUERY)
        {
            sq_pushroottable(v);
            sq_pushstring(v, PostgreSQL_Result_TAG, -1);
            if(sq_get(v, -2) == SQ_OK){
                if(sq_createinstance(v, -1) == SQ_OK){
                    sq_setinstanceup(v, -1, qres);
                    sq_setreleasehook(v, -1, sq_pgsql_result_releasehook);
                    sq_pushstring(v, _curr_row_key, -1);
                    sq_pushinteger(v, -1);
                    sq_set(v, -3);
                }
            }
        }
        result = 1;
    }
	else
    {
        dlPQclear(qres);
        //sq_settop(v, _top_);
        result = sq_throwerror(v, _SC("%s"), dlPQerrorMessage(self->db));
    }
cleanup:
	return result;
}

static SQRESULT sq_pgsql_statement_exec_query(HSQUIRRELVM v){
    return sq_pgsql_statement_exec(v, SQ_EXEC_QUERY);
}

static SQRESULT sq_pgsql_statement_exec_scalar(HSQUIRRELVM v){
    return sq_pgsql_statement_exec(v, SQ_EXEC_SCALAR);
}

static SQRESULT sq_pgsql_statement_exec_dml(HSQUIRRELVM v){
    return sq_pgsql_statement_exec(v, SQ_EXEC_DML);
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_statement_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_statement_methods[] =
{
	_DECL_FUNC(exec_query,  -2, _SC("xai")),
	_DECL_FUNC(exec_scalar,  2, _SC("xa")),
	_DECL_FUNC(exec_dml,  2, _SC("xa")),
	_DECL_FUNC(close,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_pgsql_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	PGconn *self = ((PGconn *)p);
	if (self) dlPQfinish(self);
	return 0;
}

static SQRESULT sq_pgsql_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, szConnInfo);
    PGconn *self=0;

    if(load_libpq(dynamicLibName))
    {
        self = dlPQconnectdb(szConnInfo);
        if (dlPQstatus(self) == CONNECTION_BAD) return sq_throwerror(v, _SC("Failed to connect ot database !"));
    }
    else return sq_throwerror(v, _SC("Failed to load libpq !"));

    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, sq_pgsql_releasehook);

    //save a weakref to allow statement return it's db
    sq_pushuserpointer(v, self);
    sq_weakref(v, 1);
    sq_setonregistrytable(v);

    return 1;
}

static SQRESULT sq_pgsql_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	dlPQfinish(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "database is closed"
	return 0;
}

#if 0
static PGresult *sq_pgsql_exec_params(HSQUIRRELVM v) {
	SQ_FUNC_VARS(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

    int nParams = _top_ - 2;	/* subtract connection and command */
    if(nParams)
    {
        if (nParams > 65535) return null;
        sq_reservestack(v, 4);

        Oid *paramTypes = sq_newuserdata(v, nParams * sizeof(Oid));
        char **paramValues = sq_newuserdata(v, nParams * sizeof(char*));
        int *paramLengths = sq_newuserdata(v, nParams * sizeof(int));
        int *paramFormats = sq_newuserdata(v, nParams * sizeof(int));

		for (int n = 0; n < nParams; n++)
        {
			//get_param(v, 3 + n, n, paramTypes, paramValues, paramLengths, paramFormats);
        }

        return dlPQexecParams(self. szSQL, nParams, paramTypes,
            (const char * const*)paramValues, paramLengths, paramFormats, 0);
    }
    return dlPQexec(self, szSQL);
}
#endif

static SQRESULT sq_pgsql_exec_dml(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
     int result = 0;

    PGresult *qres = dlPQexec(self, szSQL);

    bool is_ok = dlPQresultStatus(qres) == PGRES_COMMAND_OK;

    if (is_ok) result = atoi(dlPQcmdTuples(qres));
    dlPQclear(qres);

    if (!is_ok) return sq_throwerror(v, _SC("%s"), dlPQerrorMessage(self));

    sq_pushinteger(v, result);
	return 1;
}

static SQRESULT sq_pgsql_exec_scalar(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
     int result = 0;

    PGresult *qres = dlPQexec(self, szSQL);

    bool is_ok = (dlPQresultStatus(qres) == PGRES_TUPLES_OK) &&
                 (dlPQntuples(qres) == 1) && (dlPQnfields(qres) > 0);

    if (is_ok) result = atoi(dlPQgetvalue(qres, 0, 0));
    dlPQclear(qres);

    if (!is_ok) return sq_throwerror(v, _SC("%s"), dlPQerrorMessage(self));

    sq_pushinteger(v, result);
	return 1;
}

static SQRESULT sq_pgsql_exec_query(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

    PGresult *qres = dlPQexec(self, szSQL);

    if(dlPQresultStatus(qres) == PGRES_TUPLES_OK){
        sq_pushroottable(v);
        sq_pushstring(v, PostgreSQL_Result_TAG, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, qres);
                sq_setreleasehook(v, -1, sq_pgsql_result_releasehook);
                sq_pushstring(v, _curr_row_key, -1);
                sq_pushinteger(v, -1);
                sq_set(v, -3);
                return 1;
            }
        }
    }
    return sq_throwerror(v, _SC("%s"), dlPQerrorMessage(self));
}

static SQRESULT sq_pgsql_do_prepare(HSQUIRRELVM v, int isGetPrepared){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
    PGresult *qres;
    bool bresult;

    PgSqlStatement *stmt = (PgSqlStatement*)sq_malloc(sizeof(PgSqlStatement));
    memset(stmt, 0, sizeof(PgSqlStatement));
    stmt->db = self;
    stmt->isGetPrepared = isGetPrepared;

    if(isGetPrepared)
    {
        snprintf(stmt->name, sizeof(stmt->name), "%s", szSQL);
        qres = dlPQdescribePrepared(self, stmt->name);
        bresult = dlPQresultStatus(qres) == PGRES_COMMAND_OK;
        if(bresult) stmt->param_count = dlPQnparams(qres);
        dlPQclear(qres);
    }
    else
    {
        snprintf(stmt->name, sizeof(stmt->name), "sq_stmt_%p_%p", self, stmt);
        qres = dlPQprepare(self, stmt->name, szSQL, 0, NULL);
        bresult = dlPQresultStatus(qres) == PGRES_COMMAND_OK;
        dlPQclear(qres);
    }

    if(bresult){
        if(!isGetPrepared)
        {
            qres = dlPQdescribePrepared(self, stmt->name);
            stmt->param_count = dlPQnparams(qres);
            dlPQclear(qres);
        }
        sq_pushroottable(v);
        sq_pushstring(v, PostgreSQL_Statement_TAG, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, stmt);
                sq_setreleasehook(v, -1, sq_pgsql_statement_releasehook);
                return 1;
            }
        }
    }
    sq_free(stmt, sizeof(PgSqlStatement));
    return sq_throwerror(v, _SC("%s"), dlPQerrorMessage(self));
}

static SQRESULT sq_pgsql_prepare(HSQUIRRELVM v){
    return sq_pgsql_do_prepare(v, 0);
}

static SQRESULT sq_pgsql_get_prepared(HSQUIRRELVM v){
    return sq_pgsql_do_prepare(v, 1);
}

static SQRESULT sq_pgsql_error_message(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	sq_pushstring(v, dlPQerrorMessage(self), -1);
	return 1;
}

static SQRESULT sq_pgsql_version(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	sq_pushinteger(v, dlPQserverVersion(self));
	return 1;
}

static int
 inv_read =  0x40000,
 inv_write = 0x20000,
 //invalidoid = 0,
 inv_seek_set = 0,
 //inv_seek_curr = 1,
 inv_seek_end = 2;

static SQRESULT sq_pgsql_get_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_INTEGER(v, 2, oid);
    //begin_recursive_transaction();
    char *result = 0;
	int ofd = dllo_open(self, oid, inv_read);
	if(ofd >= 0){
		int blobSize = dllo_lseek(self, ofd, 0, inv_seek_end);
		dllo_lseek(self, ofd, 0, inv_seek_set);
	    SQBlob blob(blobSize);
		result = (char*)blob.GetBuf();
		int numRead = 0;
		while(blobSize > 0){
		 	int i = dllo_read(self, ofd, result+numRead, blobSize);
			numRead += i;
			blobSize -= i;
		}
		dllo_close(self, oid);
		sq_pushstring(v, (const SQChar*)blob.GetBuf(), blob.Len());
	}
	//commit_recursive_transaction();
	if(!result) sq_pushnull(v);
	return 1;
}

static SQRESULT sq_pgsql_insert_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    int result = 0;
    SQ_GET_STRING(v, 2, blob);
    SQ_GET_BOOL(v, 3, isFileName);
	if(isFileName){
		result = dllo_import(self, blob);
	} else {
		result = dllo_creat(self, inv_write);
		if(result){
			int ofd = dllo_open(self, result, inv_write);
			if (ofd >= 0){
				int i = blob_size;
				const char *blopPtr = (const char *)blob;
				int numWriten = 0;
				while(i > 0){
				 	int i2 = dllo_write(self, ofd, blopPtr+numWriten, i);
					numWriten += i2;
					i -= i2;
				}
				dllo_close(self, ofd);
			}
			else return sq_throwerror(v, _SC("Failed to insert blob !"));
		}
	}
	if(!result) sq_pushnull(v);
	return 1;
}

static SQRESULT sq_pgsql_update_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_INTEGER(v, 2, oid);
    SQ_GET_STRING(v, 3, blob);
    SQ_GET_BOOL(v, 4, isFileName);

    int result_oid = 0;
    int result_error = SQ_OK;
	int loid = dllo_creat(self, inv_write);
	int ofd = dllo_open(self, loid, inv_write);
	if(ofd >= 0){
		dllo_unlink(self, oid);
		result_oid = loid;
		if(isFileName)
		{
			char buf[2048];
			FILE *fp = fopen(blob, "rb");
			if(!fp) {
			    sq_throwerror(v, _SC("Failed to update blob from file !"));
			    result_error = SQ_ERROR;
			}
			else
			{
                char *charPtr = buf;
                int numRead;
                do{
                    numRead = fread(buf, 1, sizeof(buf), fp);
                    int numWriten = dllo_write(self, ofd, charPtr, numRead);
                    if (numWriten != numRead) {
                        sq_throwerror(v, _SC("Failed to update blob from file !"));
                        result_error = SQ_ERROR;
                        break;
                    }
                } while (numRead == 0);
                fclose(fp);
			}
		}else{
			int i = blob_size;
            const char *blopPtr = (const char *)blob;
			int numWriten = 0;
			while(i > 0){
			 	int i2 = dllo_write(self, ofd, blopPtr+numWriten, i);
				numWriten += i2;
				i -= i2;
			}
		}
		dllo_close(self, ofd);
	}
	if(result_error == SQ_ERROR) return result_error;
	sq_pushinteger(v, result_oid);
	return 1;
}

static SQRESULT sq_pgsql_delete_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_INTEGER(v, 2, oid);
	sq_pushinteger(v, dllo_unlink(self, oid));
	return 1;
}

static SQRESULT sq_pgsql_escape_string(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_STRING(v, 2, str);
	char *escaped_str = dlPQescapeLiteral(self, str, str_size);
	if(escaped_str) {
		sq_pushstring(v, escaped_str, -1);
		dlPQfreemem(escaped_str);
		return 1;
	}
	return sq_throwerror(v, _SC("could not allocate escaped string"));
}

static SQRESULT sq_pgsql_escape_bytea(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_STRING(v, 2, str);
	char *escaped_str = (char*)dlPQescapeByteaConn(self, (const unsigned char*)str, str_size);
	if(escaped_str) {
		sq_pushstring(v, escaped_str, -1);
		dlPQfreemem(escaped_str);
		return 1;
	}
	return sq_throwerror(v, _SC("could not allocate escaped bytea"));
}

static SQRESULT sq_pgsql_unescape_bytea(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_STRING(v, 2, str);
	char *escaped_str = (char*)dlPQunescapeBytea((const unsigned char*)str, str_size);
	if(escaped_str) {
		sq_pushstring(v, escaped_str, -1);
		dlPQfreemem(escaped_str);
		return 1;
	}
	return sq_throwerror(v, _SC("could not allocate unescaped bytea"));
}

static SQRESULT sq_pgsql_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    SQ_OPT_STRING(v, 2, libname, dynamicLibName);
    sq_pushbool(v, load_libpq(libname));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_methods[] =
{
	_DECL_FUNC(constructor,  2, _SC("xs")),
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(exec_dml,  2, _SC("xs")),
	//_DECL_FUNC(exec_one_dml,  -2, _SC("xs")),
	_DECL_FUNC(exec_scalar,  2, _SC("xs")),
	_DECL_FUNC(exec_query,  2, _SC("xs")),
	_DECL_FUNC(prepare,  2, _SC("xs")),
	_DECL_FUNC(get_prepared,  2, _SC("xs")),
	_DECL_FUNC(error_message,  1, _SC("x")),
	_DECL_FUNC(version,  1, _SC("x")),
	_DECL_FUNC(get_blob_field,  2, _SC("xi")),
	_DECL_FUNC(insert_blob_field,  3, _SC("xsb")),
	_DECL_FUNC(update_blob_field,  3, _SC("xisb")),
	_DECL_FUNC(delete_blob_field,  2, _SC("xi")),
	_DECL_FUNC(escape_string,  2, _SC("xs")),
	_DECL_FUNC(escape_bytea,  2, _SC("xs")),
	_DECL_FUNC(unescape_bytea,  2, _SC("xs")),
	_DECL_FUNC(loadlib,-1,_SC(".s")),
	{0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v)
{
    sq_pushstring(v,PostgreSQL_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,PostgreSQL_Statement_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_Statement_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_statement_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,PostgreSQL_Result_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_Result_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_result_methods);
    sq_pushstring(v, _curr_row_key, -1);
    sq_pushnull(v);
    sq_newslot(v, -3, SQFalse);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // WITH_POSTGRESQL
