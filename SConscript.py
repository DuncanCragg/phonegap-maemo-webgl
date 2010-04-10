import os
from os.path import join, exists

PHONEGAP_LIB        = "phonegap.js"
PHONEGAP_COMPRESSED = "app.js"
APPNAME = "PhoneGap"

def CommandMakePhonegapJS(env, source, target):
    
    print target[0].path
    ft = open( target[0].path, 'wb' )
    
    for s in source:
        fs = open( s.path, 'rb')
        d = d = fs.read();
        while len(d) != 0:
            ft.write( d )
            d = fs.read();            
        
        fs.close()        
        
        ft.write( os.linesep )
        
    ft.close()
 
jssources = [
    "phonegap.js",
    "device.js",
    "accelerometer.js",
    "notification.js"
]

javascripts = []
JSROOTPATH= "javascript"

for js in jssources:    
    maemolib = join( JSROOTPATH, js )
    if exists( maemolib ): 
        javascripts.append( maemolib )
    
Command( PHONEGAP_LIB, javascripts, CommandMakePhonegapJS )

# Compress javascripts
#compress = "java -jar ../util/yuicompressor-2.4.2.jar --charset UTF-8 -o $TARGET $SOURCE"

from pylib import jsmin

def CommandCompressJS(env, source, target):
    jsm = jsmin.JavascriptMinify()
    for s in source:
        for t in target:
            jsm.minify(open(s.path), open(t.path,'w'))
	
Command( PHONEGAP_COMPRESSED, PHONEGAP_LIB, CommandCompressJS )

