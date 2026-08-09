#define mainCRTStartup ccm_application_entry
#include "../source/ClaudeCodeManager.c"
#undef mainCRTStartup

void mainCRTStartup(void) {
    static BYTE response[]=
        "{\"data\":["
        "{\"id\":\"model-a\",\"context_length\":200000},"
        "{\"id\":\"model-b\",\"context_window\":\"1000000\"}"
        "]}";
    parse_models_from_http(response,(DWORD)(sizeof(response)-1));
    if(g_model_count!=2)ExitProcess(10);
    if(!weq_ci(g_models[0],L"model-a")||g_model_context_capacity[0]!=200000)ExitProcess(11);
    if(!weq_ci(g_models[1],L"model-b")||g_model_context_capacity[1]!=1000000)ExitProcess(12);
    ExitProcess(0);
}
