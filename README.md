# agl_services_ctlapp_sample

This code was developped during Yokohama CES-2019 integration workshop.

It contains only controller plugins code and use a generic controller binding
already installed and setup on a target called "agl-service-controller". This is
an example that interface with the homescreen to control it and offering a basic
API as example.

Please see the documentation [here](http://docs.automotivelinux.org/master/docs/devguides/en/dev/reference/ctrler/controllerConfig.html)
to configure it and adapt it to your needs.

## Native build

```bash
mkdir build && cd build
cmake ..
make
```

## Manual tests

```bash
# Native binder
afb-daemon --port=1111 --name=afb-agl-service-controller-sample --workdir=$HOME/xxxxx/agl-services-ctlapp-sample/build/package --ldpaths=lib --roothttp=htdocs --token= --verbose
```

```bash
curl -s http://localhost:1111/api/ctlapp/start?token=HELLO&name=navigation
```

You can also use the basic html app:

```bash
chromium http://localhost:1111
```
