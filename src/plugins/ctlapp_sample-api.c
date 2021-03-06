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

CTLP_LUA_REGISTER("ctlapp_sample");

CTLP_ONLOAD(plugin, ret)
{
    AFB_ApiNotice (plugin->api, "ctlapp (control appplication) plugin loaded");
    return 0;
}

CTLP_CAPI(ping, source, argsJ, eventJ)
{
    AFB_ReqSuccess(source->request, json_object_new_string("ctlapp_sample pong"), NULL);
    return 0;
}

CTLP_LUA2C(display, source, argsJ, responseJ)
{
    int err, status;
    char *application, *version, *error, *info;
    char appid[128];

    err = wrap_json_unpack(argsJ, "{ss ss !}", "application", &application, "version", &version);
    if (err) {
       AFB_ApiError (source->api, "display invalid argsJ=%s", json_object_get_string(argsJ));
       goto OnErrorExit;
    }

    status = snprintf (appid, sizeof(appid), "%s@%s", application, version);
    assert (status < sizeof(appid));

    err = AFB_ApiSync(source->api, "afm_main", "once", json_object_new_string(appid), responseJ, &error, &info);
    if (err) {
       AFB_ApiError (source->api, "display fail afm_main start appid=%s", appid);
       goto OnErrorExit;
    }

    err = AFB_ApiSync(source->api, "homescreen", "tap_shortcut", json_object_new_string(application), responseJ, &error, &info);
    if (err) {
       AFB_ApiError (source->api, "display fail afm_main start appid=%s", application);
       goto OnErrorExit;
    }

    return 0;

OnErrorExit:
    return 1;
}
