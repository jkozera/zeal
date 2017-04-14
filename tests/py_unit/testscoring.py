#!/usr/bin/env python
# -*- coding: utf-8 -*-

from _scoring import lib
import os
import timeit
import unittest


def score(a, b):
    res = lib.scoreFunction(a, b)
    if os.environ.get('ZEAL_TESTS_DEBUG') == '1':
        print a, b, res
    return res


class ScoringTest(unittest.TestCase):

    def testExactBeforeFuzzy(self):
        # see https://github.com/zealdocs/zeal/issues/677
        self.assertGreater(
            score("argument", "argparse.argument"),
            score("argument", "PyErr_BadArgument")
        )

        self.assertGreater(
            score("argument", "argparse.ArgumentParser"),
            score("argument", "inspect.BoundArguments")
        )
        
        self.assertGreater(
            score("addarg", "argparse.ArgumentParser.add_argument"),
            0
        )

        self.assertGreater(
            score("testrle", "AWS.ElasticTranscoder.testRole"),
            0
        )


if __name__ == '__main__':
    print timeit.timeit("lib.perfTest()", "from _scoring import lib", number=2)

    unittest.main()
