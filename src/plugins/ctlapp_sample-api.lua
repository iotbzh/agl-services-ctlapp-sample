--[[
  Copyright (C) 2018 "IoT.bzh"
  Author Fulup Ar Foll <fulup@iot.bzh>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.


  NOTE: strict mode: every global variables should be prefixed by '_'

  Test commands:
    afb-client-demo 'localhost:1111/api?token=XXX&uuid=magic' ctlapp_sample display
--]]


function _run_onload_(source)
    AFB:notice(source, "--InLua-- ENTER _run_onload_ CTLapp service sample\n")

    local err, response =AFB:servsync (source, "homescreen", subscribe, {}) 
    if (err) then
       AFB:error(source, "--inlua-- ERROR: Homescreen fail to display application %s@", query.application)
       return 1
    end

    return 0
end

function _evt_catcher_(source, action, event)
    AFB:notice(source, "RECV EVENT=%s", Dump_Table(event))
end

function _display_application_(source, args, query)
    AFB:debug(source, "--InLua-- ENTER _display_application_ query=%s", Dump_Table(query))

    if query == "null" then
        AFB:fail(source, "ERROR: invalid parameters")
        return 1
    end

    if query.application == nil then
        AFB:fail(source, "ERROR: application parameter not set")
        return 1
    end

    if query.version == nil then
        query.version = "0.1"
        return 1
    end

    local err, response =AFB:servsync (source, "afmmain", {[name] = "start", query.application .. "@" .. query.version}) 
     if (err) then
        AFB:fail(source, "ERROR: AfmMain fail to start application %s@%s", query.application, query.version)
        return 1
    end

    local err, response =AFB:servsync (source, "homescreen", tap_shortcut, query.application) 
    if (err) then
       AFB:fail(source, "ERROR: Homescreen fail to display application %s@", query.application)
       return 1
   end

    AFB:success(source, response)
end
