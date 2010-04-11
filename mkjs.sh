#!/bin/bash
scons
rm phonegap.js
mv app.js www
