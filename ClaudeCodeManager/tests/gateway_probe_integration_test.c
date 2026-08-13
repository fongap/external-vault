#define mainCRTStartup ccm_application_entry
#include "../source/ClaudeCodeManager.c"
#undef mainCRTStartup

void mainCRTStartup(void) {
    static BYTE response[4096];
    DWORD len=0,status=0,error=0;
    WCHAR messages_url[256];

    if(!http_get_bytes(L"http://127.0.0.1:18765/v1/models",L"test-key",L"",response,sizeof(response),&len,&status,&error))ExitProcess(20);
    if(status!=200)ExitProcess(21);
    parse_models_from_http(response,len);
    if(g_model_count!=1||!weq_ci(g_models[0],L"claude-sonnet-test"))ExitProcess(22);

    build_anthropic_messages_url(L"http://127.0.0.1:18765",messages_url,256);
    build_probe_body(g_models[0]);
    if(!http_probe_anthropic_messages(messages_url,L"test-key",L"",&status,&error))ExitProcess(23);
    /* The mock rejects combined headers and Bearer, then accepts x-api-key. */
    if(status!=422)ExitProcess(24);
    ExitProcess(0);
}
