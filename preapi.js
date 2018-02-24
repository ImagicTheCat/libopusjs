Module["onRuntimeInitialized"] = function(){
  if(Module.onload)
    Module.onload();
  Module.loaded = true;
}
