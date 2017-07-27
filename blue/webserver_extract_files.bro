@load base/protocols/http

module WebRequests;

export
{
    redef enum Log::ID += { LOG };
    
    type Request: record
    {
        ts:                 string      &log;
        source:             addr        &log;
        dest:               addr        &log;
        dest_port:          port        &log;
        method:             string      &log &optional;
        host:               string      &log &optional;
        uri:                string      &log &optional;
        referrer:           string      &log &optional;
        user_agent:         string      &log &optional;
        content_length:     count       &log &optional;
        basic_auth_user:    string      &log &optional;
    };
}

global req: Request;

event http_all_headers(c: connection, is_orig: bool, hlist: mime_header_list)
{
    if (is_orig)
    {
        
        req$ts                                              = strftime("%Y/%m/%d %H:%M:%S", c$http$ts);
        req$source                                          = c$id$orig_h;
        req$dest                                            = c$id$resp_h;
        req$dest_port                                       = c$id$resp_p;
        if (c$http?$method) req$method                      = c$http$method;
        if (c$http?$host) req$host                          = c$http$host;
        if (c$http?$uri) req$uri                            = c$http$uri;
        if (c$http?$referrer) req$referrer                  = c$http$referrer;
        if (c$http?$user_agent) req$user_agent              = c$http$user_agent;
        if (c$http?$request_body_len) req$content_length    = c$http$request_body_len;
        if (c$http?$username) req$basic_auth_user           = c$http$username;
        
    }
}

event file_sniff(f: fa_file, meta: fa_metadata)  
{  
    if(req$method == "POST" && req$uri == "/dvwa/vulnerabilities/upload/")
    {
    	Files::add_analyzer(f, Files::ANALYZER_EXTRACT);
    }
}

