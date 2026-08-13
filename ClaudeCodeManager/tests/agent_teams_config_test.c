#define mainCRTStartup ccm_application_entry
#include "../source/ClaudeCodeManager.c"
#undef mainCRTStartup

static int occurrences(LPCWSTR text,LPCWSTR needle){int count=0,pos=0,found;while((found=wfind_exact_from(text,needle,pos))>=0){count++;pos=found+1;}return count;}
static BOOL any_temp_for(LPCWSTR target){WCHAR pattern[512];WIN32_FIND_DATAW fd;HANDLE h;wcopy(pattern,512,target);wcat(pattern,512,L".ccm-agent-teams*.tmp");h=FindFirstFileW(pattern,&fd);if(h==INVALID_HANDLE_VALUE)return FALSE;FindClose(h);return TRUE;}

void mainCRTStartup(void) {
    WCHAR out[262144],out2[262144],captured[4096];BOOL external=FALSE;
    WCHAR settings_path[256],md_path[256],locked_md[256],pid[32],readback[1024];HANDLE lock;
    static const WCHAR existing[]=L"{\r\n  \"theme\": \"dark\",\r\n  \"env\": {\"KEEP\": \"yes\", \"CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS\": \"0\"},\r\n  \"nested\": {\"x\": true}\r\n}";
    static const WCHAR md[]=L"用户前言\r\n\r\n用户结尾\r\n";
    if(!agent_settings_transform(L"{}",TRUE,FALSE,L"",out,262144,captured,4096,&external))ExitProcess(10);
    if(!weq(captured,L"@absent")||wfind_ci(out,L"\"CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS\": \"1\"")<0)ExitProcess(11);
    if(!agent_settings_transform(out,TRUE,TRUE,captured,out2,262144,0,0,&external)||!weq(out,out2))ExitProcess(12);
    if(!agent_settings_transform(out,FALSE,TRUE,captured,out2,262144,0,0,&external)||wfind_ci(out2,AGENT_TEAMS_ENV_NAME)>=0)ExitProcess(13);
    if(!agent_settings_transform(existing,TRUE,FALSE,L"",out,262144,captured,4096,&external)||!weq(captured,L"\"0\"")||wfind_ci(out,L"\"KEEP\": \"yes\"")<0||wfind_ci(out,L"\"nested\": {\"x\": true}")<0)ExitProcess(14);
    if(!agent_settings_transform(out,FALSE,TRUE,captured,out2,262144,0,0,&external)||wfind_ci(out2,L"\"CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS\": \"0\"")<0)ExitProcess(15);
    if(agent_settings_transform(L"[]",TRUE,FALSE,L"",out,262144,captured,4096,&external))ExitProcess(16);
    if(agent_settings_transform(L"",TRUE,FALSE,L"",out,262144,captured,4096,&external))ExitProcess(160);
    if(agent_settings_transform(L"{\"env\":{},\"env\":{}}",TRUE,FALSE,L"",out,262144,captured,4096,&external))ExitProcess(17);
    if(!agent_settings_transform(L"{\"ENV\":{\"KEEP\":true}}",TRUE,FALSE,L"",out,262144,captured,4096,&external))ExitProcess(170);
    {CCM_AGENT_SETTINGS_INFO case_info;if(!agent_settings_inspect(out,g_json_target_tokens,&g_json_target_count,&case_info)||!case_info.target_is_one||wfind_exact_from(out,L"\"ENV\"",0)<0||wfind_exact_from(out,L"\"env\"",0)<0)ExitProcess(171);}
    if(!agent_settings_transform(L"{\"env\":{\"CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS\":\"custom\"}}",FALSE,TRUE,L"@absent",out,262144,0,0,&external)||!external||wfind_ci(out,L"custom")<0)ExitProcess(18);
    if(!agent_rules_transform(md,TRUE,out,262144)||occurrences(out,AGENT_TEAMS_MARKER_START)!=1||wfind_ci(out,L"用户前言")<0)ExitProcess(19);
    if(!agent_rules_transform(out,TRUE,out2,262144)||!weq(out,out2))ExitProcess(20);
    if(!agent_rules_transform(out,FALSE,out2,262144))ExitProcess(21);
    if(wlen(out2)!=wlen(md))ExitProcess(2100+wlen(out2));
    if(!weq(out2,md))ExitProcess(22);
    if(agent_rules_transform(L"before <!-- ClaudeCodeManager:agent-teams:start --> broken",TRUE,out,262144))ExitProcess(23);
    if(agent_rules_transform(L"<!-- ClaudeCodeManager:agent-teams:start --><!-- ClaudeCodeManager:agent-teams:start --><!-- ClaudeCodeManager:agent-teams:end -->",TRUE,out,262144))ExitProcess(24);
    /* Explicit workspace-local paths prove the transaction core is testable
       without consulting or mutating the real USERPROFILE. */
    uint_to_wstr(GetCurrentProcessId(),pid,32);wcopy(settings_path,256,L"build\\agent-teams-settings-");wcat(settings_path,256,pid);wcat(settings_path,256,L".json");wcopy(md_path,256,L"build\\agent-teams-claude-");wcat(md_path,256,pid);wcat(md_path,256,L".md");DeleteFileW(settings_path);DeleteFileW(md_path);
    if(agent_commit_files(settings_path,L"",FALSE,L"{\r\n  \"env\": {}\r\n}\r\n",md_path,L"",FALSE,L"rules\r\n")!=0)ExitProcess(25);
    if(GetFileAttributesW(settings_path)==INVALID_FILE_ATTRIBUTES||GetFileAttributesW(md_path)==INVALID_FILE_ATTRIBUTES)ExitProcess(26);
    DeleteFileW(settings_path);DeleteFileW(md_path);
    /* Lock the second target without FILE_SHARE_DELETE so its atomic replace
       deterministically fails after settings was replaced. The transaction
       must restore settings from its byte-exact backup. */
    wcopy(settings_path,256,L"build\\agent-teams-settings-rollback-");wcat(settings_path,256,pid);wcat(settings_path,256,L".json");wcopy(locked_md,256,L"build\\agent-teams-claude-locked-");wcat(locked_md,256,pid);wcat(locked_md,256,L".md");DeleteFileW(settings_path);DeleteFileW(locked_md);
    if(!write_utf8_text_file(settings_path,L"{\"before\":true}" )||!write_utf8_text_file(locked_md,L"before"))ExitProcess(27);
    lock=CreateFileW(locked_md,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(lock==INVALID_HANDLE_VALUE)ExitProcess(28);
    if(agent_commit_files(settings_path,L"{\"before\":true}",TRUE,L"{\"after\":true}",locked_md,L"before",TRUE,L"after")!=26){CloseHandle(lock);ExitProcess(29);}CloseHandle(lock);
    if(!read_text_file_w_strict(settings_path,readback,1024)||!weq(readback,L"{\"before\":true}"))ExitProcess(30);
    if(any_temp_for(settings_path)||any_temp_for(locked_md))ExitProcess(31);
    DeleteFileW(settings_path);DeleteFileW(locked_md);
    ExitProcess(0);
}
