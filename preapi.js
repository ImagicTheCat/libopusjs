Module["onRuntimeInitialized"] = function(){
  if(Module.onload)
    Module.onload();
  Module.loaded = true;
}

Module["locateFile"] = function(url){
  if(url == "libopus.wasm" && typeof LIBOPUS_WASM_URL != "undefined")
    return LIBOPUS_WASM_URL;
  else
    return url;
}
