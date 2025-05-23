#include "mpdecimal.h"
#include "squirrel.h"
#include <string.h>
SQ_OPT_STRING_STRLEN();


static const SQChar sq_decimal_ctx_TAG[] = _SC("DecimalCtx");
#define SQ_PUSH_DECIMAL_CTX_TAG(v) sq_pushstring(v,sq_decimal_ctx_TAG,SIZEOF_SQCHAR_STRING(sq_decimal_ctx_TAG));
static const SQChar sq_decimal_TAG[] = _SC("Decimal");
#define SQ_PUSH_DECIMAL_TAG(v) sq_pushstring(v,sq_decimal_TAG,SIZEOF_SQCHAR_STRING(sq_decimal_TAG))
static const SQChar sq_context_static[] = _SC("context");
#define SQ_PUSH_CONTEXT_STATIC(v) sq_pushstring(v, sq_context_static, SIZEOF_SQCHAR_STRING(sq_context_static));

#define GET_DecimalCtx_INSTANCE(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_context_t, ctx, sq_decimal_ctx_TAG)
#define GET_Decimal_INSTANCE(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_t, dec, sq_decimal_TAG)
#define GET_Decimal_INSTANCE2(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_t, dec2, sq_decimal_TAG)


static SQRESULT sq_DecimalCtx_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    mpd_context_t *ctx = (mpd_context_t *)p;
    if(ctx) sq_free(ctx, sizeof(mpd_context_t));
    return 0;
}

/*
** Creates a new DecimalCtx.
*/
static SQRESULT sq_DecimalCtx_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    SQ_OPT_INTEGER(v, 2, prec, 8);
    if(prec < 0) return sq_throwerror(v, _SC("invalid precision (" _PRINT_INT_FMT ")"), prec);
    ctx = (mpd_context_t *)sq_malloc(sizeof(mpd_context_t));
    mpd_init(ctx, prec);
    sq_setinstanceup(v, 1, ctx);
    sq_setreleasehook(v, 1, sq_DecimalCtx_release_hook);
    return 1;
}

#define SQ_DECIMAL_FUNC_GETSET(sqfn)\
static SQRESULT sq_DecimalCtx_##sqfn(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS(v);\
    GET_DecimalCtx_INSTANCE(v, 1);\
    if(_top_ > 1){\
        SQ_GET_INTEGER(v, 2, sqfn);\
        if(sqfn < 0) return sq_throwerror(v, _SC("invalid " #sqfn));\
        mpd_qset##sqfn(ctx, sqfn);\
        return 0;\
    }\
    else sq_pushinteger(v, mpd_get##sqfn(ctx));\
	return 1;\
}

SQ_DECIMAL_FUNC_GETSET(prec);
SQ_DECIMAL_FUNC_GETSET(emax);
SQ_DECIMAL_FUNC_GETSET(emin);
SQ_DECIMAL_FUNC_GETSET(round);
SQ_DECIMAL_FUNC_GETSET(traps);
SQ_DECIMAL_FUNC_GETSET(status);
SQ_DECIMAL_FUNC_GETSET(clamp);
SQ_DECIMAL_FUNC_GETSET(cr);


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_DecimalCtx_##name,nparams,tycheck}
static SQRegFunction DecimalCtx_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("xi")),
    _DECL_FUNC(prec,-1,_SC("xi")),
    _DECL_FUNC(emax,-1,_SC("xi")),
    _DECL_FUNC(emin,-1,_SC("xi")),
    _DECL_FUNC(round,-1,_SC("xi")),
    _DECL_FUNC(traps,-1,_SC("xi")),
    _DECL_FUNC(status,-1,_SC("xi")),
    _DECL_FUNC(clamp,-1,_SC("xi")),
    _DECL_FUNC(cr,-1,_SC("xi")),
    {0,0}
};
#undef _DECL_FUNC

static mpd_context_t * sq_get_global_ctx(HSQUIRRELVM v, SQInteger idx)
{
    SQ_PUSH_CONTEXT_STATIC(v);
    sq_get(v, idx);
    mpd_context_t *ctx = 0;
    sq_getinstanceup(v, -1, (void**)&ctx, (void*)sq_decimal_ctx_TAG,SQFalse);
    sq_poptop(v);
	return ctx;
}

static SQRESULT sq_Decimal_error(HSQUIRRELVM v, uint32_t status) {
    const SQChar *error = _SC("MPD_??");
#define CASE_ERROR(n) case n: error = #n; break;
    switch(status){
        CASE_ERROR(MPD_Clamped);
        CASE_ERROR(MPD_Conversion_syntax);
        CASE_ERROR(MPD_Division_by_zero );
        CASE_ERROR(MPD_Division_impossible);
        CASE_ERROR(MPD_Division_undefined);
        CASE_ERROR(MPD_Fpu_error);
        CASE_ERROR(MPD_Inexact);
        CASE_ERROR(MPD_Invalid_context);
        CASE_ERROR(MPD_Invalid_operation);
        CASE_ERROR(MPD_Malloc_error);
        CASE_ERROR(MPD_Not_implemented);
        CASE_ERROR(MPD_Overflow);
        CASE_ERROR(MPD_Rounded);
        CASE_ERROR(MPD_Subnormal);
        CASE_ERROR(MPD_Underflow);
    }
#undef CASE_ERROR
    return sq_throwerror(v, _SC("%s"), error);
}

static SQRESULT sq_Decimal_set_from(HSQUIRRELVM v, SQInteger idx, mpd_context_t *ctx, mpd_t *dec, uint32_t *status){
    SQInteger _rc_;
    switch(sq_gettype(v, idx)){

        case OT_INSTANCE:{
            GET_Decimal_INSTANCE2(v, idx);
            mpd_qcopy(dec, dec2, status);
        }

        case OT_INTEGER:{
            SQ_GET_INTEGER(v, idx, iparam);
#ifdef CONFIG_32
            mpd_qset_i32(dec, iparam, ctx, status);
#else
            mpd_qset_i64(dec, iparam, ctx, status);
#endif
        }
        break;
        case OT_STRING:{
            SQ_GET_STRING(v, idx, str);
            mpd_qset_string(dec, str, ctx, status);
        }
        break;
        case OT_FLOAT:{
            SQ_GET_FLOAT(v, idx, fparam);
            char buf[32];
            snprintf(buf, sizeof(buf), "%f", fparam);
            mpd_qset_string(dec, buf, ctx, status);
        }
        break;

        default:
            return sq_throwerror(v, _SC("invalid type (%s) to convert to decimal"), sq_gettypename(v, idx));
    }
    return SQ_OK;
}

static SQRESULT sq_Decimal_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    mpd_t *dec = (mpd_t *)p;
    if(dec) mpd_del(dec);
    return 0;
}

/*
** Creates a new Decimal.
*/
static SQRESULT sq_Decimal_constructor (HSQUIRRELVM v) {
    uint32_t status = 0;
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *dec = mpd_new(ctx);
    if(sq_gettop(v) > 1){
        if(sq_Decimal_set_from(v, 2, ctx, dec, &status) != SQ_OK) return SQ_ERROR;
    }
    else
#ifdef CONFIG_32
        mpd_qset_i32(dec, 0, ctx, &status);
#else
        mpd_qset_i64(dec, 0, ctx, &status);
#endif

	ctx->status |= status;
	if (status&ctx->traps) {
	    mpd_del(dec);
	    return sq_Decimal_error(v, status);
	}

    sq_setinstanceup(v, 1, dec);
    sq_setreleasehook(v, 1, sq_Decimal_release_hook);
    return 1;
}

static SQRESULT sq_Decimal_set(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    uint32_t status = 0;
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    if(sq_Decimal_set_from(v, 2, ctx, dec, &status) != SQ_OK) return SQ_ERROR;
	ctx->status |= status;
	if (status&ctx->traps) {
	    return sq_Decimal_error(v, status);
	}
	sq_settop(v, 1); //returns itself
	return 1;
}

static SQRESULT sq_Decimal_new_for_dec (HSQUIRRELVM v, mpd_t *dec, mpd_context_t *ctx, uint32_t status) {
    //mpd_addstatus_raise(ctx, status);
	ctx->status |= status;
	if (status&ctx->traps) {
	    mpd_del(dec);
	    return sq_Decimal_error(v, status);
	}

    sq_pushstring(v, sq_decimal_TAG, -1);
    sq_getonroottable(v);
    sq_createinstance(v, -1);
    sq_setinstanceup(v, -1, dec);
    sq_setreleasehook(v, -1, sq_Decimal_release_hook);
    return 1;
}

static SQRESULT sq_Decimal_tostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    char *decstring =  mpd_to_eng(dec, 1);
    sq_pushstring(v, decstring, -1);
    if (decstring) mpd_free(decstring);
	return 1;
}

#define SQ_DECIMAL_FUNC2(sqfn, decfn)\
static SQRESULT sq_Decimal_##sqfn(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS_NO_TOP(v);\
    GET_Decimal_INSTANCE(v, 1);\
    GET_Decimal_INSTANCE2(v, 2);\
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);\
    mpd_t *result = mpd_new(ctx);\
    uint32_t status = 0;\
    decfn(result, dec, dec2, ctx, &status);\
	return sq_Decimal_new_for_dec(v, result, ctx, status);\
}

SQ_DECIMAL_FUNC2(_add, mpd_qadd);
SQ_DECIMAL_FUNC2(_sub, mpd_qsub);
SQ_DECIMAL_FUNC2(_mul, mpd_qmul);
SQ_DECIMAL_FUNC2(_div, mpd_qdiv);
SQ_DECIMAL_FUNC2(_modulo, mpd_qrem);
SQ_DECIMAL_FUNC2(max, mpd_qmax);
SQ_DECIMAL_FUNC2(min, mpd_qmin);
SQ_DECIMAL_FUNC2(pow, mpd_qpow);
SQ_DECIMAL_FUNC2(next_toward, mpd_qnext_toward);

#define SQ_DECIMAL_FUNC1(sqfn, decfn)\
static SQRESULT sq_Decimal_##sqfn(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS_NO_TOP(v);\
    GET_Decimal_INSTANCE(v, 1);\
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);\
    mpd_t *result = mpd_new(ctx);\
    uint32_t status = 0;\
    decfn(result, dec, ctx, &status);\
	return sq_Decimal_new_for_dec(v, result, ctx, status);\
}

SQ_DECIMAL_FUNC1(_unm, mpd_qminus);
SQ_DECIMAL_FUNC1(next_minus, mpd_qnext_minus);
SQ_DECIMAL_FUNC1(next_plus, mpd_qnext_plus);
SQ_DECIMAL_FUNC1(abs, mpd_qabs);
SQ_DECIMAL_FUNC1(sqrt, mpd_qsqrt);
SQ_DECIMAL_FUNC1(exp, mpd_qexp);
SQ_DECIMAL_FUNC1(ln, mpd_qln);
SQ_DECIMAL_FUNC1(log10, mpd_qlog10);
SQ_DECIMAL_FUNC1(invroot, mpd_qinvroot);
SQ_DECIMAL_FUNC1(floor, mpd_qfloor);
SQ_DECIMAL_FUNC1(ceil, mpd_qceil);
SQ_DECIMAL_FUNC1(trunc, mpd_qtrunc);

static SQRESULT sq_Decimal__cmp(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    sq_pushinteger(v, mpd_cmp(dec, dec2, ctx));
	return 1;
}

#define DECIMAL_IS(fn) \
static SQRESULT sq_Decimal_##fn(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS_NO_TOP(v);\
    GET_Decimal_INSTANCE(v, 1);\
    sq_pushbool(v, mpd_##fn(dec));\
	return 1;\
}

DECIMAL_IS(isfinite);
DECIMAL_IS(isinfinite);
DECIMAL_IS(isnan);
DECIMAL_IS(isnegative);
DECIMAL_IS(ispositive);
DECIMAL_IS(isqnan);
DECIMAL_IS(issigned);
DECIMAL_IS(issnan);
DECIMAL_IS(isspecial);
DECIMAL_IS(iszero);
DECIMAL_IS(isinteger);
DECIMAL_IS(isodd);
DECIMAL_IS(iseven);

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_Decimal_##name,nparams,tycheck}
static SQRegFunction Decimal_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("x n|s|x")),
    _DECL_FUNC(set, 2,_SC("x n|s|x")),
    {_SC("_tostring"),sq_Decimal_tostring, 1,_SC("x")},
    _DECL_FUNC(tostring,1,_SC("x")),
    _DECL_FUNC(_add, 2,_SC("xx")),
    _DECL_FUNC(_sub, 2,_SC("xx")),
    _DECL_FUNC(_mul, 2,_SC("xx")),
    _DECL_FUNC(_div, 2,_SC("xx")),
    _DECL_FUNC(_modulo, 2,_SC("xx")),
    _DECL_FUNC(_cmp, 2,_SC("xx")),
    _DECL_FUNC(max, 2,_SC("xx")),
    _DECL_FUNC(min, 2,_SC("xx")),
    _DECL_FUNC(pow, 2,_SC("xx")),
    _DECL_FUNC(abs, 1,_SC("x")),
    _DECL_FUNC(_unm, 1,_SC("x")),
    _DECL_FUNC(sqrt, 1,_SC("x")),
    _DECL_FUNC(exp, 1,_SC("x")),
    _DECL_FUNC(ln, 1,_SC("x")),
    _DECL_FUNC(log10, 1,_SC("x")),
    _DECL_FUNC(floor, 1,_SC("x")),
    _DECL_FUNC(ceil, 1,_SC("x")),
    _DECL_FUNC(trunc, 1,_SC("x")),
    _DECL_FUNC(invroot, 1,_SC("x")),
    _DECL_FUNC(next_minus, 1,_SC("x")),
    _DECL_FUNC(next_plus, 1,_SC("x")),
    _DECL_FUNC(next_toward, 2,_SC("xx")),
    _DECL_FUNC(isfinite, 1,_SC("x")),
    _DECL_FUNC(isinfinite, 1,_SC("x")),
    _DECL_FUNC(isnan, 1,_SC("x")),
    _DECL_FUNC(isnegative, 1,_SC("x")),
    _DECL_FUNC(ispositive, 1,_SC("x")),
    _DECL_FUNC(isqnan, 1,_SC("x")),
    _DECL_FUNC(issigned, 1,_SC("x")),
    _DECL_FUNC(issnan, 1,_SC("x")),
    _DECL_FUNC(isspecial, 1,_SC("x")),
    _DECL_FUNC(iszero, 1,_SC("x")),
    _DECL_FUNC(isinteger, 1,_SC("x")),
    _DECL_FUNC(isodd, 1,_SC("x")),
    _DECL_FUNC(iseven, 1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

#define CTXC(s)  { _SC(#s), MPD_ ## s },

static const struct {
    const SQChar* name;
    mpd_uint_t value;
} ctx_constants[] = {
    /*Precision and Exponents*/
    CTXC(MAX_PREC)
    CTXC(MAX_EMAX)
    //CTXC(MIN_EMIN)
    /* rounding */
    CTXC(ROUND_UP)                  /* round away from 0 */
    CTXC(ROUND_DOWN)                /* round toward 0 (truncate) */
    CTXC(ROUND_CEILING)             /* round toward +infinity */
    CTXC(ROUND_FLOOR)               /* round toward -infinity */
    CTXC(ROUND_HALF_UP)             /* 0.5 is rounded up */
    CTXC(ROUND_HALF_DOWN)           /* 0.5 is rounded down */
    CTXC(ROUND_HALF_EVEN)           /* 0.5 is rounded to even */
    CTXC(ROUND_05UP)                /* round zero or five away from 0  */
    CTXC(ROUND_TRUNC)               /* truncate, but set infinities  */
    /* Trap-enabler and Status flags */
    CTXC(Conversion_syntax)
    CTXC(Division_impossible)
    CTXC(Division_undefined)
    CTXC(Invalid_context)
    CTXC(Invalid_operation)
    CTXC(Malloc_error)
    CTXC(Clamped)
    CTXC(Division_by_zero)
    CTXC(Fpu_error)
    CTXC(Inexact)
    CTXC(Not_implemented)
    CTXC(Overflow)
    CTXC(Rounded)
    CTXC(Subnormal)
    CTXC(Underflow)
    CTXC(IEEE_Invalid_operation)
    /*IEEE Interchange Formats*/
    CTXC(IEEE_CONTEXT_MAX_BITS)
    CTXC(DECIMAL32)
    CTXC(DECIMAL64)
    CTXC(DECIMAL128)
    /* terminator */
    { NULL, 0 }
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_decimal(HSQUIRRELVM v)
{
    SQ_PUSH_DECIMAL_CTX_TAG(v);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_decimal_ctx_TAG);
    sq_insert_reg_funcs(v, DecimalCtx_methods);
    for(int i=0; ctx_constants[i].name; ++i){
        sq_pushstring(v, ctx_constants[i].name, -1);
        sq_pushinteger(v, ctx_constants[i].value);
        sq_newslot(v,-3,SQTrue);
    }
    sq_newslot(v,-3,SQTrue);

    SQ_PUSH_DECIMAL_TAG(v);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_decimal_TAG);

    SQ_PUSH_CONTEXT_STATIC(v);
	sq_pushstring(v,sq_decimal_ctx_TAG,-1);
	sq_getonroottable(v);
	sq_pushroottable(v);
	sq_call(v, 1, SQTrue, SQFalse);
	sq_remove(v, -2);
    sq_newslot(v,-3,SQTrue);

    sq_insert_reg_funcs(v, Decimal_methods);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif
