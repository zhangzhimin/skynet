# Use of this source code is governed by the MIT License.
{
	"target_defaults":{
		"configurations":{
			"Debug":{
				"msvs_configuration_platform": "x64",
				"cflags" : ['-std=c++11'],
				"cflags_cc":['-std=c++11'],
				"cxxflags":['-std=c++11'],
			},
		},	
	},

	"make_global_settings":[
		["CXX", "/usr/bin/g++"],
		["LINK", "/usr/bin/g++"],
	],
	"targets":[
		{
			"target_name" : "skynet",
			"type" : "executable",
			"include_dirs":['../',
							'/usr/local/include',
							],
			"sources" : ["ffnet_example.cpp"],
			'cflags_cc': ['-std=c++11'],
			"msvs_settings":{
				"VCCLCompilerTool":{
#"Optimization": "0",
					"WarningLevel": "4",
				#	"WarnAsError": "true",
					"RuntimeLibrary": "1",
				}
			},
			"conditions":[
				['OS=="mac"',{
					'xcode_settings':{
						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
						'CLANG_CXX_LIBRARY':'libc++',
#'OTHER_FLAGS': ['-std=c++11', '-stdlib=libc++'],
#						'OTHER_LDFLAS': ['-stdlib=libc++'],
					},
				}],
			],
		},
	]
}
