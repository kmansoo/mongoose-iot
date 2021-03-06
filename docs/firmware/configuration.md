---
title: Configuration
---

Mongoose Firmware uses 3 layers of configuration, backed by 3 plain text
JSON files:

- `conf_sys_defaults.json` - "factory" system configuration. This file stays
  constant after its creation by the firmware build process.
- `conf_app_defaults.json` - vendor-specific configuration. A developer that
  uses Mongoose Firmware may use it to override any system parameter or to
  add some more parameters. This file is expected to be created once by the
  device bootstrapping phase, before shipping to the end-customer.
- `conf.json` - end-customer configuration. Overrides previous two settings.
  For example, the WiFi network name and password can be set by the end customer.

When Mongoose Firmware boots, these three files are loaded in that specific
sequence. As a result, a configuration oject is created. In C, a configuration
object is a structure: `get_cfg()` call returns a pointer to it.
In JavaScript, it is a `Sys.conf` object.

In JavaScript, to make changes to configuration, update the respective value
(for example, `Sys.conf.wifi.ap.enable = false;`) and call the `Sys.conf.save()`
function. That call will update `conf.json` file and reboot the firmware.

In C, the configuration object is accessible via the `get_cfg()` function. See an
[example on how to make and use custom configuration parameters in C](https://github.com/cesanta/mongoose-iot/blob/master/fw/examples/c_hello/src/app_main.c#L19).

Apart from programmatic access, the configuration can be accessed via the
Web UI. If your device has joined your local WiFi network, point your browser
to http://YOUR-DEVICE-IP. If your device has started its own WiFi network,
join it and point your browser to http://192.168.4.1. Then, make any
required change and press the Save button.

To view a list of all system configuration parameters and their meaning, please see this
[conf_sys_schema.json](https://github.com/cesanta/dev/blob/master/fw/src/fs/conf_sys_schema.json) file. This file is used by the Web UI to render configuration parameters
and their descriptions.
