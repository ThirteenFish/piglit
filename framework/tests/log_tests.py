# Copyright (c) 2014 Intel Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

""" Module provides tests for log.py module """

import sys
import itertools
from types import *  # This is a special * safe module
import nose.tools as nt
from framework.log import Log

valid_statuses = ('pass', 'fail', 'crash', 'warn', 'dmesg-warn',
                  'dmesg-fail', 'skip', 'dry-run')

def test_initialize_log_terse():
    """ Test that Log initializes with verbose=False """
    log = Log(100, False)
    assert log


def test_initialize_log_verbose():
    """ Test that Log initializes with verbose=True """
    log = Log(100, True)
    assert log


def test_pre_log_return():
    """ Test that pre_log returns a number """
    log = Log(100, False)

    ret = log.pre_log()
    nt.assert_true(isinstance(ret, (IntType, FloatType, LongType)),
                   msg="Log.pre_log() didn't return a numeric type!")


def test_post_log_increment_complete():
    """ Tests that Log.post_log() increments self.__complete """
    log = Log(100, False)
    ret = log.pre_log()
    log.post_log(ret, 'pass')
    nt.assert_equal(log._Log__complete, 1,
                    msg="Log.post_log() did not properly incremented "
                        "Log.__current")


def check_post_log_increment_summary(stat):
    """ Test that passing a result to post_log works correctly """
    log = Log(100, False)
    ret = log.pre_log()
    log.post_log(ret, stat)
    print log._Log__summary
    nt.assert_equal(log._Log__summary[stat], 1,
                    msg="Log.__summary[{}] was not properly "
                        "incremented".format(stat))


def test_post_log_increment_summary():
    """ Generator that creates tests for self.__summary """
    yieldable = check_post_log_increment_summary

    for stat in valid_statuses:
        yieldable.description = ("Test that Log.post_log increments "
                                 "self._summary[{}]".format(stat))
        yield yieldable, stat


def test_post_log_removes_complete():
    """ Test that Log.post_log() removes finished tests from __running """
    log = Log(100, False)
    ret = log.pre_log()
    log.post_log(ret, 'pass')
    nt.assert_not_in(ret, log._Log__running,
                     msg="Running tests not removed from running list")


@nt.raises(AssertionError)
def test_post_log_increment_summary_bad():
    """ Only statuses in self.__summary_keys are valid for post_log """
    log = Log(100, False)
    ret = log.pre_log()
    log.post_log(ret, 'fails')
