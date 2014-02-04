#! /usr/bin/env python
import os
import glob
import time
import math

from optparse import OptionParser


parser = OptionParser()

parser.add_option('--files', metavar='F', type='string', action='store',
                  dest='files',
                  help='Input files')

parser.add_option('--outfile', metavar='F', type='string', action='store',
                  dest='outfile',
                  help='Output file')

parser.add_option('--productLabel', metavar='P', type='string', action='store',
                  dest='productLabel',
                  help='EDM product label')

parser.add_option('--instanceLabel', metavar='I', type='string', action='store',
                  dest='instanceLabel',
                  default='',
                  help='EDM product instance label, optional')


parser.add_option('--edmType', metavar='T', type='string', action='store',
                  dest='edmType',
                  default='std::vector<reco::PFCandidate>',
                  help='EDM type')

parser.add_option('--max', metavar='T', type='int', action='store',
                  dest='max',
                  default=None,
                  help='Max number of events')

(options, args) = parser.parse_args()

argv = []

if options.productLabel is None :
    print 'Must specify a product! Exiting'
    exit()

if options.files is None :
    print 'Must specify input files! Exiting'
    exit()

if options.outfile is None :
    print 'Must specify output file name! Exiting'
    exit()

import ROOT

import sys
from DataFormats.FWLite import Events, Handle

#files = glob.glob( options.files )
events = Events (options.files)

objsLabel = ( options.productLabel, options.instanceLabel )
objsHandle = Handle( options.edmType )

count = 0

fout = open( options.outfile, 'w')

for event in events:

    weight = 1.0
    if count % 10000 == 0  :
      print  '--------- Processing Event ' + str(count)
    count = count + 1

        
    event.getByLabel( objsLabel, objsHandle )

    
    for iobj in objsHandle.product() :
        s = '{0:25.10f} {1:25.10f} {2:25.10f} {3:25.10f}\n'.format( iobj.px(), iobj.py(), iobj.pz(), iobj.energy() )
        fout.write(s)
    fout.write( "#END\n" )


    if options.max is not None and count > options.max :
        break
    
