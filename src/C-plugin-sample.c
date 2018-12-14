/*
 * Copyright (C) 2018 "IoT.bzh"
 * Author "Sebastien Douheret" <sebastien@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "ctl-plugin.h"
#include "wrap-json.h"

/*
 * Macro to register the plugin to the controller as ctlapp-sample and could be
 * reach from a LUA plugin its LUA2C functions else use CTLP_CAPI_REGISTER to
 * register to the controller */
CTLP_LUA_REGISTER("ctl-plugin-sample");
/* The same than above but without LUA entry points
CTLP_CAPI_REGISTER("ctl-plugin-sample"); */

/*
 * Macro declaring a function to be executed right after the plugin loads with
 * the following signature:
 * int CtlPluginOnload(CtlPluginT *plugin, void* handle)
 */
CTLP_ONLOAD(plugin, ret)
{
    AFB_API_NOTICE (plugin->api, "ctlapp (control appplication) plugin loaded");
    return 0;
}

/*
 * Macro registering a C function to be called through the binder named ping.
 * The signature of controller's plugin C function is:
 * int funcname(CtlSourceT *source, json_object* argsJ, json_object* queryJ)
 * with 'funcname' as the first CTLP_CAPI argument, here 'ping'.
 */
CTLP_CAPI(ping, source, argsJ, queryJ)
{
    afb_req_success(source->request, json_object_new_string("ctl-plugin-sample pong"), NULL);
    return 0;
}

/*
 * Macro registering a C function that could be called from LUA plugins or as a
 * controller action. The signature of LUA2C function is:
 * int funcname (CtlSourceT* source, json_object* argsJ, json_object** responseJ)
 * with 'funcname' as the first CTLP_CAPI argument, here 'display'.
 */
CTLP_LUA2C(display, source, argsJ, responseJ)
{
    int err, status;
    char *application, *version, *error, *info;
    char appid[128];

    err = wrap_json_unpack(argsJ, "{ss ss !}", "application", &application, "version", &version);
    if (err) {
       AFB_API_ERROR (source->api, "display invalid argsJ=%s", json_object_get_string(argsJ));
       goto OnErrorExit;
    }

    status = snprintf (appid, sizeof(appid), "%s@%s", application, version);
    assert (status < sizeof(appid));

    err = afb_api_call_sync(source->api, "afm_main", "once", json_object_new_string(appid), responseJ, &error, &info);
    if (err) {
       AFB_API_ERROR (source->api, "display fail afm_main start appid=%s", appid);
       goto OnErrorExit;
    }

    err = afb_api_call_sync(source->api, "homescreen", "tap_shortcut", json_object_new_string(application), responseJ, &error, &info);
    if (err) {
       AFB_API_ERROR (source->api, "display fail afm_main start appid=%s", application);
       goto OnErrorExit;
    }

    return 0;

OnErrorExit:
    return 1;
}
