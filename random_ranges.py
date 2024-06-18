#!/usr/bin/python


import numpy
import argparse

parser = argparse.ArgumentParser(description='Generates integers belong to ranges.')

parser.add_argument('--csv-file', dest='csv_file',
                    default='ips.csv',
                    help='filename for <range_begin>, <range_cardinality>')

parser.add_argument('--ips-file', dest='ips_file',
                    default='ips.data',
                    help='filename for ips')


NUM_RANGES = 10
RNG = 2**32
TOTAL_ADDRESSES = 1000

parser.add_argument('--num-ranges', dest='NUM_RANGES',
                    default=10,
                    type=int,
                    help='number of ranges')

parser.add_argument('--rng', dest='RNG',
                    default=2**32,
                    type=int,
                    help='maximum number')

parser.add_argument('--total-addresses', dest='TOTAL_ADDRESSES',
                    default=1000,
                    type=int,
                    help='number of IP addresses')


args = parser.parse_args()

ls = numpy.random.rand(args.NUM_RANGES) * args.RNG
# lengths of ranges

starts = numpy.ceil(numpy.sort(numpy.random.rand(args.NUM_RANGES) * args.RNG))
# starts of ranges

sum = numpy.sum(ls)
coeff = args.TOTAL_ADDRESSES / sum
rounded = numpy.round(ls * coeff)

start_length = numpy.concatenate([[starts], [rounded]], axis=0)
csv = start_length.T

prev_end = 0

#'<range_begin>, <range_cardinality>' per line
with open(args.csv_file, "w+") as csv_file:
    for arange in csv:
        print >>csv_file, "%d, %d" % (arange[0], arange[1])
        if arange[0] <= prev_end:
            raise Exception('overlap detected')
        prev_end = arange[1]

#IP per line        
with open(args.ips_file, "w+") as ips_file:
    for arange in csv:
        curval = arange[0]
        endval = arange[0] + arange[1]
        while curval < endval:
            print >>ips_file, "%d" % curval
            curval += 1
