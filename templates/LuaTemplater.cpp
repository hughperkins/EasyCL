// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>
#include <stdexcept>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "LuaTemplater.h"

using namespace std;

#undef STATIC
#define STATIC
#undef VIRTUAL
#define VIRTUAL
#define PUBLIC

namespace easycl {
PUBLIC LuaTemplater::LuaTemplater() {
    L = luaL_newstate();
    luaL_openlibs(L);

    if(luaL_dostring(L, getTemplaterSource().c_str())) {
        string error = lua_tostring(L, -1);
        throw runtime_error("Could not load templater source: " + error);
    }
}
PUBLIC LuaTemplater::~LuaTemplater() {
    lua_close(L);
}
PUBLIC void LuaTemplater::set(std::string name, std::string value) {
    lua_pushstring(L, value.c_str());
    lua_setglobal(L, name.c_str());
}
PUBLIC void LuaTemplater::set(std::string name, float value) {
    lua_pushnumber(L, value);
    lua_setglobal(L, name.c_str());
}
PUBLIC void LuaTemplater::set(std::string name, int value) {
    lua_pushnumber(L, value);
    lua_setglobal(L, name.c_str());
}
PUBLIC void LuaTemplater::set(std::string name, std::vector< std::string> &values) {
    lua_newtable(L);
    for(int i = 0; i < (int)values.size(); i++) {
        lua_pushstring(L, values[i].c_str());
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, name.c_str());
}
PUBLIC void LuaTemplater::set(std::string name, std::vector< int> &values) {
    lua_newtable(L);
    for(int i = 0; i < (int)values.size(); i++) {
        lua_pushnumber(L, values[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, name.c_str());
}
PUBLIC void LuaTemplater::set(std::string name, std::vector< float> &values) {
    lua_newtable(L);
    for(int i = 0; i < (int)values.size(); i++) {
        lua_pushnumber(L, values[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, name.c_str());
}
PUBLIC std::string LuaTemplater::render(std::string template_string) {
    lua_getfield(L, -1, "compile");
    lua_pushstring(L, template_string.c_str());
    if (lua_pcall(L, 1, 1, 0) != 0) {
        string error = lua_tostring(L, -1);
        cout << "error: " << error << endl;
        throw runtime_error("Error: " + error);
    }
    std::string result = lua_tostring(L, -1);
    lua_pop(L, 1);

    return result;
}
std::string LuaTemplater::getTemplaterSource() {
    // [[[cog
    // import stringify
    // stringify.stringify("templaterSource", "templates/templater.lua")
    // ]]]
    // generated using cog, from templates/templater.lua:
    const char * templaterSource =  
    "-- adapted from https://john.nachtimwald.com/2014/08/06/using-lua-as-a-templating-engine/\n" 
    "--\n" 
    "-- changes from original:\n" 
    "--    code sections {% end in %} now\n" 
    "--    fixed some bugs, eg:\n" 
    "--      - 'template {{avalue}}' didnt work before\n" 
    "--      - last character was truncated\n" 
    "--\n" 
    "--- Template renderer.\n" 
    "--\n" 
    "-- Takes a string with embedded Lua code and renders\n" 
    "-- it based on the Lua code.\n" 
    "--\n" 
    "-- Supports:\n" 
    "--  * {{var}} for printing variables.\n" 
    "--  * {% func %} for running Lua functions.\n" 
    "--\n" 
    "--  Multi-line strings in Lua blocks are supported but\n" 
    "--  [[ is not allowed. Use [=[ or some other variation.\n" 
    "--\n" 
    "--  Both compile and compile_file can take an optional\n" 
    "--  env table which when provided will be used as the\n" 
    "--  env for the Lua code in the template. This allows\n" 
    "--  a level of sandboxing. Note that any globals including\n" 
    "--  libraries that the template needs to access must be\n" 
    "--  provided by env if used.\n" 
    "\n" 
    "local M = {}\n" 
    "\n" 
    "-- Append text or code to the builder.\n" 
    "local function appender(builder, text, code)\n" 
    "    if code then\n" 
    "        builder[#builder+1] = code\n" 
    "    else\n" 
    "        -- [[ has a \\n immediately after it. Lua will strip\n" 
    "        -- the first \\n so we add one knowing it will be\n" 
    "        -- removed to ensure that if text starts with a \\n\n" 
    "        -- it won't be lost.\n" 
    "        builder[#builder+1] = \"_ret[#_ret+1] = [[\\n\" .. text .. \"]]\"\n" 
    "    end\n" 
    "end\n" 
    "\n" 
    "--- Takes a string and determines what kind of block it\n" 
    "-- is and takes the appropriate action.\n" 
    "--\n" 
    "-- The text should be something like:\n" 
    "-- \"{{ ... }}\"\n" 
    "--\n" 
    "-- If the block is supported the begin and end tags will\n" 
    "-- be stripped and the associated action will be taken.\n" 
    "-- If the tag isn't supported the block will be output\n" 
    "-- as is.\n" 
    "local function run_block(builder, text)\n" 
    "    local func\n" 
    "    local tag\n" 
    "\n" 
    "    -- print('run_block [' .. text .. ']' )\n" 
    "    tag = text:sub(1, 2)\n" 
    "\n" 
    "    if tag == \"{{\" then\n" 
    "        func = function(code)\n" 
    "            return ('_ret[#_ret+1] = %s'):format(code)\n" 
    "        end\n" 
    "    elseif tag == \"{%\" then\n" 
    "        func = function(code)\n" 
    "            return code\n" 
    "        end\n" 
    "    end\n" 
    "    if func then\n" 
    "        appender(builder, nil, func(text:sub(3, #text-2)))\n" 
    "    else\n" 
    "        appender(builder, text)\n" 
    "    end\n" 
    "end\n" 
    "\n" 
    "--- Compile a Lua template into a string.\n" 
    "--\n" 
    "-- @param      tmpl The template.\n" 
    "-- @param[opt] env  Environment table to use for sandboxing.\n" 
    "--\n" 
    "-- return Compiled template.\n" 
    "function M._compile(tmpl)\n" 
    "    --print('templ [' .. tmpl .. ']')\n" 
    "    -- Turn the template into a string that can be run though\n" 
    "    -- Lua. Builder will be used to efficiently build the string\n" 
    "    -- we'll run. The string will use it's own builder (_ret). Each\n" 
    "    -- part that comprises _ret will be the various pieces of the\n" 
    "    -- template. Strings, variables that should be printed and\n" 
    "    -- functions that should be run.\n" 
    "    local builder = { \"_ret = {}\\n\" }\n" 
    "    local pos     = 1\n" 
    "    local b\n" 
    "    local func\n" 
    "    local err\n" 
    "\n" 
    "    if #tmpl == 0 then\n" 
    "        return \"\"\n" 
    "    end\n" 
    "\n" 
    "    while pos < #tmpl do\n" 
    "        -- Look for start of a Lua block.\n" 
    "        doublebracepos = tmpl:find(\"{{\", pos)\n" 
    "        bracepercentpos = tmpl:find(\"{%%\", pos)\n" 
    "        if doublebracepos and not bracepercentpos or ( doublebracepos and bracepercentpos and doublebracepos < bracepercentpos ) then\n" 
    "            b = doublebracepos\n" 
    "            -- Add all text up until this block.\n" 
    "            appender(builder, tmpl:sub(pos, b-1))\n" 
    "            -- Find the end of the block.\n" 
    "            pos = tmpl:find(\"}}\", b)\n" 
    "            if not pos then\n" 
    "                appender(builder, \"End tag ('}}') missing\")\n" 
    "                break\n" 
    "            end\n" 
    "            run_block(builder, tmpl:sub(b, pos+1))\n" 
    "            -- Skip back the }} (pos points to the start of }}).\n" 
    "            pos = pos+2\n" 
    "        elseif bracepercentpos then\n" 
    "            b = bracepercentpos\n" 
    "            appender(builder, tmpl:sub(pos, b-1))\n" 
    "            -- Find the end of the block.\n" 
    "            pos = tmpl:find(\"%%}\", b)\n" 
    "            if not pos then\n" 
    "                appender(builder, \"End tag ('%}') missing\")\n" 
    "                break\n" 
    "            end\n" 
    "            run_block(builder, tmpl:sub(b, pos+1))\n" 
    "            -- Skip back the }} (pos points to the start of }}).\n" 
    "            pos = pos+2\n" 
    "        else\n" 
    "            break\n" 
    "        end\n" 
    "    end\n" 
    "    -- Add any text after the last block. Or all of it if there\n" 
    "    -- are no blocks.\n" 
    "    if pos then\n" 
    "        appender(builder, tmpl:sub(pos, #tmpl))\n" 
    "    end\n" 
    "\n" 
    "    builder[#builder+1] = \"return table.concat(_ret)\"\n" 
    "    local tc = table.concat(builder, \"\\n\")\n" 
    "    -- print('tc', tc)\n" 
    "    local func, err = loadstring(tc)\n" 
    "    if not func then\n" 
    "        print('Error')\n" 
    "        print('Incoming template:\\n', tmpl)\n" 
    "        print('intemediate form (this is just pure Lua code, you can run this in lua):\\n', tc)\n" 
    "        return err\n" 
    "    end\n" 
    "    status, res = pcall(\n" 
    "      function()\n" 
    "        return func()\n" 
    "      end\n" 
    "    )\n" 
    "    if not status then\n" 
    "      print('Error')\n" 
    "      print('Incoming template:\\n', tmpl)\n" 
    "      print('intemediate form (this is just pure Lua code, you can run this in lua, see what happens):')\n" 
    "      i = 1\n" 
    "      for line in string.gmatch(tc, '[^\\n]+') do\n" 
    "        print(i .. ':', line)\n" 
    "        i = i + 1\n" 
    "      end\n" 
    "      print('err', res)\n" 
    "      error( res )\n" 
    "    else\n" 
    "      return res\n" 
    "    end\n" 
    "end\n" 
    "\n" 
    "function M.compile(tmpl)\n" 
    "  -- status = true\n" 
    "  last_compiled = ''\n" 
    "  compiled = tmpl\n" 
    "  while compiled ~= last_compiled do\n" 
    "    last_compiled = compiled\n" 
    "    compiled = M._compile(compiled)\n" 
    "  end\n" 
    "  return compiled\n" 
    "end\n" 
    "\n" 
    "return M\n" 
    "\n" 
    "";
    // [[[end]]]
    return templaterSource;
}
}
