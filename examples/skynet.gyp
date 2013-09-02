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
							'ARCHS': ['x86_64' ],
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
							'ARCHS': [ 'x86_64' ],
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
			'sources' : ['ffnet_example.cpp'],
			'cflags_cc': ['-std=c++11'],

			'conditions':[
				['OS=="mac"',{
					'include_dirs':['../', '/usr/local/include'],
					'library_dirs':['/usr/local/lib'],
					'link_settings':{
						'libraries':['libboost_system.a', 'libboost_filesystem.a'],
					},
				}],
				['OS=="win"',{
					'include_dirs':['../', '$(BOOST_INCLUDE_DIR)'],
					'library_dirs':['$(BOOST_LIBRARY_DIR)'],
				}],
			],
		},
		{
			'target_name' : 'auto_encoder',
			'type' : 'executable',
		
			'sources' : ['auto_encoder_example.cpp'],
			'cflags_cc': ['-std=c++11'],

			'conditions':[
				['OS=="mac"',{
					'include_dirs':['../', '/usr/local/include'],
					'library_dirs':['/usr/local/lib'],
					'link_settings':{
						'libraries':['libboost_system.a', 'libboost_filesystem.a'],
					},
				}],
				['OS=="win"',{
					'include_dirs':['../', '$(BOOST_INCLUDE_DIR)'],
					'library_dirs':['$(BOOST_LIBRARY_DIR)'],
				}],
			],
		},
	]
}
