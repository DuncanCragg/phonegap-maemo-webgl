#!/bin/bash
scons
rm phonegap.js
mv phonegap-compressed.js www-example/phonegap.js
