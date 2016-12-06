import os

class Builder():
#public:
	def __init__(self, name, platform, deployDir, util):
		self.module = name;
		self.platform = platform;
		self.deployDir = deployDir;
		self.util = util;
		self.moduleDir = util.tacAbsPath( 'packages', 'node_modules', self.module )

	def install(self):
		self.util.lintPackage( self.module, self.moduleDir )

		dstDir = os.path.join( self.deployDir, self.module )
		self.util.cp( 'src', self.moduleDir, dstDir )
		self.util.cp( 'package.json', self.moduleDir, dstDir )

	def test(self):
		self.util.testAndCheckCoverage( self.module, self.moduleDir )

	#virtual
	def clean(self):
		pass

	#virtual
	def build(self):
		pass
