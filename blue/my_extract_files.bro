global mime_to_ext: table[string] of string = {  
    ["application/x-dosexec"] = "exe",  
    ["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = "doc", 
    ["application/msword"]="doc",
    ["text/rtf"] = "doc",
    ["application/rtf"]="rtf",

     
};  
  
event file_sniff(f: fa_file, meta: fa_metadata)  
{  
    if ( f$source != "HTTP" )  
        return;  
  
    if ( ! meta?$mime_type )  
        return;  
  
    if ( meta$mime_type !in mime_to_ext )  
        return;  
  
    local fname = fmt("%s-%s.%s", f$source, f$id, mime_to_ext[meta$mime_type]);  
    print fmt("Extracting file %s", fname);  
    Files::add_analyzer(f, Files::ANALYZER_EXTRACT, [$extract_filename=fname]);  
}
