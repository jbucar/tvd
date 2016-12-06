#!/usr/bin/env python2

# Automate the generation of depot_build factoriies config

import json

from string import Template
s = Template("""   "${prod}_${plat}": {
        "name": "${uprod}_${uplat}",
        "tags": ["depot_build", "${prod}", "${plat}" ],
        "factory": {
            "product": "${prod}",
            "is_tool": true,
            "type": "depot_build",
            "slave": "ArchI7",
            "platform": "${plat}",
            "toolchain": "${toolchain}",
            "config": "tool/tac/build/devices/${dir}/deps/${uprod}.cmake"
        },
        "schedulers": [{
            "type": "force_schedule",
            "properties": [
                { "class": "ChoiceStringParameter", "parameters": {"name": "build_type", "label": "Build type", "choices": ["release","debug"], "default": "debug" } },
                { "class": "ChoiceStringParameter", "parameters": {"name": "build_method", "label": "Build mode", "choices": ["shared","static"], "default": "static" } },
                { "class": "EmptyStringParameter",  "parameters": {"name": "dst_path" , "label": "Destination path", "size": 80, "default": "builds/${toolchain_dir}/${prod}/%(prop:build_type)s/%(prop:build_method)s" } }
            ],
            "name": "Force build for ${uprod}_${uplat}"
        }]
    },""")

plat2toolchain = {
	"x64": "CLANG_STATIC64",
	"aml": "AML_STATIC"
}

plat2toolchain_dir = {
	"x64": "CLANG_64",
	"aml": "AML"
}

for prod in [ "ginga", "htmlshellremote", "ioctlmodule", "libcecnode", "tvdmodule" ]:
	for plat in [ "aml", "x64" ]:
		directory = "common"
		if prod == "tvdmodule" and plat == "aml":
			directory = "aml"
		d = {
			"prod": prod,
			"plat": plat,
			"dir": directory,
			"uprod": prod.upper(),
			"uplat": plat.upper(),
			"toolchain": plat2toolchain[plat],
			"toolchain_dir": plat2toolchain_dir[plat]
		}
		print( s.substitute(d))
