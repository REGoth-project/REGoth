//
// Created by andre on 05.06.16.
//

#include <utils/logger.h>
#include "Stubs.h"
#include <daedalus/DaedalusVM.h>

void ::Logic::ScriptExternals::registerStubs(Daedalus::DaedalusVM& vm, bool verbose)
{
    vm.registerExternalFunction("print", [=](Daedalus::DaedalusVM& vm){
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

    });

    vm.registerExternalFunction("printmulti", [=](Daedalus::DaedalusVM& vm){
        std::string s4 = vm.popString(); if(verbose) LogInfo() << "s4: " << s4;
        std::string s3 = vm.popString(); if(verbose) LogInfo() << "s3: " << s3;
        std::string s2 = vm.popString(); if(verbose) LogInfo() << "s2: " << s2;
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

    });

    vm.registerExternalFunction("printdebug", [=](Daedalus::DaedalusVM& vm){
        std::string s = vm.popString(); if(verbose) LogInfo() << "s: " << s;

    });

    vm.registerExternalFunction("printscreen", [=](Daedalus::DaedalusVM& vm){
        int32_t timesec = vm.popDataValue(); if(verbose) LogInfo() << "timesec: " << timesec;
        std::string font = vm.popString(); if(verbose) LogInfo() << "font: " << font;
        int32_t posy = vm.popDataValue(); if(verbose) LogInfo() << "posy: " << posy;
        int32_t posx = vm.popDataValue(); if(verbose) LogInfo() << "posx: " << posx;
        std::string msg = vm.popString(); if(verbose) LogInfo() << "msg: " << msg;
        int32_t dialognr = vm.popDataValue(); if(verbose) LogInfo() << "dialognr: " << dialognr;

    });

    vm.registerExternalFunction("hlp_random", [=](Daedalus::DaedalusVM& vm){
        int32_t n0 = vm.popDataValue(); if(verbose) LogInfo() << "n0: " << n0;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_strcmp", [=](Daedalus::DaedalusVM& vm){
        std::string s2 = vm.popString(); if(verbose) LogInfo() << "s2: " << s2;
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_isvalidnpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_isvaliditem", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_isitem", [=](Daedalus::DaedalusVM& vm){
        int32_t instancename = vm.popDataValue(); if(verbose) LogInfo() << "instancename: " << instancename;

        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_getnpc", [=](Daedalus::DaedalusVM& vm){
        int32_t instancename = vm.popDataValue(); if(verbose) LogInfo() << "instancename: " << instancename;
        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("hlp_getinstanceid", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("hlp_getinstanceid", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_wait", [=](Daedalus::DaedalusVM& vm){
        float n1 = vm.popFloatValue(); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_getstatetime", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_setstatetime", [=](Daedalus::DaedalusVM& vm){
        int32_t seconds = vm.popDataValue(); if(verbose) LogInfo() << "seconds: " << seconds;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("wld_istime", [=](Daedalus::DaedalusVM& vm){
        int32_t min2 = vm.popDataValue(); if(verbose) LogInfo() << "min2: " << min2;
        int32_t hour2 = vm.popDataValue(); if(verbose) LogInfo() << "hour2: " << hour2;
        int32_t min1 = vm.popDataValue(); if(verbose) LogInfo() << "min1: " << min1;
        int32_t hour1 = vm.popDataValue(); if(verbose) LogInfo() << "hour1: " << hour1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_insertnpc", [=](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(verbose) LogInfo() << "spawnpoint: " << spawnpoint;
        int32_t npcinstance = vm.popDataValue(); if(verbose) LogInfo() << "npcinstance: " << npcinstance;

    });

    vm.registerExternalFunction("wld_insertnpcandrespawn", [=](Daedalus::DaedalusVM& vm){
        float spawndelay = vm.popFloatValue(); if(verbose) LogInfo() << "spawndelay: " << spawndelay;
        std::string spawnpoint = vm.popString(); if(verbose) LogInfo() << "spawnpoint: " << spawnpoint;
        int32_t instance = vm.popDataValue(); if(verbose) LogInfo() << "instance: " << instance;

    });

    vm.registerExternalFunction("ai_playani", [=](Daedalus::DaedalusVM& vm){
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_standup", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_standupquick", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_quicklook", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_lookat", [=](Daedalus::DaedalusVM& vm){
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_lookatnpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_stoplookat", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_pointat", [=](Daedalus::DaedalusVM& vm){
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_pointatnpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_stoppointat", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_takeitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_dropitem", [=](Daedalus::DaedalusVM& vm){
        int32_t itemid = vm.popDataValue(); if(verbose) LogInfo() << "itemid: " << itemid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_useitem", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_useitemtostate", [=](Daedalus::DaedalusVM& vm){
        int32_t state = vm.popDataValue(); if(verbose) LogInfo() << "state: " << state;
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_usemob", [=](Daedalus::DaedalusVM& vm){
        int32_t targetstate = vm.popDataValue(); if(verbose) LogInfo() << "targetstate: " << targetstate;
        std::string schemename = vm.popString(); if(verbose) LogInfo() << "schemename: " << schemename;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_ismobavailable", [=](Daedalus::DaedalusVM& vm){
        std::string schemename = vm.popString(); if(verbose) LogInfo() << "schemename: " << schemename;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_getmobstate", [=](Daedalus::DaedalusVM& vm){
        std::string schemename = vm.popString(); if(verbose) LogInfo() << "schemename: " << schemename;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_setwalkmode", [=](Daedalus::DaedalusVM& vm){
        int32_t n0 = vm.popDataValue(); if(verbose) LogInfo() << "n0: " << n0;
        uint32_t arr_n;
        int32_t n = vm.popVar(arr_n); if(verbose) LogInfo() << "n: " << n;

    });

    vm.registerExternalFunction("ai_gotowp", [=](Daedalus::DaedalusVM& vm){
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_gotofp", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(); if(verbose) LogInfo() << "fpname: " << fpname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_gotonextfp", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(); if(verbose) LogInfo() << "fpname: " << fpname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_gotonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_gotoitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_gotosound", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_teleport", [=](Daedalus::DaedalusVM& vm){
        std::string waypoint = vm.popString(); if(verbose) LogInfo() << "waypoint: " << waypoint;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_getnearestwp", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn("");
    });

    vm.registerExternalFunction("npc_getnextwp", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn("");
    });

    vm.registerExternalFunction("wld_isfpavailable", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(); if(verbose) LogInfo() << "fpname: " << fpname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_isnextfpavailable", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(); if(verbose) LogInfo() << "fpname: " << fpname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isonfp", [=](Daedalus::DaedalusVM& vm){
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_iswayblocked", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_turntonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n1;
        int32_t n1 = vm.popVar(arr_n1); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_turnaway", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n1;
        int32_t n1 = vm.popVar(arr_n1); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_whirlaround", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_turntosound", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_aligntowp", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_dodge", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;

    });

    vm.registerExternalFunction("mdl_applyoverlaymds", [=](Daedalus::DaedalusVM& vm){
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("mdl_removeoverlaymds", [=](Daedalus::DaedalusVM& vm){
        std::string overlayname = vm.popString(); if(verbose) LogInfo() << "overlayname: " << overlayname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("mdl_applyoverlaymdstimed", [=](Daedalus::DaedalusVM& vm){
        float timeticks = vm.popFloatValue(); if(verbose) LogInfo() << "timeticks: " << timeticks;
        std::string overlayname = vm.popString(); if(verbose) LogInfo() << "overlayname: " << overlayname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("mdl_applyrandomani", [=](Daedalus::DaedalusVM& vm){
        std::string s2 = vm.popString(); if(verbose) LogInfo() << "s2: " << s2;
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("mdl_applyrandomanifreq", [=](Daedalus::DaedalusVM& vm){
        float f2 = vm.popFloatValue(); if(verbose) LogInfo() << "f2: " << f2;
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("mdl_startfaceani", [=](Daedalus::DaedalusVM& vm){
        float holdtime = vm.popFloatValue(); if(verbose) LogInfo() << "holdtime: " << holdtime;
        float intensity = vm.popFloatValue(); if(verbose) LogInfo() << "intensity: " << intensity;
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("mdl_applyrandomfaceani", [=](Daedalus::DaedalusVM& vm){
        float probmin = vm.popFloatValue(); if(verbose) LogInfo() << "probmin: " << probmin;
        float timemaxvar = vm.popFloatValue(); if(verbose) LogInfo() << "timemaxvar: " << timemaxvar;
        float timemax = vm.popFloatValue(); if(verbose) LogInfo() << "timemax: " << timemax;
        float timeminvar = vm.popFloatValue(); if(verbose) LogInfo() << "timeminvar: " << timeminvar;
        float timemin = vm.popFloatValue(); if(verbose) LogInfo() << "timemin: " << timemin;
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_getbodystate", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasbodyflag", [=](Daedalus::DaedalusVM& vm){
        int32_t bodyflag = vm.popDataValue(); if(verbose) LogInfo() << "bodyflag: " << bodyflag;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_playanibs", [=](Daedalus::DaedalusVM& vm){
        int32_t bodystate = vm.popDataValue(); if(verbose) LogInfo() << "bodystate: " << bodystate;
        std::string aniname = vm.popString(); if(verbose) LogInfo() << "aniname: " << aniname;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;

    });

    vm.registerExternalFunction("npc_settofistmode", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_settofightmode", [=](Daedalus::DaedalusVM& vm){
        int32_t weapon = vm.popDataValue(); if(verbose) LogInfo() << "weapon: " << weapon;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_isinfightmode", [=](Daedalus::DaedalusVM& vm){
        int32_t fmode = vm.popDataValue(); if(verbose) LogInfo() << "fmode: " << fmode;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_drawweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_readymeleeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_readyrangedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_removeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_getreadiedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_hasreadiedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasreadiedmeleeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasreadiedrangedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasrangedweaponwithammo", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_gettarget", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getnexttarget", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isnexttargetavailable", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_settarget", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_attack", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_finishingmove", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_defend", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_flee", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_aimat", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_attacker;
        int32_t attacker = vm.popVar(arr_attacker); if(verbose) LogInfo() << "attacker: " << attacker;

    });

    vm.registerExternalFunction("ai_shootat", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_attacker;
        int32_t attacker = vm.popVar(arr_attacker); if(verbose) LogInfo() << "attacker: " << attacker;

    });

    vm.registerExternalFunction("ai_stopaim", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_attacker;
        int32_t attacker = vm.popVar(arr_attacker); if(verbose) LogInfo() << "attacker: " << attacker;

    });

    vm.registerExternalFunction("npc_arewestronger", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isaiming", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_insertitem", [=](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(verbose) LogInfo() << "spawnpoint: " << spawnpoint;
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;

    });

    vm.registerExternalFunction("ai_lookforitem", [=](Daedalus::DaedalusVM& vm){
        int32_t instance = vm.popDataValue(); if(verbose) LogInfo() << "instance: " << instance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("wld_removeitem", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("createinvitem", [=](Daedalus::DaedalusVM& vm){
        int32_t n1 = vm.popDataValue(); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("createinvitems", [=](Daedalus::DaedalusVM& vm){
        int32_t n2 = vm.popDataValue(); if(verbose) LogInfo() << "n2: " << n2;
        int32_t n1 = vm.popDataValue(); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_getinvitem", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_hasitems", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getinvitembyslot", [=](Daedalus::DaedalusVM& vm){
        int32_t slotnr = vm.popDataValue(); if(verbose) LogInfo() << "slotnr: " << slotnr;
        int32_t category = vm.popDataValue(); if(verbose) LogInfo() << "category: " << category;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_removeinvitem", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_owner;
        int32_t owner = vm.popVar(arr_owner); if(verbose) LogInfo() << "owner: " << owner;

    });

    vm.registerExternalFunction("npc_removeinvitems", [=](Daedalus::DaedalusVM& vm){
        int32_t amount = vm.popDataValue(); if(verbose) LogInfo() << "amount: " << amount;
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_owner;
        int32_t owner = vm.popVar(arr_owner); if(verbose) LogInfo() << "owner: " << owner;

    });

    vm.registerExternalFunction("mob_createitems", [=](Daedalus::DaedalusVM& vm){
        int32_t amount = vm.popDataValue(); if(verbose) LogInfo() << "amount: " << amount;
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        std::string mobname = vm.popString(); if(verbose) LogInfo() << "mobname: " << mobname;

    });

    vm.registerExternalFunction("mob_hasitems", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        std::string mobname = vm.popString(); if(verbose) LogInfo() << "mobname: " << mobname;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("equipitem", [=](Daedalus::DaedalusVM& vm){
        int32_t n1 = vm.popDataValue(); if(verbose) LogInfo() << "n1: " << n1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("ai_equipbestmeleeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_equipbestrangedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_equipbestarmor", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_unequipweapons", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_unequiparmor", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_equiparmor", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_owner;
        int32_t owner = vm.popVar(arr_owner); if(verbose) LogInfo() << "owner: " << owner;

    });

    vm.registerExternalFunction("npc_getequippedmeleeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_getequippedrangedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_getequippedarmor", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_hasequippedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasequippedmeleeweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasequippedrangedweapon", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasequippedarmor", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_ownedbynpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;

        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_ownedbyguild", [=](Daedalus::DaedalusVM& vm){
        int32_t guild = vm.popDataValue(); if(verbose) LogInfo() << "guild: " << guild;

        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isdetectedmobownedbynpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_owner;
        int32_t owner = vm.popVar(arr_owner); if(verbose) LogInfo() << "owner: " << owner;
        uint32_t arr_user;
        int32_t user = vm.popVar(arr_user); if(verbose) LogInfo() << "user: " << user;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_giveitem", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n2;
        int32_t n2 = vm.popVar(arr_n2); if(verbose) LogInfo() << "n2: " << n2;

        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("npc_startitemreactmodules", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasoffered", [=](Daedalus::DaedalusVM& vm){
        int32_t iteminstance = vm.popDataValue(); if(verbose) LogInfo() << "iteminstance: " << iteminstance;
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("doc_open", [=](Daedalus::DaedalusVM& vm){
        std::string picname = vm.popString(); if(verbose) LogInfo() << "picname: " << picname;

    });

    vm.registerExternalFunction("doc_font", [=](Daedalus::DaedalusVM& vm){
        std::string fontname = vm.popString(); if(verbose) LogInfo() << "fontname: " << fontname;

    });

    vm.registerExternalFunction("doc_print", [=](Daedalus::DaedalusVM& vm){
        std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

    });

    vm.registerExternalFunction("doc_mapcoordinates", [=](Daedalus::DaedalusVM& vm){
        float pixely2 = vm.popFloatValue(); if(verbose) LogInfo() << "pixely2: " << pixely2;
        float pixelx2 = vm.popFloatValue(); if(verbose) LogInfo() << "pixelx2: " << pixelx2;
        float gamey2 = vm.popFloatValue(); if(verbose) LogInfo() << "gamey2: " << gamey2;
        float gamex2 = vm.popFloatValue(); if(verbose) LogInfo() << "gamex2: " << gamex2;
        float pixely1 = vm.popFloatValue(); if(verbose) LogInfo() << "pixely1: " << pixely1;
        float pixelx1 = vm.popFloatValue(); if(verbose) LogInfo() << "pixelx1: " << pixelx1;
        float gamey1 = vm.popFloatValue(); if(verbose) LogInfo() << "gamey1: " << gamey1;
        float gamex1 = vm.popFloatValue(); if(verbose) LogInfo() << "gamex1: " << gamex1;
        std::string levelname = vm.popString(); if(verbose) LogInfo() << "levelname: " << levelname;

    });

    vm.registerExternalFunction("ai_output", [=](Daedalus::DaedalusVM& vm){
        std::string outputname = vm.popString(); if(verbose) LogInfo() << "outputname: " << outputname;
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_outputsvm", [=](Daedalus::DaedalusVM& vm){
        std::string svmname = vm.popString(); if(verbose) LogInfo() << "svmname: " << svmname;
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_outputsvm_overlay", [=](Daedalus::DaedalusVM& vm){
        std::string svmname = vm.popString(); if(verbose) LogInfo() << "svmname: " << svmname;
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_waittillend", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_ask", [=](Daedalus::DaedalusVM& vm){


        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_asktext", [=](Daedalus::DaedalusVM& vm){
        std::string strno = vm.popString(); if(verbose) LogInfo() << "strno: " << strno;
        std::string stryes = vm.popString(); if(verbose) LogInfo() << "stryes: " << stryes;


        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_waitforquestion", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_setrefusetalk", [=](Daedalus::DaedalusVM& vm){
        int32_t timesec = vm.popDataValue(); if(verbose) LogInfo() << "timesec: " << timesec;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_refusetalk", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_memoryentry", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_victim;
        int32_t victim = vm.popVar(arr_victim); if(verbose) LogInfo() << "victim: " << victim;
        int32_t newsid = vm.popDataValue(); if(verbose) LogInfo() << "newsid: " << newsid;
        uint32_t arr_offender;
        int32_t offender = vm.popVar(arr_offender); if(verbose) LogInfo() << "offender: " << offender;
        int32_t source = vm.popDataValue(); if(verbose) LogInfo() << "source: " << source;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_memoryentryguild", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_victimguild;
        int32_t victimguild = vm.popVar(arr_victimguild); if(verbose) LogInfo() << "victimguild: " << victimguild;
        int32_t newsid = vm.popDataValue(); if(verbose) LogInfo() << "newsid: " << newsid;
        uint32_t arr_offender;
        int32_t offender = vm.popVar(arr_offender); if(verbose) LogInfo() << "offender: " << offender;
        int32_t source = vm.popDataValue(); if(verbose) LogInfo() << "source: " << source;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_hasnews", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_victim;
        int32_t victim = vm.popVar(arr_victim); if(verbose) LogInfo() << "victim: " << victim;
        uint32_t arr_offender;
        int32_t offender = vm.popVar(arr_offender); if(verbose) LogInfo() << "offender: " << offender;
        int32_t newsid = vm.popDataValue(); if(verbose) LogInfo() << "newsid: " << newsid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isnewsgossip", [=](Daedalus::DaedalusVM& vm){
        int32_t newsnumber = vm.popDataValue(); if(verbose) LogInfo() << "newsnumber: " << newsnumber;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getnewswitness", [=](Daedalus::DaedalusVM& vm){
        int32_t newsnumber = vm.popDataValue(); if(verbose) LogInfo() << "newsnumber: " << newsnumber;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("npc_getnewsvictim", [=](Daedalus::DaedalusVM& vm){
        int32_t newsnumber = vm.popDataValue(); if(verbose) LogInfo() << "newsnumber: " << newsnumber;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("npc_getnewsoffender", [=](Daedalus::DaedalusVM& vm){
        int32_t newsnumber = vm.popDataValue(); if(verbose) LogInfo() << "newsnumber: " << newsnumber;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("npc_isdead", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_knowsinfo", [=](Daedalus::DaedalusVM& vm){
        int32_t infoinstance = vm.popDataValue(); if(verbose) LogInfo() << "infoinstance: " << infoinstance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_checkinfo", [=](Daedalus::DaedalusVM& vm){
        int32_t important = vm.popDataValue(); if(verbose) LogInfo() << "important: " << important;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_giveinfo", [=](Daedalus::DaedalusVM& vm){
        int32_t important = vm.popDataValue(); if(verbose) LogInfo() << "important: " << important;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_checkavailablemission", [=](Daedalus::DaedalusVM& vm){
        int32_t important = vm.popDataValue(); if(verbose) LogInfo() << "important: " << important;
        int32_t missionstate = vm.popDataValue(); if(verbose) LogInfo() << "missionstate: " << missionstate;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_checkrunningmission", [=](Daedalus::DaedalusVM& vm){
        int32_t important = vm.popDataValue(); if(verbose) LogInfo() << "important: " << important;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_checkoffermission", [=](Daedalus::DaedalusVM& vm){
        int32_t important = vm.popDataValue(); if(verbose) LogInfo() << "important: " << important;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("mis_setstatus", [=](Daedalus::DaedalusVM& vm){
        int32_t newstatus = vm.popDataValue(); if(verbose) LogInfo() << "newstatus: " << newstatus;
        int32_t missionname = vm.popDataValue(); if(verbose) LogInfo() << "missionname: " << missionname;

    });

    vm.registerExternalFunction("mis_getstatus", [=](Daedalus::DaedalusVM& vm){
        int32_t missionname = vm.popDataValue(); if(verbose) LogInfo() << "missionname: " << missionname;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("mis_ontime", [=](Daedalus::DaedalusVM& vm){
        int32_t missionname = vm.popDataValue(); if(verbose) LogInfo() << "missionname: " << missionname;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_stopprocessinfos", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;

    });

    vm.registerExternalFunction("npc_isplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_player;
        int32_t player = vm.popVar(arr_player); if(verbose) LogInfo() << "player: " << player;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_detectplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasdetectednpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isnear", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getdisttonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc2;
        int32_t npc2 = vm.popVar(arr_npc2); if(verbose) LogInfo() << "npc2: " << npc2;
        uint32_t arr_npc1;
        int32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getdisttowp", [=](Daedalus::DaedalusVM& vm){
        std::string wpname = vm.popString(); if(verbose) LogInfo() << "wpname: " << wpname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getdisttoitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getdisttoplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc1;
        int32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("snd_getdisttosource", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_gettrueguild", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_setattitude", [=](Daedalus::DaedalusVM& vm){
        int32_t att = vm.popDataValue(); if(verbose) LogInfo() << "att: " << att;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_settempattitude", [=](Daedalus::DaedalusVM& vm){
        int32_t att = vm.popDataValue(); if(verbose) LogInfo() << "att: " << att;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_getattitude", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_settrueguild", [=](Daedalus::DaedalusVM& vm){
        int32_t guildid = vm.popDataValue(); if(verbose) LogInfo() << "guildid: " << guildid;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_setguildattitude", [=](Daedalus::DaedalusVM& vm){
        int32_t guild2 = vm.popDataValue(); if(verbose) LogInfo() << "guild2: " << guild2;
        int32_t attitude = vm.popDataValue(); if(verbose) LogInfo() << "attitude: " << attitude;
        int32_t guild1 = vm.popDataValue(); if(verbose) LogInfo() << "guild1: " << guild1;

    });

    vm.registerExternalFunction("wld_getguildattitude", [=](Daedalus::DaedalusVM& vm){
        int32_t guild2 = vm.popDataValue(); if(verbose) LogInfo() << "guild2: " << guild2;
        int32_t guild1 = vm.popDataValue(); if(verbose) LogInfo() << "guild1: " << guild1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getpermattitude", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_exchangeguildattitudes", [=](Daedalus::DaedalusVM& vm){
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;

    });

    vm.registerExternalFunction("npc_getguildattitude", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc_1;
        int32_t npc_1 = vm.popVar(arr_npc_1); if(verbose) LogInfo() << "npc_1: " << npc_1;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_setknowsplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_player;
        int32_t player = vm.popVar(arr_player); if(verbose) LogInfo() << "player: " << player;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_knowsplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_player;
        int32_t player = vm.popVar(arr_player); if(verbose) LogInfo() << "player: " << player;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_startstate", [=](Daedalus::DaedalusVM& vm){
        std::string wpname = vm.popString(); if(verbose) LogInfo() << "wpname: " << wpname;
        int32_t statebehaviour = vm.popDataValue(); if(verbose) LogInfo() << "statebehaviour: " << statebehaviour;

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_clearaiqueue", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_setnpcstostate", [=](Daedalus::DaedalusVM& vm){
        int32_t radius = vm.popDataValue(); if(verbose) LogInfo() << "radius: " << radius;

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_isinstate", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_wasinstate", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ta", [=](Daedalus::DaedalusVM& vm){
        std::string waypoint = vm.popString(); if(verbose) LogInfo() << "waypoint: " << waypoint;

        int32_t stop_h = vm.popDataValue(); if(verbose) LogInfo() << "stop_h: " << stop_h;
        int32_t start_h = vm.popDataValue(); if(verbose) LogInfo() << "start_h: " << start_h;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ta_min", [=](Daedalus::DaedalusVM& vm){
        std::string waypoint = vm.popString(); if(verbose) LogInfo() << "waypoint: " << waypoint;

        int32_t stop_m = vm.popDataValue(); if(verbose) LogInfo() << "stop_m: " << stop_m;
        int32_t stop_h = vm.popDataValue(); if(verbose) LogInfo() << "stop_h: " << stop_h;
        int32_t start_m = vm.popDataValue(); if(verbose) LogInfo() << "start_m: " << start_m;
        int32_t start_h = vm.popDataValue(); if(verbose) LogInfo() << "start_h: " << start_h;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_continueroutine", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_isinroutine", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_exchangeroutine", [=](Daedalus::DaedalusVM& vm){
        std::string routinename = vm.popString(); if(verbose) LogInfo() << "routinename: " << routinename;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("wld_setobjectroutine", [=](Daedalus::DaedalusVM& vm){
        int32_t state = vm.popDataValue(); if(verbose) LogInfo() << "state: " << state;
        std::string objname = vm.popString(); if(verbose) LogInfo() << "objname: " << objname;
        int32_t min1 = vm.popDataValue(); if(verbose) LogInfo() << "min1: " << min1;
        int32_t hour1 = vm.popDataValue(); if(verbose) LogInfo() << "hour1: " << hour1;

    });

    vm.registerExternalFunction("wld_setmobroutine", [=](Daedalus::DaedalusVM& vm){
        int32_t state = vm.popDataValue(); if(verbose) LogInfo() << "state: " << state;
        std::string objname = vm.popString(); if(verbose) LogInfo() << "objname: " << objname;
        int32_t min1 = vm.popDataValue(); if(verbose) LogInfo() << "min1: " << min1;
        int32_t hour1 = vm.popDataValue(); if(verbose) LogInfo() << "hour1: " << hour1;

    });

    vm.registerExternalFunction("rtn_exchange", [=](Daedalus::DaedalusVM& vm){
        std::string newroutine = vm.popString(); if(verbose) LogInfo() << "newroutine: " << newroutine;
        std::string oldroutine = vm.popString(); if(verbose) LogInfo() << "oldroutine: " << oldroutine;

    });

    vm.registerExternalFunction("ta_beginoverlay", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ta_endoverlay", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ta_removeoverlay", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("mdl_setmodelscale", [=](Daedalus::DaedalusVM& vm){
        float z = vm.popFloatValue(); if(verbose) LogInfo() << "z: " << z;
        float y = vm.popFloatValue(); if(verbose) LogInfo() << "y: " << y;
        float x = vm.popFloatValue(); if(verbose) LogInfo() << "x: " << x;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("mdl_setmodelfatness", [=](Daedalus::DaedalusVM& vm){
        float fatness = vm.popFloatValue(); if(verbose) LogInfo() << "fatness: " << fatness;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_changeattribute", [=](Daedalus::DaedalusVM& vm){
        int32_t value = vm.popDataValue(); if(verbose) LogInfo() << "value: " << value;
        int32_t atr = vm.popDataValue(); if(verbose) LogInfo() << "atr: " << atr;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_hastalent", [=](Daedalus::DaedalusVM& vm){
        int32_t tal = vm.popDataValue(); if(verbose) LogInfo() << "tal: " << tal;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_hasfighttalent", [=](Daedalus::DaedalusVM& vm){
        int32_t tal = vm.popDataValue(); if(verbose) LogInfo() << "tal: " << tal;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_createspell", [=](Daedalus::DaedalusVM& vm){
        int32_t spellnr = vm.popDataValue(); if(verbose) LogInfo() << "spellnr: " << spellnr;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_learnspell", [=](Daedalus::DaedalusVM& vm){
        int32_t spellnr = vm.popDataValue(); if(verbose) LogInfo() << "spellnr: " << spellnr;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_setteleportpos", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_getactivespell", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getactivespellcat", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_setactivespellinfo", [=](Daedalus::DaedalusVM& vm){
        int32_t i1 = vm.popDataValue(); if(verbose) LogInfo() << "i1: " << i1;
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getactivespelllevel", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ai_readyspell", [=](Daedalus::DaedalusVM& vm){
        int32_t investmana = vm.popDataValue(); if(verbose) LogInfo() << "investmana: " << investmana;
        int32_t spellid = vm.popDataValue(); if(verbose) LogInfo() << "spellid: " << spellid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_unreadyspell", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_hasspell", [=](Daedalus::DaedalusVM& vm){
        int32_t spellid = vm.popDataValue(); if(verbose) LogInfo() << "spellid: " << spellid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_percenable", [=](Daedalus::DaedalusVM& vm){

        int32_t percid = vm.popDataValue(); if(verbose) LogInfo() << "percid: " << percid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_percdisable", [=](Daedalus::DaedalusVM& vm){
        int32_t percid = vm.popDataValue(); if(verbose) LogInfo() << "percid: " << percid;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_setperctime", [=](Daedalus::DaedalusVM& vm){
        float seconds = vm.popFloatValue(); if(verbose) LogInfo() << "seconds: " << seconds;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("perc_setrange", [=](Daedalus::DaedalusVM& vm){
        int32_t range = vm.popDataValue(); if(verbose) LogInfo() << "range: " << range;
        int32_t percid = vm.popDataValue(); if(verbose) LogInfo() << "percid: " << percid;

    });

    vm.registerExternalFunction("npc_sendpassiveperc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc3;
        int32_t npc3 = vm.popVar(arr_npc3); if(verbose) LogInfo() << "npc3: " << npc3;
        uint32_t arr_npc2;
        int32_t npc2 = vm.popVar(arr_npc2); if(verbose) LogInfo() << "npc2: " << npc2;
        int32_t perc_type = vm.popDataValue(); if(verbose) LogInfo() << "perc_type: " << perc_type;
        uint32_t arr_npc1;
        int32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;

    });

    vm.registerExternalFunction("npc_sendsingleperc", [=](Daedalus::DaedalusVM& vm){
        int32_t percid = vm.popDataValue(); if(verbose) LogInfo() << "percid: " << percid;
        uint32_t arr_target;
        int32_t target = vm.popVar(arr_target); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("npc_perceiveall", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("wld_detectnpc", [=](Daedalus::DaedalusVM& vm){
        int32_t guild = vm.popDataValue(); if(verbose) LogInfo() << "guild: " << guild;

        int32_t instance = vm.popDataValue(); if(verbose) LogInfo() << "instance: " << instance;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_detectitem", [=](Daedalus::DaedalusVM& vm){
        int32_t flags = vm.popDataValue(); if(verbose) LogInfo() << "flags: " << flags;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_getdetectedmob", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn("");
    });

    vm.registerExternalFunction("npc_canseenpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc2;
        int32_t npc2 = vm.popVar(arr_npc2); if(verbose) LogInfo() << "npc2: " << npc2;
        uint32_t arr_npc1;
        int32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_canseenpcfreelos", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_other;
        int32_t other = vm.popVar(arr_other); if(verbose) LogInfo() << "other: " << other;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_canseeitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_npc1;
        int32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_canseesource", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("ta_cs", [=](Daedalus::DaedalusVM& vm){
        std::string rolename = vm.popString(); if(verbose) LogInfo() << "rolename: " << rolename;
        std::string csname = vm.popString(); if(verbose) LogInfo() << "csname: " << csname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("ai_playcutscene", [=](Daedalus::DaedalusVM& vm){
        std::string csname = vm.popString(); if(verbose) LogInfo() << "csname: " << csname;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

    });

    vm.registerExternalFunction("hlp_cutsceneplayed", [=](Daedalus::DaedalusVM& vm){
        std::string csname = vm.popString(); if(verbose) LogInfo() << "csname: " << csname;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isincutscene", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("snd_play", [=](Daedalus::DaedalusVM& vm){
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

    });

    vm.registerExternalFunction("snd_play3d", [=](Daedalus::DaedalusVM& vm){
        std::string s1 = vm.popString(); if(verbose) LogInfo() << "s1: " << s1;
        uint32_t arr_n0;
        int32_t n0 = vm.popVar(arr_n0); if(verbose) LogInfo() << "n0: " << n0;

    });

    vm.registerExternalFunction("snd_issourcenpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("snd_issourceitem", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_assignroomtoguild", [=](Daedalus::DaedalusVM& vm){
        int32_t guild = vm.popDataValue(); if(verbose) LogInfo() << "guild: " << guild;
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

    });

    vm.registerExternalFunction("wld_assignroomtonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_roomowner;
        int32_t roomowner = vm.popVar(arr_roomowner); if(verbose) LogInfo() << "roomowner: " << roomowner;
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

    });

    vm.registerExternalFunction("wld_getplayerportalowner", [=](Daedalus::DaedalusVM& vm){

        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("wld_getplayerportalguild", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("wld_getformerplayerportalowner", [=](Daedalus::DaedalusVM& vm){

        vm.setReturnVar(0);
    });

    vm.registerExternalFunction("wld_getformerplayerportalguild", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_isplayerinmyroom", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("npc_wasplayerinmyroom", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("inttostring", [=](Daedalus::DaedalusVM& vm){
        int32_t x = vm.popDataValue(); if(verbose) LogInfo() << "x: " << x;
        vm.setReturn("");
    });

    vm.registerExternalFunction("floattoint", [=](Daedalus::DaedalusVM& vm){
        float x = vm.popFloatValue(); if(verbose) LogInfo() << "x: " << x;
        vm.setReturn(0);
    });

    vm.registerExternalFunction("inttofloat", [=](Daedalus::DaedalusVM& vm){
        int32_t x = vm.popDataValue(); if(verbose) LogInfo() << "x: " << x;
        vm.setReturn(0.0f);
    });

    vm.registerExternalFunction("concatstrings", [=](Daedalus::DaedalusVM& vm){
        std::string str2 = vm.popString(); if(verbose) LogInfo() << "str2: " << str2;
        std::string str1 = vm.popString(); if(verbose) LogInfo() << "str1: " << str1;
        vm.setReturn("");
    });



    vm.registerExternalFunction("log_createtopic", [=](Daedalus::DaedalusVM& vm){
        int32_t section = vm.popDataValue(); if(verbose) LogInfo() << "section: " << section;
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;

    });

    vm.registerExternalFunction("log_settopicstatus", [=](Daedalus::DaedalusVM& vm){
        int32_t status = vm.popDataValue(); if(verbose) LogInfo() << "status: " << status;
        std::string name = vm.popString(); if(verbose) LogInfo() << "name: " << name;

    });

    vm.registerExternalFunction("log_addentry", [=](Daedalus::DaedalusVM& vm){
        std::string entry = vm.popString(); if(verbose) LogInfo() << "entry: " << entry;
        std::string topic = vm.popString(); if(verbose) LogInfo() << "topic: " << topic;

    });

    vm.registerExternalFunction("doc_create", [=](Daedalus::DaedalusVM& vm){

        vm.setReturn(0);
    });

    vm.registerExternalFunction("doc_setpages", [=](Daedalus::DaedalusVM& vm){
        int32_t pages = vm.popDataValue(); if(verbose) LogInfo() << "pages: " << pages;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_setpage", [=](Daedalus::DaedalusVM& vm){
        int32_t vbool = vm.popDataValue(); if(verbose) LogInfo() << "vbool: " << vbool;
        std::string pageimage = vm.popString(); if(verbose) LogInfo() << "pageimage: " << pageimage;
        int32_t page = vm.popDataValue(); if(verbose) LogInfo() << "page: " << page;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_setfont", [=](Daedalus::DaedalusVM& vm){
        std::string fontname = vm.popString(); if(verbose) LogInfo() << "fontname: " << fontname;
        int32_t page = vm.popDataValue(); if(verbose) LogInfo() << "page: " << page;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_setmargins", [=](Daedalus::DaedalusVM& vm){
        int32_t vbool = vm.popDataValue(); if(verbose) LogInfo() << "vbool: " << vbool;
        int32_t bottommargin = vm.popDataValue(); if(verbose) LogInfo() << "bottommargin: " << bottommargin;
        int32_t rightmargin = vm.popDataValue(); if(verbose) LogInfo() << "rightmargin: " << rightmargin;
        int32_t topmargin = vm.popDataValue(); if(verbose) LogInfo() << "topmargin: " << topmargin;
        int32_t leftmargin = vm.popDataValue(); if(verbose) LogInfo() << "leftmargin: " << leftmargin;
        int32_t page = vm.popDataValue(); if(verbose) LogInfo() << "page: " << page;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_printline", [=](Daedalus::DaedalusVM& vm){
        std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;
        int32_t page = vm.popDataValue(); if(verbose) LogInfo() << "page: " << page;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_printlines", [=](Daedalus::DaedalusVM& vm){
        std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;
        int32_t page = vm.popDataValue(); if(verbose) LogInfo() << "page: " << page;
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });

    vm.registerExternalFunction("doc_show", [=](Daedalus::DaedalusVM& vm){
        int32_t handle = vm.popDataValue(); if(verbose) LogInfo() << "handle: " << handle;

    });
}


