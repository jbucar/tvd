SAFE_SYMBOLS = ["list", "dict", "tuple", "set", "long", "float", "object",
                "bool", "callable", "True", "False", "dir",
                "frozenset", "getattr", "hasattr", "abs", "cmp", "complex",
                "divmod", "id", "pow", "round", "slice", "vars",
                "hash", "hex", "int", "isinstance", "issubclass", "len",
                "map", "filter", "max", "min", "oct", "chr", "ord", "range",
                "reduce", "repr", "str", "type", "zip", "xrange", "None",
                "Exception", "KeyboardInterrupt"]
# Also add the standard exceptions
__bi = __builtins__
if type(__bi) is not dict:
    __bi = __bi.__dict__
for k in __bi:
    if k.endswith("Error") or k.endswith("Warning"):
        SAFE_SYMBOLS.append(k)
del __bi


def create(sourceCode, name, additional_symbols=dict()):
  """
  Create a python function from the given source code
  
  \param sourceCode A python string containing the core of the
  function. Might include the return statement (or not), definition of
  local functions, classes, etc. Indentation matters !
  
  \param args The string representing the arguments to put in the function's
  prototype, such as "a, b", or "a=12, b",
  or "a=12, b=dict(akey=42, another=5)"

  \param additional_symbols A dictionary variable name =>
  variable/funcion/object to include in the generated function's
  closure

  The sourceCode will be executed in a restricted environment,
  containing only the python builtins that are harmless (such as map,
  hasattr, etc.). To allow the function to access other modules or
  functions or objects, use the additional_symbols parameter. For
  example, to allow the source code to access the re and sys modules,
  as well as a global function F named afunction in the sourceCode and
  an object OoO named ooo in the sourceCode, specify:
      additional_symbols = dict(re=re, sys=sys, afunction=F, ooo=OoO)

  \return A python function implementing the source code. It can be
  recursive: the (internal) name of the function being defined is:
  __TheFunction__. Its docstring is the initial sourceCode string.

  Tests show that the resulting function does not have any calling
  time overhead (-3% to +3%, probably due to system preemption aleas)
  compared to normal python function calls.
  """
  # Include the sourcecode as the code of a function __TheFunction__:
  #s = "def __TheFunction__(%s):\n" % args
  #s += "\t" + "\n\t".join(sourceCode.split('\n')) + "\n"
  s = sourceCode

  # Byte-compilation (optional)
  byteCode = compile(s, "<string>", 'exec')  

  # Setup the local and global dictionaries of the execution
  # environment for __TheFunction__
  bis   = dict() # builtins
  globs = dict()
  locs  = dict()

  # Setup a standard-compatible python environment
  bis["locals"]  = lambda: locs
  bis["globals"] = lambda: globs
  globs["__builtins__"] = bis
  globs["__name__"] = "SUBENV"
  globs["__doc__"] = sourceCode

  # Determine how the __builtins__ dictionary should be accessed
  if type(__builtins__) is dict:
    bi_dict = __builtins__
  else:
    bi_dict = __builtins__.__dict__

  # Include the safe symbols
  for k in SAFE_SYMBOLS:
    # try from current locals
    try:
      locs[k] = locals()[k]
      continue
    except KeyError:
      pass
    # Try from globals
    try:
      globs[k] = globals()[k]
      continue
    except KeyError:
      pass
    # Try from builtins
    try:
      bis[k] = bi_dict[k]
    except KeyError:
      # Symbol not available anywhere: silently ignored
      pass

  # Include the symbols added by the caller, in the globals dictionary
  globs.update(additional_symbols)

  # Finally execute the def __TheFunction__ statement:
  eval(byteCode, globs, locs)
  # As a result, the function is defined as the item __TheFunction__
  # in the locals dictionary
  fct = locs[name]
  # Attach the function to the globals so that it can be recursive
  del locs[name]
  globs[name] = fct
  # Attach the actual source code to the docstring
  fct.__doc__ = sourceCode
  return fct
  
