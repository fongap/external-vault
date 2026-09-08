#define mainCRTStartup ccm_application_entry
#include "../source/AgentDock.c"
#undef mainCRTStartup

/* Codex config.toml transform tests: managed-key replacement, verbatim
   preservation of user content, idempotency, foreign-provider isolation and
   fail-closed behaviour on malformed or duplicated input. */

static int has(LPCWSTR text,LPCWSTR needle){return wfind_exact_from(text,needle,0)>=0;}
static int count_of(LPCWSTR text,LPCWSTR needle){int count=0,pos=0,found;while((found=wfind_exact_from(text,needle,pos))>=0){count++;pos=found+1;}return count;}

static const WCHAR USER_TOML[]=
L"# user preamble\r\n"
L"model = \"o4-mini\"\r\n"
L"approval_policy = \"untrusted\"\r\n"
L"\r\n"
L"[sandbox_workspace_write]\r\n"
L"network_access = false\r\n"
L"\r\n"
L"[profiles.fast]\r\n"
L"model = \"gpt-4.1\"\r\n"
L"model_reasoning_effort = \"low\"\r\n";

void mainCRTStartup(void) {
    WCHAR out[262144],out2[262144];

    /* 1. Fresh file (no existing config.toml): gateway mode. */
    if(!codex_toml_transform(L"",FALSE,TRUE,FALSE,L"gpt-5.1",L"high",L"https://gw.example.com/v1",out,262144))ExitProcess(10);
    if(!has(out,L"model = \"gpt-5.1\"")||!has(out,L"model_reasoning_effort = \"high\""))ExitProcess(11);
    if(!has(out,L"model_provider = \"ccm\"")||!has(out,L"[model_providers.ccm]"))ExitProcess(12);
    if(!has(out,L"base_url = \"https://gw.example.com/v1\"")||!has(out,L"env_key = \"CODEX_API_KEY\"")||!has(out,L"wire_api = \"responses\""))ExitProcess(13);
    /* Idempotent: transforming the output again must be byte-stable. */
    if(!codex_toml_transform(out,TRUE,TRUE,FALSE,L"gpt-5.1",L"high",L"https://gw.example.com/v1",out2,262144))ExitProcess(14);
    if(!weq(out,out2))ExitProcess(15);

    /* 2. Merge into an existing user config: replace managed keys only. */
    if(!codex_toml_transform(USER_TOML,TRUE,TRUE,FALSE,L"gpt-5.1",L"high",L"https://gw.example.com/v1",out,262144))ExitProcess(20);
    if(!has(out,L"# user preamble"))ExitProcess(21);
    if(!has(out,L"model = \"gpt-5.1\"")||has(out,L"model = \"o4-mini\""))ExitProcess(22);
    if(!has(out,L"approval_policy = \"untrusted\""))ExitProcess(23);
    /* Profile-scoped keys must survive untouched. */
    if(!has(out,L"[profiles.fast]")||!has(out,L"model = \"gpt-4.1\"")||!has(out,L"model_reasoning_effort = \"low\""))ExitProcess(24);
    if(!has(out,L"[sandbox_workspace_write]")||!has(out,L"network_access = false"))ExitProcess(25);
    /* Managed insertions must be top-level (before the first header). */
    {int provider_pos,section_pos,header_pos;
     provider_pos=wfind_exact_from(out,L"model_provider = \"ccm\"",0);
     section_pos=wfind_exact_from(out,L"[model_providers.ccm]",0);
     header_pos=wfind_exact_from(out,L"[sandbox_workspace_write]",0);
     if(provider_pos<0||section_pos<0||header_pos<0||provider_pos>header_pos||section_pos>header_pos)ExitProcess(26);}
    if(count_of(out,L"model_provider = \"ccm\"")!=1)ExitProcess(27);
    /* Idempotent second pass. */
    if(!codex_toml_transform(out,TRUE,TRUE,FALSE,L"gpt-5.1",L"high",L"https://gw.example.com/v1",out2,262144))ExitProcess(28);
    if(!weq(out,out2))ExitProcess(29);

    /* 3. Switch back to official: drop managed provider + section, keep the
          rest, still replace the top-level model. */
    if(!codex_toml_transform(out,TRUE,FALSE,TRUE,L"gpt-5.1",L"high",L"",out2,262144))ExitProcess(30);
    if(has(out2,L"model_provider")||has(out2,L"[model_providers.ccm]")||has(out2,L"env_key"))ExitProcess(31);
    if(!has(out2,L"model = \"gpt-5.1\"")||!has(out2,L"[profiles.fast]")||!has(out2,L"model = \"gpt-4.1\"")||!has(out2,L"# user preamble"))ExitProcess(32);
    /* Idempotent as well. */
    {WCHAR out3[262144];
     if(!codex_toml_transform(out2,TRUE,FALSE,TRUE,L"gpt-5.1",L"high",L"",out3,262144))ExitProcess(33);
     if(!weq(out2,out3))ExitProcess(34);}

    /* 4. Foreign model_provider is never removed and never clobbered. */
    if(!codex_toml_transform(L"model_provider = \"openai\"\r\n\r\n[profiles.x]\r\nmodel = \"m\"\r\n",TRUE,FALSE,TRUE,L"m2",L"",L"",out,262144))ExitProcess(40);
    if(!has(out,L"model_provider = \"openai\"")||has(out,L"[model_providers.ccm]"))ExitProcess(41);
    if(!has(out,L"model = \"m2\""))ExitProcess(42);
    /* Gateway mode must take ownership of the assignment. */
    if(!codex_toml_transform(L"model_provider = \"openai\"\r\n",TRUE,TRUE,FALSE,L"m",L"",L"https://gw/v1",out,262144))ExitProcess(43);
    if(count_of(out,L"model_provider = \"ccm\"")!=1||has(out,L"model_provider = \"openai\""))ExitProcess(44);

    /* 5. Literal-string provider value ('ccm') is recognised on clear. */
    if(!codex_toml_transform(L"model_provider = 'ccm'\r\n",TRUE,FALSE,TRUE,L"",L"",L"",out,262144))ExitProcess(50);
    if(has(out,L"model_provider"))ExitProcess(51);

    /* 6. Fail-closed: invalid TOML, duplicates, malformed headers. */
    if(codex_toml_transform(L"model = \"unterminated\r\n",TRUE,TRUE,FALSE,L"m",L"",L"",out,262144))ExitProcess(60);
    if(codex_toml_transform(L"model = \"a\"\r\nmodel = \"b\"\r\n",TRUE,TRUE,FALSE,L"m",L"",L"",out,262144))ExitProcess(61);
    if(codex_toml_transform(L"model_provider = \"ccm\"\r\nmodel_provider = \"ccm\"\r\n",TRUE,TRUE,FALSE,L"m",L"",L"",out,262144))ExitProcess(62);
    if(codex_toml_transform(L"[model_providers.ccm]\r\nbase_url = \"a\"\r\n\r\n[model_providers.ccm]\r\nbase_url = \"b\"\r\n",TRUE,TRUE,FALSE,L"m",L"",L"",out,262144))ExitProcess(63);
    if(codex_toml_transform(L"[broken\r\n",TRUE,FALSE,FALSE,L"",L"",L"",out,262144))ExitProcess(64);
    /* Two managed sections in gateway mode is also a hard stop. */
    if(codex_toml_transform(L"[model_providers.ccm]\r\nbase_url = \"a\"\r\n\r\n[model_providers.ccm]\r\nbase_url = \"b\"\r\n",TRUE,TRUE,FALSE,L"m",L"",L"",out,262144))ExitProcess(65);

    /* 7. Validation primitives. */
    if(!toml_validate(L"a = \"x\" # tail comment\r\n[b]\rnc = '''\nmulti\nline'''\r\n"))ExitProcess(70);
    if(toml_validate(L"a = \"line\r\nbreak\""))ExitProcess(71);
    if(toml_validate(L"a = \"esc\\\""))ExitProcess(72);

    /* 8. Escaping: quotes and backslashes in values survive a round trip. */
    if(!codex_toml_transform(L"",FALSE,FALSE,FALSE,L"mo\"del\\x",L"",L"",out,262144))ExitProcess(80);
    if(!has(out,L"model = \"mo\\\"del\\\\x\""))ExitProcess(81);

    /* 9. LF-only files stay valid: managed keys are still found and replaced. */
    if(!codex_toml_transform(L"model = \"old\"\napproval_policy = \"never\"\n",TRUE,TRUE,FALSE,L"new",L"",L"https://gw/v1",out,262144))ExitProcess(90);
    if(!has(out,L"model = \"new\"")||has(out,L"model = \"old\"")||!has(out,L"approval_policy = \"never\""))ExitProcess(91);

    /* 10. Profile store: pack / unpack round trip (pure in-memory — the
           registry-backed load/store touch the real user hive and are
           therefore not exercised here). */
    {
        CCM_PROFILE *p;
        static WCHAR snapshot[262144];
        g_profile_count=0;
        p=&g_profiles[g_profile_count++];memset(p,0,sizeof(*p));
        p->id=3;wcopy(p->name,64,L"主网关");p->kind=1;wcopy(p->base,2048,L"https://gw.example.com/v1");
        wcopy(p->defm,1024,L"gpt-5.1");wcopy(p->opus,1024,L"o1");wcopy(p->sonnet,1024,L"gpt-4.1");wcopy(p->effort,16,L"high");
        p=&g_profiles[g_profile_count++];memset(p,0,sizeof(*p));
        p->id=9;wcopy(p->name,64,L"官方");p->kind=0;wcopy(p->defm,1024,L"default");
        profiles_pack();
        wcopy(snapshot,262144,g_profile_pack);
        g_profile_count=0;
        profiles_unpack(snapshot);
        if(g_profile_count!=2)ExitProcess(100);
        p=&g_profiles[0];
        if(p->id!=3||!weq(p->name,L"主网关")||p->kind!=1||!weq(p->base,L"https://gw.example.com/v1")||!weq(p->defm,L"gpt-5.1")||!weq(p->opus,L"o1")||!weq(p->sonnet,L"gpt-4.1")||!weq(p->haiku,L"")||!weq(p->effort,L"high"))ExitProcess(101);
        p=&g_profiles[1];
        if(p->id!=9||!weq(p->name,L"官方")||p->kind!=0||!weq(p->defm,L"default"))ExitProcess(102);
        /* Truncated tail must not crash or resurrect entries. */
        {WCHAR truncated[262144];wcopy(truncated,262144,snapshot);
         truncated[wlen(truncated)/2]=0;
         if(!profiles_unpack(truncated))ExitProcess(103);}
        /* Round-trip stability: pack(unpack(pack)) is byte-stable. */
        profiles_unpack(snapshot);profiles_pack();
        if(!weq(snapshot,g_profile_pack))ExitProcess(104);
    }

    ExitProcess(0);
}
