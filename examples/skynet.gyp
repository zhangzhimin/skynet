# Use of this source code is governed by the MIT License.
{
	'target_defaults':{
		'configurations':{
			'Debug':{
				'conditions':[
					['OS=="win"',{
						'msvs_configuration_platform': 'x64',
						'msvs_settings':{
							'VCCLCompilerTool':{					
								'Optimization': '0',
								'WarningLevel': '4',
								#	'WarnAsError': 'true',
								'RuntimeLibrary': '1', # 1 :MTD
							},
							'VCLinkerTool':{
								'GenerateDebugInformation': 'true',
							}
						},
					}],

					['OS=="mac"',{
						'xcode_settings':{
							'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
							'CLANG_CXX_LIBRARY':'libc++',
						},
					}],
				]
			},
			'Release':{
				'conditions':[
					['OS=="win"',{
						'msvs_configuration_platform': 'x64',
						'msvs_settings':{
							'VCCLCompilerTool':{					
								'Optimization': '2',
								'WarningLevel': '4',
								#	'WarnAsError': 'true',
								'RuntimeLibrary': '0', # 1 :MTD
							},
							'VCLinkerTool':{
								'GenerateDebugInformation': 'false',
							}
						},
					}],
					
					['OS=="mac"',{
						'xcode_settings':{
							'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
							'CLANG_CXX_LIBRARY':'libc++',
						},
					}],
				]
			},
		},	
	},

	'make_global_settings':[
		['CXX', '/usr/bin/g++'],
		['LINK', '/usr/bin/g++'],
	],
	'targets':[
		{
			'target_name' : 'ffnet',
			'type' : 'executable',
			'include_dirs':['../',
							'/usr/local/include',
							],
			'sources' : ['ffnet_example.cpp'],
			'cflags_cc': ['-std=c++11'],

#			'conditions':[
#				['OS=="mac"',{
#					'xcode_settings':{
#						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
#						'CLANG_CXX_LIBRARY':'libc++',
#					},
#				}],
#],
		},
		{
			'target_name' : 'auto_encoder',
			'type' : 'executable',
			'include_dirs':['../',
							'/usr/local/include',
							],
			'sources' : ['auto_encoder_example.cpp'],
			'cflags_cc': ['-std=c++11'],

#			'conditions':[
#				['OS=="mac"',{
#					'xcode_settings':{
#						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
#						'CLANG_CXX_LIBRARY':'libc++',
#					},
#				}],
#			],
		},
	]
}
