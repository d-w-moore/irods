from __future__ import print_function
import os
import re
import sys
import tempfile

if sys.version_info < (2, 7):
    import unittest2 as unittest
else:
    import unittest

from . import session
from .. import test
from .. import lib
from ..configuration import IrodsConfig

class Test_Imv(session.make_sessions_mixin([('otherrods', 'rods')], []), unittest.TestCase):


    def setUp(self):
        super(Test_Imv, self).setUp()
        self.admin = self.admin_sessions[0]
        (fd, self.testfile) = tempfile.mkstemp()
        os.write(fd, 'text\n')
        os.close(fd)

    def tearDown(self):
        os.unlink(self.testfile)
        super(Test_Imv, self).tearDown()


    def test_imv_fails_with_error_for_missing_target_coll__issue_4414(self):

        testobj_basename = os.path.basename(self.testfile)
        pattern = re.compile (r'(\S+)') # - searches for non-whitespace in output

        # put a data object and a subcollection under collection 'test1'

        self.admin.run_icommand(['imkdir','-p','test1/subcoll'])
        self.admin.run_icommand(['iput',self.testfile,'test1'])

        # ensure collection test2 does not exist

        out,err,rc = self.admin.run_icommand(['ils','test2'])
        self.assertFalse (rc == 0)

        test_fns = { 'FALSE' : self.assertFalse,
                     'TRUE' : self.assertTrue }

        #cond = 'FALSE' # once 4414 is fixed , this will change to : cond = 'TRUE'
        cond = 'TRUE'
        testFunction = test_fns[ cond ]

        out,err,rc = self.admin.run_icommand(['imv', 'test1/subcoll','test2/subcoll'])

        testFunction( rc != 0, msg = "imv subcoll to nonexisting path - return of 0 - asserted " + cond )
        testFunction( pattern.findall(out + err), msg = "imv subcoll to nonexisting path - error msg - asserted " + cond )

        out,err,rc = self.admin.run_icommand( ['imv', 'test1/'+ testobj_basename,
                                                      'test2/'+ testobj_basename ] )

        testFunction( rc != 0, msg = "imv subobj to nonexisting path - return of 0 - asserted " + cond)
        testFunction( pattern.findall(out + err), msg = "imv subobj to nonexisting path - error msg - asserted " + cond )


    @unittest.skipIf(test.settings.RUN_IN_TOPOLOGY, "Skip for Topology Testing")
    def test_imv_boost_any_cast_exception_on_rename__issue_4301(self):
        config = IrodsConfig()

        with lib.file_backed_up(config.server_config_path):
            core_re_path = os.path.join(config.core_re_directory, 'core.re')

            with lib.file_backed_up(core_re_path):
                with open(core_re_path, 'a') as core_re:
                    core_re.write('pep_api_data_obj_rename_post(*a, *b, *c) {}\n')

                src = 'test_file_issue_4301_a.txt'
                lib.make_file(src, 1024, 'arbitrary')
                self.admin.assert_icommand(['iput', src])

                dst = 'test_file_issue_4301_b.txt'
                self.admin.assert_icommand(['imv', src, dst])
                self.admin.assert_icommand('ils', 'STDOUT', dst)
