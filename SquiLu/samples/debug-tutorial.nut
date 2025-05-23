// Load up the debugger module and assign it to a variable.
local dbg = dofile("debugger.nut");
print("");

print([==[
	Welcome to the interactive debugger.lua tutorial.
	You'll want to open tutorial.nut in an editor to follow along.
]==]);

print([==[
	You are now in the debugger! (Woo! \o/).
	debugger.nut doesn't support traditional breakpoints.
	Instead you call the dbg() object to invoke the debugger.
	Real break points would be better, but this
	keeps debugger.lua simple and very fast.
	
	Notice how it prints out your current file and
	line as well as which function you are in.
	Keep a close watch on this as you follow along.
	It should be stopped a line after the dbg() call.
	(Line 80 probably?)
	
	Sometimes functions don't have global names.
	It might print the method name, local variable
	that held the function, or file:line where it starts.
	
	Type 's' to step to the next line.
	(s = Step to the next executable line)
]==]);

// Multi-line strings are executable statements apparently
// need to put this in an local to make the tutorial flow nicely.
local str1 = [==[
	The 's' command steps to the next executable line.
	This may step you into a function call.
	
	In this case, then next line was a C function that printed this message.
	You can't step into C functions, so it just steps over them.
	
	If you hit <return>, the debugger will rerun your last command.
	Hit <return> 5 times to step into and through func1().
	Watch the line numbers.
]==];

local str2 = [==[
	Stop!
	You've now stepped through func1()
	Notice how entering and exiting a function takes a step.
	
	Now try the 'n' command.
	(n = step to the Next line in the source code)
]==];

local function func1()
{
	print("	Stepping through func1()...");
	print("	Almost there...");
}

local function func2()
{
	print("	You used the 'n' command.");
	print("	So it's skipping over the lines in func2().");
	
	local function f()
	{
		print("	... and anything it might call.");
	}
	
	f();
	
	print("");
	print([==[
	The 'n' command steps to the next line in the source file.
	Unlike the 's' command, it steps over function calls, and not into them.
	
	Now try the 'c' command to continue on to the next breakpoint.
	(c =  Continue execution)
]==]);
}

dbg();
print(str1);

func1();
print(str2);

func2();

local function func3()
{
	print([==[
	You are now sitting at a breakpoint inside of a func3().
	Let's say you got here by stepping into the function.
	After poking around for a bit, you just want to step until the
	function returns, but don't want to
	run the next command over and over.
	
	For this you would use the 'f' command. Try it now.
	(f = Finish current function)
]==]);
	
	dbg();
	
	print([==[
	Now you are inside func4(), right after where it called func3().
	func4() has some arguments, local variables and upvalues.
	Let's assume you want to see them.
	
	Try the 'l' command to list all the locally available variables.
	(l = List local variables)
	
	Type 'c' to continue on to the next section.
]==]);
}

local my_upvalue1 = "Wee an upvalue";
local my_upvalue2 = "Awww, can't see this one";
globalvar <- "Weeee a global";

function func4(a, b, ...)
{
	local c = "sea";
	local varargs_copy = clone vargv;
	
	// Functions only get upvalues if you reference them.
	local d = my_upvalue1 + " ... with stuff appended to it";
	
	func3();
	
	print([==[
	Some things to notice about the local variables list.
	'...'
	  This is the list of varargs passed to the function.
	  (This only works with Lua 5.2+ or LuaJIT 2.0+)
	  Note: varargs are not an array, but debugger.lua stores them that way.
	'my_upvalue1'
	  This is a local variable defined outside of, but
	  referenced by the function. Upvalues show up
	  *only* when you reference them within your
	  function. 'my_upvalue2' isn't in the list
	  because func4() doesn't reference it.
	
	Listing the locals is nice, but sometimes there are too many to see at once.
	Often times it's useful to print just a single variable,
	evaluate an expression, or call a function to see what it returns.
	For that you use the 'p' command.
	(p = Print or evaluate an expression)
	
	Try these commands:
	p my_upvalue1
	p 1 + 1
	p print("foo")
	p math.cos(0)
	
	You can also interact with varargs. (Except on Lua 5.1)
	For example:
	p ...
	p select(2, ...)
	p {...}
	
	Type 'c' to continue to the next section.
]==]);
	dbg();
}

func4(1, "two", "vararg1", "vararg2", "vararg3");

local function func5()
{
	local my_var = "func5()";
	print([==[
	You are now in func5() which was called from func6().
	func6() was called from func7().
	
	Try the 't' command to print out a backtrace and see for yourself.
	(t = backTrace)
	
	Type 'c' to continue to the next section
]==]);
	dbg();
	
	print([==[
	Notice that func5(), func6() and func7() all have a
	'my_var' local. You can print the func5()'s my_var easily enough.
	What if you wanted to see what local variables were in func6()
	or func7() to see how you got where you were?
	
	For that you use the 'u' and 'd' commands.
	(u = Move up a stack frame)
	(d = Move down a stack frame)
	
	Try the 'u' and 'd' commands a few times.
	Print out the value of my_var using the 'p' command each time.
	
	Type 'c' to continue.
]==]);
	dbg();
}

local function func6()
{
	local my_var = "func6()";
	func5();
}

local function func7()
{
	local my_var = "func7()";
	func6();
}

func7();

print([==[
	That leaves only one more command.
	Wouldn't it be nice if there was a way to remember
	all these one letter debugger commands?
	
	Type 'h' to show the command list.
	(h = Help)
	
	Type 'c' to continue.
]==]);
dbg();

print([==[
	The following loop uses an assert-style breakpoint.
	It will only engage when the conditional fails. (when i == 5)
	
	Type 'c' to continue.
]==]);

for(local i=0; i < 10; ++i)
{
	print("i = " + i);
	
	if(i == 5) dbg();
}

print([==[
	Last but not least, is the dbg.call() function.
	It works sort of like Lua's xpcall() function,
	but starts the debugger when an uncaught error occurs.
	Note that dbg.call() does *not* take a list of varargs though.
	You must call it on a function that takes no arguments.
	
	dbg.call(function()
	{
		// Potentially buggy code goes here.
	});
	
	Wrap it around your program's main loop or main entry point.
	Then when your program crashes, you won't need to go back
	and add breakpoints.
	
	That pretty much wraps ups the basics.
	Hopefully you find debugger.lua to be simple but useful.
]==]);

dbg.call(function()
{
	local foo = "foo";
	
	// Try adding a string and integer
	local bar = foo + 12;
	
	// Program never makes it to here...
});
