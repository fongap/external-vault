#define mainCRTStartup ccm_application_entry
#include "../source/ClaudeCodeManager.c"
#undef mainCRTStartup

void mainCRTStartup(void) {
    WCHAR url[256];
    static BYTE response[]=
        "{\"data\":["
        "{\"id\":\"model-a\",\"context_length\":200000},"
        "{\"id\":\"model-b\",\"context_window\":\"1000000\"}"
        "]}";
    parse_models_from_http(response,(DWORD)(sizeof(response)-1));
    if(g_model_count!=2)ExitProcess(10);
    if(!weq_ci(g_models[0],L"model-a")||g_model_context_capacity[0]!=200000)ExitProcess(11);
    if(!weq_ci(g_models[1],L"model-b")||g_model_context_capacity[1]!=1000000)ExitProcess(12);
    build_anthropic_messages_url(L"https://gateway.example/v1",url,256);
    if(!weq_ci(url,L"https://gateway.example/v1/messages"))ExitProcess(13);
    build_anthropic_messages_url(L"https://gateway.example/",url,256);
    if(!weq_ci(url,L"https://gateway.example/v1/messages"))ExitProcess(14);
    if(!is_unsupported_manager_field_native(L"CLAUDE_CODE_MAX_CONTEXT_TOKENS"))ExitProcess(15);
    build_probe_body(L"claude-sonnet-4\"test");
    if(ccm_probe_body_len<=0)ExitProcess(16);
    {
        static const char expected[]="{\"model\":\"claude-sonnet-4\\\"test\",\"max_tokens\":1,\"messages\":[{\"role\":\"user\",\"content\":\"ping\"}]}";
        unsigned int i=0;while(expected[i]&&ccm_probe_body[i]==expected[i])i++;
        if(expected[i]||ccm_probe_body[i])ExitProcess(17);
    }
    ExitProcess(0);
}
