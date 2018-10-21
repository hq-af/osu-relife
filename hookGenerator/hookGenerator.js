const hook = require('./hook.json');


var output = "";

output += "//---------- s.H_" + hook.methodName + "\n"; 

output += "typedef " + hook.returnType + "(__stdcall* " + hook.methodName.toUpperCase() + ")(";
for (var i=0; i<hook.args.length; i++)
  output += hook.args[i] + (i < hook.args.length - 1 ? ", " : "");
output += ");\n"

var detourH = "";
detourH += hook.returnType + " " + "__stdcall Detour" + hook.methodName + "(";
for (var i=0; i<hook.args.length; i++)
detourH += hook.args[i] + " arg" + i + (i < hook.args.length - 1 ? ", " : "");
detourH += ")";
output += detourH + ";\n";
var hookH = "";
hookH += "void Hook_" + hook.methodName + "()";
output += hookH + ";";
output += "\n//---------- e.H_" + hook.methodName + "\n\n"; 

output += "//---------- s.C_" + hook.methodName + "\n"; 

output += hook.methodName.toUpperCase() + " fps" + hook.methodName + " = NULL;\n\n"
output += hookH + ` {\n    auto target = GetProcAddress(LoadLibrary(L"${hook.dllName}"), "${hook.methodName}");\n    MH_CreateHook(target, &Detour${hook.methodName}, (LPVOID*)&fps${hook.methodName});\n    MH_EnableHook(target);\n    std::cout << "created hook for '${hook.dllName}@${hook.methodName}'" << std::endl;\n}\n\n`;

output += detourH + ` {\n    std::cout << "called '${hook.dllName}@${hook.methodName}'" << std::endl;\n\n    return fps${hook.methodName}(`;
for (var i=0; i<hook.args.length; i++)
  output += "arg" + i + (i < hook.args.length - 1 ? ", " : "");
output+= ");\n}";
output += "\n//---------- e.C_" + hook.methodName; 

console.log(output);