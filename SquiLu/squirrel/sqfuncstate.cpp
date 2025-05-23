/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#ifndef NO_COMPILER
#include "sqcompiler.h"
#include "sqstring.h"
#include "sqfuncproto.h"
#include "sqtable.h"
#include "sqopcodes.h"
#include "sqfuncstate.h"

#if __cplusplus >= 201103L
#include <utility>
#define SQCPPMOVE(x) std::move(x)
#else
#define SQCPPMOVE(x) (x)
#endif

#ifdef _DEBUG_DUMP
#define ENUM_OP(a,b) {_SC(#a)},
SQInstructionDesc g_InstrDesc[]={
    SQ_OP_CODE_LIST()
};
#undef ENUM_OP
#endif
void SQDumpLiteral(SQObjectPtr &o)
{
	switch(sq_type(o)){
		case OT_STRING_UTF8:
		case OT_STRING: {
		    int i, len, buf_idx = 0;
		    #define BUF_SIZE 64
		    SQChar buf[BUF_SIZE];
		    SQChar *str = _stringval(o);
		    scprintf(_SC("\""));
		    for(i=0, len=_string(o)->_len; i < len; ++i, ++buf_idx) {
                if(buf_idx == (BUF_SIZE-1)) {
                    scprintf(_SC("%*.s"), buf_idx, buf);
                    buf_idx = 0;
                }
                switch(str[i])
                {
                case _SC('\a'):
                case _SC('\b'):
                case _SC('\t'):
                case _SC('\n'):
                case _SC('\v'):
                case _SC('\f'):
                case _SC('\r'):
                case _SC('\\'):
                case _SC('\"'):
                    buf[buf_idx++] = _SC('\\');
                    //falthrough
                default:
                        buf[buf_idx++] = str[i];
                }
		    }
		    if(buf_idx > 0) scprintf(_SC("%*.s"), buf_idx, buf);
		    scprintf(_SC("\""));
		    #undef BUF_SIZE
        }
		break;
		case OT_FLOAT: scprintf(_SC("{%f}"),_float(o));break;
		case OT_INTEGER: scprintf(_SC("{") _PRINT_INT_FMT _SC("}"),_integer(o));break;
		case OT_BOOL: scprintf(_SC("%s"),_integer(o)?_SC("true"):_SC("false"));break;
		default: scprintf(_SC("(%s %p)"),GetTypeName(o),(void*)_rawval(o));break; //shut up compiler
	}
}

SQFuncState::SQFuncState(SQSharedState *ss,SQFuncState *parent,CompilerErrorFunc efunc,void *ed):
    _returnexp(0),_stacksize(0),_nliterals(0),_parent(parent),_lastline(0),_traps(0),
    _outers(0),_optimization(true),_varparams(false),_bgenerator(false),_sharedstate(ss),
    _errfunc(efunc),_errtarget(ed),_ss(ss)
{
		_literals = SQTable::Create(ss,0);
		_strings =  SQTable::Create(ss,0);
}

void SQFuncState::Error(const SQChar *err)
{
	_errfunc(_errtarget,err);
}

#ifdef _DEBUG_DUMP
void SQFuncState::Dump(SQFunctionProto *func)
{
	SQUnsignedInteger n=0,i;
	SQInteger si;
	scprintf(_SC("SQInstruction sizeof %d\n"),(SQInt32)sizeof(SQInstruction));
	scprintf(_SC("SQObject sizeof %d\n"),(SQInt32)sizeof(SQObject));
	scprintf(_SC("--------------------------------------------------------------------\n"));
	scprintf(_SC("*****FUNCTION [%s]\n"),sq_type(func->_name)==OT_STRING?_stringval(func->_name):_SC("unknown"));
	scprintf(_SC("-----LITERALS\n"));
	SQObjectPtr refidx,key,val;
	SQInteger idx;
	SQObjectPtrVec templiterals;
	templiterals.resize(_nliterals);
	while((idx=_table(_literals)->Next(false,refidx,key,val))!=-1) {
		refidx=idx;
		templiterals[_integer(val)]=key;
	}
	for(i=0;i<templiterals.size();i++){
		scprintf(_SC("[%d] "),(SQInt32)n);
		SQDumpLiteral(templiterals[i]);
		scprintf(_SC("\n"));
		n++;
	}
	scprintf(_SC("-----PARAMS\n"));
	if(_varparams)
		scprintf(_SC("<<VARPARAMS>>\n"));
	n=0;
	for(i=0;i<_parameters.size();i++){
		scprintf(_SC("[%d] "),(SQInt32)n);
		SQDumpLiteral(_parameters[i]);
		scprintf(_SC("\n"));
		n++;
	}
	scprintf(_SC("-----LOCALS\n"));
	for(si=0;si<func->_nlocalvarinfos;si++){
		SQLocalVarInfo lvi=func->_localvarinfos[si];
		scprintf(_SC("[%d] %s \t%d %d\n"),(SQInt32)lvi._pos,_stringval(lvi._name),(SQInt32)lvi._start_op,(SQInt32)lvi._end_op);
		n++;
	}
	scprintf(_SC("-----LINE INFO\n"));
	for(i=0;i<_lineinfos.size();i++){
		SQLineInfo li=_lineinfos[i];
		scprintf(_SC("op [%d] line [%d] \n"),(SQInt32)li._op,(SQInt32)li._line);
		n++;
	}
	scprintf(_SC("-----dump\n"));
	n=0;
	for(i=0;i<_instructions.size();i++){
		SQInstruction &inst=_instructions[i];
		if(inst.op==_OP_LOAD || inst.op==_OP_DLOAD || inst.op==_OP_PREPCALLK || inst.op==_OP_GETK ){

			SQInteger lidx = inst._arg1;
			scprintf(_SC("[%03d] %15s %d "),(SQInt32)n,g_InstrDesc[inst.op].name,inst._arg0);
			if(lidx >= 0xFFFFFFFF)
				scprintf(_SC("null"));
			else {
				SQInteger refidx;
				SQObjectPtr val,key,refo;
				while(((refidx=_table(_literals)->Next(false,refo,key,val))!= -1) && (_integer(val) != lidx)) {
					refo = refidx;
				}
				SQDumpLiteral(key);
			}
			if(inst.op != _OP_DLOAD) {
				scprintf(_SC(" %d %d \n"),inst._arg2,inst._arg3);
			}
			else {
				scprintf(_SC(" %d "),inst._arg2);
				lidx = inst._arg3;
				if(lidx >= 0xFFFFFFFF)
					scprintf(_SC("null"));
				else {
					SQInteger refidx;
					SQObjectPtr val,key,refo;
					while(((refidx=_table(_literals)->Next(false,refo,key,val))!= -1) && (_integer(val) != lidx)) {
						refo = refidx;
				}
				SQDumpLiteral(key);
				scprintf(_SC("\n"));
			}
			}
		}
		else if(inst.op==_OP_LOADFLOAT) {
			scprintf(_SC("[%03d] %15s %d %f %d %d\n"),(SQInt32)n,g_InstrDesc[inst.op].name,inst._arg0,*((SQFloat*)&inst._arg1),inst._arg2,inst._arg3);
		}
	/*	else if(inst.op==_OP_ARITH){
			scprintf(_SC("[%03d] %15s %d %d %d %c\n"),(SQInt32)n,g_InstrDesc[inst.op].name,inst._arg0,inst._arg1,inst._arg2,inst._arg3);
		}*/
		else {
			scprintf(_SC("[%03d] %15s %d %d %d %d\n"),(SQInt32)n,g_InstrDesc[inst.op].name,inst._arg0,inst._arg1,inst._arg2,inst._arg3);
		}
		n++;
	}
	scprintf(_SC("-----\n"));
	scprintf(_SC("stack size[%d]\n"),(SQInt32)func->_stacksize);
	scprintf(_SC("--------------------------------------------------------------------\n\n"));
}
#endif

SQInteger SQFuncState::GetNumericConstant(const SQInteger cons)
{
	return GetConstant(SQObjectPtr(cons));
}

SQInteger SQFuncState::GetNumericConstant(const SQFloat cons)
{
	return GetConstant(SQObjectPtr(cons));
}

SQInteger SQFuncState::GetConstant(const SQObject &cons)
{
	SQObjectPtr val;
	if(!_table(_literals)->Get(cons,val))
	{
		val = _nliterals;
		_table(_literals)->NewSlot(cons,val);
		_nliterals++;
		if(_nliterals > MAX_LITERALS) {
			val.Null();
			Error(_SC("internal compiler error: too many literals"));
		}
	}
	return _integer(val);
}

void SQFuncState::SetInstructionParams(SQInteger pos,SQInteger arg0,SQInteger arg1,SQInteger arg2,SQInteger arg3)
{
	_instructions[pos]._arg0=(unsigned char)*((SQUnsignedInteger *)&arg0);
	_instructions[pos]._arg1=(SQInt32)*((SQUnsignedInteger *)&arg1);
	_instructions[pos]._arg2=(unsigned char)*((SQUnsignedInteger *)&arg2);
	_instructions[pos]._arg3=(unsigned char)*((SQUnsignedInteger *)&arg3);
}

void SQFuncState::SetInstructionParam(SQInteger pos,SQInteger arg,SQInteger val)
{
	switch(arg){
		case 0:_instructions[pos]._arg0=(unsigned char)*((SQUnsignedInteger *)&val);break;
		case 1:case 4:_instructions[pos]._arg1=(SQInt32)*((SQUnsignedInteger *)&val);break;
		case 2:_instructions[pos]._arg2=(unsigned char)*((SQUnsignedInteger *)&val);break;
		case 3:_instructions[pos]._arg3=(unsigned char)*((SQUnsignedInteger *)&val);break;
	};
}

SQInteger SQFuncState::AllocStackPos()
{
	SQInteger npos=_vlocals.size();
	_vlocals.push_back(SQLocalVarInfo());
	if(_vlocals.size()>((SQUnsignedInteger)_stacksize)) {
		if(_stacksize>MAX_FUNC_STACKSIZE) Error(_SC("internal compiler error: too many locals"));
		_stacksize=_vlocals.size();
	}
	return npos;
}

SQInteger SQFuncState::PushTarget(SQInteger n)
{
	if(n!=-1){
		_targetstack.push_back(n);
		return n;
	}
	n=AllocStackPos();
	_targetstack.push_back(n);
	return n;
}

SQInteger SQFuncState::GetUpTarget(SQInteger n){
	return _targetstack[((_targetstack.size()-1)-n)];
}

SQInteger SQFuncState::TopTarget(){
	return _targetstack.back();
}
SQInteger SQFuncState::PopTarget()
{
	SQUnsignedInteger npos=_targetstack.back();
	assert(npos < _vlocals.size());
	SQLocalVarInfo &t = _vlocals[npos];
	if(sq_type(t._name)==OT_NULL){
		_vlocals.pop_back();
	}
	_targetstack.pop_back();
	return npos;
}

SQInteger SQFuncState::GetStackSize()
{
	return _vlocals.size();
}

SQInteger SQFuncState::CountOuters(SQInteger stacksize)
{
	SQInteger outers = 0;
	SQInteger k = _vlocals.size() - 1;
	while(k >= stacksize) {
		SQLocalVarInfo &lvi = _vlocals[k];
		k--;
		if(lvi._end_op == UINT_MINUS_ONE) { //this means is an outer
			outers++;
		}
	}
	return outers;
}

void SQFuncState::SetStackSize(SQInteger n)
{
	SQInteger size=_vlocals.size();
	while(size>n){
		size--;
		SQLocalVarInfo lvi = _vlocals.back();
		if(sq_type(lvi._name)!=OT_NULL){
			if(lvi._end_op == UINT_MINUS_ONE) { //this means is an outer
				_outers--;
			}
			lvi._end_op = GetCurrentPos();
			_localvarinfos.push_back(lvi);
		}
		_vlocals.pop_back();
	}
}

bool SQFuncState::IsConstant(const SQObject &name,SQObject &e)
{
	SQObjectPtr val;
	if(_table(_sharedstate->_consts)->Get(name,val)) {
		e = val;
		return true;
	}
	return false;
}

bool SQFuncState::IsLocal(SQUnsignedInteger stkpos)
{
	if(stkpos>=_vlocals.size())return false;
	else if(sq_type(_vlocals[stkpos]._name)!=OT_NULL)return true;
	return false;
}

SQInteger SQFuncState::PushLocalVariable(const SQObject &name, SQInteger scope, SQInteger type)
{
	SQInteger pos=_vlocals.size();
	SQLocalVarInfo lvi;
	lvi._name=name;
	lvi._start_op=GetCurrentPos()+1;
	lvi._pos=_vlocals.size();
	lvi._scope=scope;
	lvi._type=type;
	_vlocals.push_back(lvi);
	if(_vlocals.size()>((SQUnsignedInteger)_stacksize))_stacksize=_vlocals.size();
//printf("%d %d %d %d %d %s\n", __LINE__, scope, type, pos, lvi._start_op, _stringval(lvi._name));
	return pos;
}



SQInteger SQFuncState::GetLocalVariable(const SQObject &name)
{
	SQInteger locals=_vlocals.size();
	while(locals>=1){
		SQLocalVarInfo &lvi = _vlocals[locals-1];
		if(sq_type(lvi._name)==OT_STRING && _string(lvi._name)==_string(name)){
			return locals-1;
		}
		locals--;
	}
	return -1;
}

void SQFuncState::MarkLocalAsOuter(SQInteger pos)
{
	SQLocalVarInfo &lvi = _vlocals[pos];
	lvi._end_op = UINT_MINUS_ONE;
	_outers++;
}

SQInteger SQFuncState::FindOuterVariable(const SQObject &name)
{
	SQInteger pos=-1;
	if(_parent) {
		pos = _parent->GetLocalVariable(name);
		if(pos == -1) {
			pos = _parent->FindOuterVariable(name);
		}
	}
	return pos;
}

SQInteger SQFuncState::GetOuterVariable(const SQObject &name)
{
	SQInteger outers = _outervalues.size();
	for(SQInteger i = 0; i<outers; i++) {
		if(_string(_outervalues[i]._name) == _string(name))
			return i;
	}
	SQInteger pos=-1;
	if(_parent) {
		pos = _parent->GetLocalVariable(name);
		if(pos == -1) {
			pos = _parent->GetOuterVariable(name);
			if(pos != -1) {
				_outervalues.push_back(SQOuterVar(name,SQObjectPtr(SQInteger(pos)),otOUTER)); //local
				return _outervalues.size() - 1;
			}
		}
		else {
			_parent->MarkLocalAsOuter(pos);
			_outervalues.push_back(SQOuterVar(name,SQObjectPtr(SQInteger(pos)),otLOCAL)); //local
			return _outervalues.size() - 1;
		}
	}
	return -1;
}

SQInteger SQFuncState::AddParameter(const SQObject &name, SQInteger scope, SQInteger type)
{
	SQInteger rc = PushLocalVariable(name, scope, type);
	if(rc < 0) return rc;
	_parameters.push_back(name);
	return rc;
}

void SQFuncState::AddParameterTypeName(const SQObject &type_name)
{
	_vlocals.top()._type_name = type_name;
}

void SQFuncState::AddParameterTypeName(const SQChar *type_name)
{
	if(type_name) AddParameterTypeName(CreateString(type_name, scstrlen(type_name)));
}

void SQFuncState::SetReturnTypeName(const SQChar *type_name)
{
	if(type_name) _return_type = CreateString(type_name, scstrlen(type_name));
}

SQInteger SQFuncState::FindGotoTarget(const SQObject &name)
{
	for(size_t i=0, len=_gototargets.size(); i < len; ++i){
        if(_string(_gototargets[i].name) == _string(name)) return i;
	}
	return -1;
}

bool SQFuncState::AddGotoTarget(const SQObject &name, SQInteger line, SQInteger traps, SQInteger nested)
{
    if(FindGotoTarget(name) >= 0) return false;
	SQGotoLabelsInfo info;
	//GetConstant(name);
	info.name = name;
	info.pos = GetCurrentPos();
	info.line = line;
	info.traps = traps;
	info.nested = nested;
	_gototargets.push_back(info);
	return true;
}

void SQFuncState::AddLineInfos(SQInteger line,bool lineop,bool force)
{
	if(_lastline!=line || force){
		SQLineInfo li;
		li._line=line;li._op=(GetCurrentPos()+1);
		if(lineop)AddInstruction(_OP_LINE,0,line);
		if(_lastline!=line) {
			_lineinfos.push_back(li);
		}
		_lastline=line;
	}
}

void SQFuncState::DiscardTarget()
{
	SQInteger discardedtarget = PopTarget();
	SQInteger size = _instructions.size();
	if(size > 0 && _optimization){
		SQInstruction &pi = _instructions[size-1];//previous instruction
		switch(pi.op) {
		case _OP_SET:case _OP_NEWSLOT:case _OP_SETOUTER:case _OP_CALL:
			if(pi._arg0 == discardedtarget) {
				pi._arg0 = 0xFF;
			}
		}
	}
}

void SQFuncState::AddInstruction(SQInstruction &i)
{
	SQInteger size = _instructions.size();
	if(size > 0 && _optimization){ //simple optimizer
		SQInstruction &pi = _instructions[size-1];//previous instruction
		switch(i.op) {
		case _OP_JZ:
			if( pi.op == _OP_CMP && pi._arg1 < 0xFF) {
				pi.op = _OP_JCMP;
				pi._arg0 = (unsigned char)pi._arg1;
				pi._arg1 = i._arg1;
				return;
			}
			break;
		case _OP_SET:
		case _OP_NEWSLOT:
			if(i._arg0 == i._arg3) {
				i._arg0 = 0xFF;
			}
			break;
		case _OP_SETOUTER:
			if(i._arg0 == i._arg2) {
				i._arg0 = 0xFF;
			}
			break;
		case _OP_RETURN:
			if( _parent && i._arg0 != MAX_FUNC_STACKSIZE && pi.op == _OP_CALL && _returnexp < size-1) {
				pi.op = _OP_TAILCALL;
			} else if(pi.op == _OP_CLOSE){
				pi = i;
				return;
			}
		break;
		case _OP_GET:
			if( pi.op == _OP_LOAD && pi._arg0 == i._arg2 && (!IsLocal(pi._arg0))){
				pi._arg2 = (unsigned char)i._arg1;
				pi.op = _OP_GETK;
				pi._arg0 = i._arg0;

				return;
			}
		break;
		case _OP_PREPCALL:
			if( pi.op == _OP_LOAD  && pi._arg0 == i._arg1 && (!IsLocal(pi._arg0))){
				pi.op = _OP_PREPCALLK;
				pi._arg0 = i._arg0;
				pi._arg2 = i._arg2;
				pi._arg3 = i._arg3;
				return;
			}
			break;
		case _OP_APPENDARRAY: {
			SQInteger aat = -1;
			switch(pi.op) {
			case _OP_LOAD: aat = AAT_LITERAL; break;
			case _OP_LOADINT: aat = AAT_INT; break;
			case _OP_LOADBOOL: aat = AAT_BOOL; break;
			case _OP_LOADFLOAT: aat = AAT_FLOAT; break;
			default: break;
			}
			if(aat != -1 && pi._arg0 == i._arg1 && (!IsLocal(pi._arg0))){
				pi.op = _OP_APPENDARRAY;
				pi._arg0 = i._arg0;
				pi._arg2 = (unsigned char)aat;
				pi._arg3 = MAX_FUNC_STACKSIZE;
				return;
			}
            		}
			break;
		case _OP_MOVE:
			switch(pi.op) {
			case _OP_GET: case _OP_ADD: case _OP_SUB: case _OP_MUL: case _OP_DIV: case _OP_MOD: case _OP_BITW:
			case _OP_LOADINT: case _OP_LOADFLOAT: case _OP_LOADBOOL: case _OP_LOAD:

				if(pi._arg0 == i._arg1)
				{
					pi._arg0 = i._arg0;
					_optimization = false;
					//_result_elimination = false;
					return;
				}
			}

			if(pi.op == _OP_MOVE)
			{
				pi.op = _OP_DMOVE;
				pi._arg2 = i._arg0;
				pi._arg3 = (unsigned char)i._arg1;
				return;
			}
			break;
		case _OP_LOAD:
			if(pi.op == _OP_LOAD && i._arg1 < 256) {
				pi.op = _OP_DLOAD;
				pi._arg2 = i._arg0;
				pi._arg3 = (unsigned char)i._arg1;
				return;
			}
			break;
		case _OP_EQ:case _OP_NE:
			if(pi.op == _OP_LOAD && pi._arg0 == i._arg1 && (!IsLocal(pi._arg0) ))
			{
				pi.op = i.op;
				pi._arg0 = i._arg0;
				pi._arg2 = i._arg2;
				pi._arg3 = MAX_FUNC_STACKSIZE;
				return;
			}
			break;
		case _OP_LOADNULLS:
			if((pi.op == _OP_LOADNULLS && pi._arg0+pi._arg1 == i._arg0)) {

				pi._arg1 = pi._arg1 + 1;
				pi.op = _OP_LOADNULLS;
				return;
			}
            break;
		case _OP_LINE:
			if(pi.op == _OP_LINE) {
				_instructions.pop_back();
				_lineinfos.pop_back();
			}
			break;
		}
	}
	_optimization = true;
	_instructions.push_back(i);
}

SQObject SQFuncState::CreateString(const SQChar *s,SQInteger len)
{
	SQObjectPtr ns(SQString::Create(_sharedstate,s,len));
	_table(_strings)->NewSlot(ns,(SQInteger)1);
	return SQCPPMOVE(ns);
}

SQObject SQFuncState::CreateTable()
{
	SQObjectPtr nt(SQTable::Create(_sharedstate,0));
	_table(_strings)->NewSlot(nt,(SQInteger)1);
	return SQCPPMOVE(nt);
}

SQFunctionProto *SQFuncState::BuildProto()
{

	SQFunctionProto *f=SQFunctionProto::Create(_ss,_instructions.size(),
		_nliterals,_parameters.size(),_functions.size(),_outervalues.size(),
		_lineinfos.size(),_localvarinfos.size(),_defaultparams.size());

	SQObjectPtr refidx,key,val;
	SQInteger idx;

	f->_stacksize = _stacksize;
	f->_sourcename = _sourcename;
	f->_bgenerator = _bgenerator;
	f->_name = _name;
	f->_return_type = _return_type;

	while((idx=_table(_literals)->Next(false,refidx,key,val))!=-1) {
		f->_literals[_integer(val)]=key;
		refidx=idx;
	}

	for(SQUnsignedInteger nf = 0; nf < _functions.size(); nf++) f->_functions[nf] = _functions[nf];
	for(SQUnsignedInteger np = 0; np < _parameters.size(); np++) f->_parameters[np] = _parameters[np];
	for(SQUnsignedInteger np = 0; np < _parameters.size(); np++) f->_parameters_type[np] = _vlocals[np]._type_name;
	for(SQUnsignedInteger no = 0; no < _outervalues.size(); no++) f->_outervalues[no] = _outervalues[no];
	for(SQUnsignedInteger nl = 0; nl < _localvarinfos.size(); nl++) f->_localvarinfos[nl] = _localvarinfos[nl];
	for(SQUnsignedInteger ni = 0; ni < _lineinfos.size(); ni++) f->_lineinfos[ni] = _lineinfos[ni];
	for(SQUnsignedInteger nd = 0; nd < _defaultparams.size(); nd++) f->_defaultparams[nd] = _defaultparams[nd];

	memcpy(f->_instructions,&_instructions[0],_instructions.size()*sizeof(SQInstruction));

	f->_varparams = _varparams;

#ifdef SQ_JIT_LLVM
	f->JitCompile();
#endif

	return f;
}

SQFuncState *SQFuncState::PushChildState(SQSharedState *ss)
{
	SQFuncState *child = (SQFuncState *)sq_malloc(sizeof(SQFuncState));
	new (child) SQFuncState(ss,this,_errfunc,_errtarget);
	_childstates.push_back(child);
	return child;
}

void SQFuncState::PopChildState()
{
	SQFuncState *child = _childstates.back();
	sq_delete(child,SQFuncState);
	_childstates.pop_back();
}

SQFuncState::~SQFuncState()
{
	while(_childstates.size() > 0)
	{
		PopChildState();
	}
}

#endif
