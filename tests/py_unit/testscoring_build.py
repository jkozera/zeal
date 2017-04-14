#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cffi import FFI

scoring_lines = []
def_lines = []
with open(__file__.replace('testscoring_build.py', '../../src/libs/registry/docset.cpp')) as f:
    in_scoring = False
    in_def = False
    for line in f:
        line = line.rstrip()
        if line.startswith('static int scoreFuzzy'):
            in_scoring = True
            scoring_lines.append('extern "C" {')
        if line.startswith('static void sqlite'):
            in_scoring = False
        if in_def:
            def_lines.append(line)
            if line.endswith(')'):
                def_lines[-1] += ';'
                in_def = False
        if in_scoring or line.startswith('#include <Q') or line.startswith("#include <c"):
            if line.startswith('static '):
                line = line[len('static '):]
                if line.endswith(')'):
                    def_lines.append(line + ';')
                else:
                    def_lines.append(line)
                    in_def = True
            scoring_lines.append(line)

scoring_lines.extend('''
    void perfTest() {
        FILE* f = fopen("allsymbols", "r");
        char buf[1000];
        while(fscanf(f, "%s\\n", buf) != EOF) {
            scoreFunction("aaaaaaaaaa", buf);
        }
    }
'''.split('\n'))
def_lines.append('void perfTest();')
scoring_lines.append('} // extern "C"')

print '\n'.join(scoring_lines)

ffibuilder = FFI()
ffibuilder.set_source(
    '_scoring',
    '\n'.join(scoring_lines),
    source_extension='.cpp',
    include_dirs=[
        '/usr/include/x86_64-linux-gnu/qt5',
        '/usr/include/x86_64-linux-gnu/qt5/QtCore'
    ],
    extra_link_args=['-lQt5Core', '-lsqlite3'],
)

ffibuilder.cdef('\n'.join(def_lines))

if __name__ == '__main__':
    ffibuilder.compile(verbose=True)
