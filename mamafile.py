import mama
class CppGuid(mama.BuildTarget):
    workspace = 'build'

    def dependencies(self):
        pass

    def configure(self):
        if self.is_test_target():
            print("Enabled CppGuidTests for testing")
            self.add_cmake_options('CPPGUID_BUILD_TESTS=ON')

    def package(self):
        self.export_include('include')
        self.export_libs()
        if self.linux:
            self.export_syslib('uuid')
        if self.macos or self.ios:
            self.export_syslib('-framework Foundation')

    def test(self, args):
        self.gdb(f'lib/CppGuidTests {args}', src_dir=True)
