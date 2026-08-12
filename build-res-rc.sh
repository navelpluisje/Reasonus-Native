#!/bin/bash
echo "Start generate res.rc_mac_dlg"
cd src
php ../lib/WDL/WDL/swell/swell_resgen.php res.rc
cd ..
echo "Finished generating res.rc_mac_dlg"
