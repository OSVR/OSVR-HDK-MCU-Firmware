#!/usr/bin/env python3

# Copyright 2014 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

####################################################
# EDID Parser
# Takes in binary blob EDID
# This parser is primarily based on the following:
# 1. "VESA Enhanced Extended Display Identification Data Standard", Release A,
#   Revision 2, Sept. 25, 2006.
# 2. "A DTV Profile for Uncompressed High Speed Digital Interfaces",
#   ANSI/CEA-861-F, Aug., 2013.
# 3. HDMI spec.
####################################################

"""Parses EDID and establishes command line options for EDID analysis."""


import argparse
import re

import edid.data_block as data_block
import edid.descriptor as descriptor
import edid.edid as edid
import edid.extensions as extensions
import edid.tools as tools
import edid.video_block as video_block
from edid.tools import BytesFromFile, PrintHexData

LAYOUT_MODE = 0
NORMAL_MODE = 1
VERBOSE_MODE = 2


TYPE_ALL = 'all'
TYPE_BASE = 'base'
TYPE_VENDOR = 'vendor'
TYPE_BD = 'bd'
TYPE_DCC = 'dcc'
TYPE_ET = 'et'
TYPE_ST = 'st'
TYPE_DP = 'dp'
TYPE_XALL = 'xall'


RAW_OFF = 0
RAW_HEX = 1
RAW_DEC = 2


VALID_TYPES = set([TYPE_ALL, TYPE_BASE, TYPE_VENDOR, TYPE_BD, TYPE_DCC, TYPE_ET,
                   TYPE_ST, TYPE_DP, TYPE_XALL])


VALID_TYPE_MESSAGE = ('Type options include: all (all info), base (base block),'
                      ' vendor (vendor and product info, bd (basic display in '
                      'base block), dcc (display x, y chromaticity coordinates '
                      'in base block), et (established timings in base block), '
                      'st (standard timings in base block), dp (descriptors in '
                      'base block), x<n> (show nth extension), x-all (show all '
                      'extensions). Default: \'all\'.')

type_help_string = ('Types of information to print, listed as a single '
                    'string (command separated). %s' % VALID_TYPE_MESSAGE)


################
#   FUNCTIONS  #
###################

def PrintSpace(num=1):
  """Format print out nicely by adding space between sections.

  Args:
    num: Denotes a smaller space (num = 0) or regular 3-line space (num = 1).
  """
  if num == 1 or num == 2:
    print('\n\n\n')
  elif num == 0:
    print('\n')


def PrintList(alist, mode, print_format):
  """Print out a list of properties and their values in specified format.

  Args:
    alist: The list of tuples to print.
    mode: The level of verbosity for analysis.
    print_format: The string format for printing.
  """
  for x, s in alist:
    if mode == NORMAL_MODE and not s:
      continue
    print(print_format % (x, s))


def GetManufacturerInfo(e, mode, raw_mode):
  """Print and interpret the manufacturer information of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Manufacturing/vendor info]')

  if raw_mode:
    PrintRawRange(e.GetData(), raw_mode, 0x08, 0x12)

  if mode == LAYOUT_MODE:
    return

  info = [
      ['Manufacturer ID:', e.manufacturer_id],
      ['ID Product Code:', e.product_code],
      ['Serial number:', e.serial_number],
      ['Week of manufacture:', e.manufacturing_week],
      ['Year of manufacture:', e.manufacturing_year],
      ['Model year:', e.model_year]
  ]

  PrintList(info, mode, '  %-22s %s')


def GetBasicDisplay(e, mode, raw_mode):
  """Print and interpret the basic display information of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Basic Display Information]')

  if raw_mode:
    PrintRawRange(e.GetData(), raw_mode, 0x14, 0x19)

  if mode == LAYOUT_MODE:
    return

  bd = e.basic_display

  # Video input definition
  # 0 = Analog, 1 = Digital
  if bd.video_input_type:  # Digital
    print('Digital Video Signal Interface')

    dig_info = [
        ['Color Bit Depth:', bd.color_bit_depth],
        ['Digital Video Interface Standard Support:',
         bd.digital_supports]
    ]

    PrintList(dig_info, mode, '  %-50s %s')

  elif not bd.video_input_type:  # Analog
    print('Analog Video Signal Interface')

    analog_info = [
        ['Video white and sync levels:', bd.signal_level],
        ['Blank-to-black setup expected:', bd.blank_black],
        ['Separate sync supported:', bd.separate_sync],
        ['Composite sync (on HSync) supported:', bd.composite_sync],
        ['Sync on green supported:', bd.green_sync],
        ['VSync serrated when composite/sync-on-green used:',
         bd.vsync_pulse]
    ]

    PrintList(analog_info, mode, '  %-50s %s')

  # Shared basic display properties (both analog/digital)

  if not bd.horizontal_dim or not bd.vertical_dim:
    max_dim = None
  else:
    max_dim = '%d x %d' % (bd.horizontal_dim, bd.vertical_dim)

  pix_str = ('Preferred timing includes native timing pixel\n  %-50s'
             % '  format and refresh rate:')

  info = [
      ['Maximum dimensions (cm):', max_dim],
      ['Aspect ratio (portrait):', bd.aspect_ratio_portrait],
      ['Aspect ratio (landscape):', bd.aspect_ratio_landscape],
      ['Display gamma:', '%.2f' % bd.display_gamma],
      ['DPM standby supported:', bd.dpm_standby],
      ['DPM suspend supported:', bd.dpm_suspend],
      ['DPM active-off supported:', bd.active_off],
      ['Display color type:', bd.display_type],
      ['sRGB Standard is default colour space:', bd.srgb_as_default],
      [pix_str, bd.native_preferred_timing_mode]
  ]

  PrintList(info, mode, '  %-50s %s')

  cfs = bd.cont_freq_support
  if mode == VERBOSE_MODE and cfs:
    print('  %-50s %s' % ('Continuous frequency supported:', cfs))


def GetChromaticity(e, mode, raw_mode):
  """Print and interpret the chromaticity information of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Chromaticity information]')

  if raw_mode:
    PrintRawRange(e.GetData(), raw_mode, 0x19, 0x23)

  if mode == LAYOUT_MODE:
    return

  chrom = e.chromaticity

  info = [
      ('Red', chrom.red_x, chrom.red_y),
      ('Green', chrom.grn_x, chrom.grn_y),
      ('Blue', chrom.blue_x, chrom.blue_y),
      ('White', chrom.wht_x, chrom.wht_y)
  ]

  for x in info:
    print('%7s: (%3d, %3d)' % x)


def GetEstablishedTiming(e, mode, raw_mode):
  """Print and interpret the established timing information of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Established timing bitmap]')

  if raw_mode:
    PrintRawRange(e.GetData(), raw_mode, 0x23, 0x26)

  if mode == LAYOUT_MODE:
    return

  et = e.established_timings
  results = tools.ListTrueOnly(et.supported_timings)

  if not results:  # If empty list was returned
    print('  None')
  else:
    for r in results:

      if '@' in r:
        res, hz = r.split('@')
        print('  %-12s @%s' % (res, hz))

      else:
        print('  %-12s' % r)


def GetBaseStandardTiming(e, mode, raw_mode):
  """Print and interpret the standard timing information of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Standard timing information]')

  if raw_mode:
    PrintRawRange(e.GetData(), raw_mode, 0x26, 0x36)

  if mode == LAYOUT_MODE:
    return

  sts = e.standard_timings
  if sts:
    for st in sts:
      PrintSt(st)
  elif mode == VERBOSE_MODE:
    print('  None')


def PrintSt(st):
  """Print out information in a single standard_timings.StandardTiming object.

  Args:
    st: A standard_timings.StandardTiming object.
  """
  x_res = st.x_resolution
  rat = st.xy_pixel_ratio

  num, denum = list(map(int, rat.split(':')))

  y_res = (x_res * denum) / num
  freq = st.vertical_freq

  form_rat = '(%s)' % rat
  print('  %4d x %4d  %-8s @ %d Hz' % (x_res, y_res, form_rat, freq))


def GetDescriptorBlocks(e, mode, raw_mode):
  """Print and interpret the descriptor blocks information of an EDID.

  Calls PrintBlockAnalysis on each block for detailed print out.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  print('[Descriptor blocks 1-4]')

  descs = e.descriptors

  for x in range(0, len(descs)):

    prefix = 'Block #%d: ' % (x + 1)
    PrintBlockAnalysis(e, descs[x], mode, raw_mode, x, prefix)
    PrintSpace(mode)


def PrintBlockAnalysis(e, desc, mode, raw_mode, start, prefix=None):
  """Print and interpret a single 18-byte descriptor's information.

  Called up to 4 times in a base EDID.
  Uses descriptor module to determine descriptor type.

  Args:
    e: The full EDID being parsed.
    desc: The descriptor being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
    start: The start index of the descriptor.
    prefix: Optional string description of which (nth) descriptor this is within
        the EDID.
  """
  print('%s%s' % (prefix, desc.type))

  if mode == LAYOUT_MODE:

    if desc.type == descriptor.TYPE_DISPLAY_RANGE_LIMITS:
      print('  Subtype: %s' % desc.subtype)

  if raw_mode:
    base = 54
    PrintRawRange(e.GetData(), raw_mode, base + (start * 18),
                  base + ((start + 1) * 18))

  if mode == LAYOUT_MODE:
    return

  if desc.type in (descriptor.TYPE_PRODUCT_SERIAL_NUMBER,
                   descriptor.TYPE_ALPHANUM_DATA_STRING,
                   descriptor.TYPE_DISPLAY_PRODUCT_NAME):

    print('  Data string:\t%s' % desc.string)

  elif desc.type == descriptor.TYPE_DISPLAY_RANGE_LIMITS:

    print('Subtype:', desc.subtype)

    vert_rate = '%2d - %d' % (desc.min_vertical_rate,
                              desc.max_vertical_rate)
    hor_rate = '%2d - %d' % (desc.min_horizontal_rate,
                             desc.max_horizontal_rate)

    info = [
        ['Vertical rate (Hz):', vert_rate],
        ['Horizontal rate (kHz):', hor_rate],
        ['Pixel clock (MHz):', desc.pixel_clock]
    ]

    PrintList(info, mode, '  %-35s %s')

    if desc.subtype == descriptor.SUBTYPE_DISPLAY_RANGE_CVT:

      ss = []
      for ar in tools.ListTrueOnly(desc.supported_aspect_ratios):
        ss.append('  %-35s %s' % ('', ar))
      asp = '\n'.join(ss)

      ss = []
      for cb in tools.ListTrueOnly(desc.cvt_blanking_support):
        ss.append('  %-35s %s' % ('', cb))
      cvt_blank = '\n'.join(ss)

      ss = []
      for ds in tools.ListTrueOnly(desc.display_scaling_support):
        ss.append('  %-35s %s' % ('', ds))
      dis_scal = '\n'.join(ss)

      cvt_info = [
          ['CVT Version:', desc.cvt_version],
          ['Additional Pixel Clock:',
           '%s MHz' % desc.additional_pixel_clock],
          ['Maximum active pixels:', desc.max_active_pixels],
          ['Supported aspect ratios:', asp.strip()],
          ['Preferred aspect ratio:', desc.preferred_aspect_ratio],
          ['CVT blanking support:', cvt_blank.strip()],
          ['Display scaling support:', dis_scal.strip()],
          ['Preferred vertical refresh (Hz):', desc.preferred_vert_refresh]
      ]

      PrintList(cvt_info, mode, '  %-35s %s')

    elif desc.subtype == descriptor.SUBTYPE_DISPLAY_RANGE_2ND_GTF:

      gtf_info = [
          ['Start break frequency:', desc.start_break_freq],
          ['C:', desc.c],
          ['M:', desc.m],
          ['K:', desc.k],
          ['J:', desc.j]
      ]

      PrintList(gtf_info, mode, '  %-25s %s')

  elif desc.type == descriptor.TYPE_COLOR_POINT_DATA:
    cp_1 = desc.first_color_point
    cp_2 = desc.second_color_point

    PrintCp(cp_1, 1)
    PrintCp(cp_2, 2)

  elif desc.type == descriptor.TYPE_STANDARD_TIMING:

    sts = desc.standard_timings
    for st in sts:
      PrintSt(st)

  elif desc.type == descriptor.TYPE_DISPLAY_COLOR_MANAGEMENT:

    dcm_info = [
        ['Red a3:', desc.red_a3],
        ['Red a2:', desc.red_a2],
        ['Green a3:', desc.green_a3],
        ['Green a2:', desc.green_a2],
        ['Blue a3:', desc.blue_a3],
        ['Blue a2:', desc.blue_a2]
    ]

    PrintList(dcm_info, mode, '  %-12s %s')

  elif desc.type == descriptor.TYPE_CVT_TIMING:

    cvts = desc.coordinated_video_timings
    for cvt in cvts:
      PrintCvt(cvt)

  elif desc.type == descriptor.TYPE_ESTABLISHED_TIMINGS_III:

    print(tools.ListTrueOnly(desc.established_timings))

  elif desc.type == descriptor.TYPE_MANUFACTURER_SPECIFIED:
    if raw_mode:
      print(desc.GetBlob())

  elif desc.type == descriptor.TYPE_DETAILED_TIMING:
    PrintDtd(desc)


def PrintCp(cp, num):
  """Print out information about a single descriptor.ColorPoint object.

  Args:
    cp: A descriptor.ColorPoint object.
    num: The index of the object (1st or 2nd).
  """
  print('Color Point %d' % num)

  wht = '(%d, %d)' % (cp.white_x, cp.white_y)

  cp_info = [
      ['Index number:', cp.index_number],
      ['White point coordinates:', wht],
      ['Gamma:', str(cp.gamma) if cp.gamma else 'Gamma not described here']
  ]

  PrintList(cp_info, VERBOSE_MODE, '  %-30s %s')


def PrintDtd(desc):
  """Print out information about a single descriptor.DetailedTimingDescriptor.

  Used in the base EDID analysis as well as certain extensions (i.e., VTB).

  Args:
    desc: The descriptor.DetailedTimingDescriptor.
  """
  pix = '%.1f MHz' % (desc.pixel_clock)
  active = '%d x %d' % (desc.h_active_pixels, desc.v_active_lines)
  blank = '%d x %d' % (desc.h_blanking_pixels,
                       desc.v_blanking_lines)
  fp = '%d x %d' % (desc.h_sync_offset, desc.v_sync_offset)
  sp = '%d x %d' % (desc.h_sync_pulse, desc.v_sync_pulse)
  ds = '%d x %d' % (desc.h_display_size, desc.v_display_size)
  bord = '%d x %d' % (desc.h_border_pixels, desc.v_border_lines)

  info = [
      ['Pixel clock:', pix],
      ['Addressable:', active],
      ['Blanking:', blank],
      ['Front porch:', fp],
      ['Sync pulse:', sp],
      ['Image size (mm):', ds],
      ['Border:', bord],
      ['Interlace:', desc.interlaced],
      ['Stereo viewing:', desc.stereo_mode]
  ]

  PrintList(info, VERBOSE_MODE, '  %-17s %s')

  st = desc.sync_type
  print('  Sync type:')
  for x in st:
    print('    %-39s %s' % ('%s:' % x, st[x]))


def AnalyzeExtension(e, mode, raw_mode, block_num=1):
  """Print and interpret an extension of an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
    block_num: The index of the extension being analyzed (default: 1st ext).
  """
  ext = e.GetExtension(block_num)
  print('EXTENSION NUMBER %d: %s' % (block_num, ext.type))

  if mode == VERBOSE_MODE:
    print('Tag: %d' % ext.tag)

  if ext.type == extensions.TYPE_CEA_861:

    dbs = ext.data_blocks
    dtds = ext.dtds

    if mode == LAYOUT_MODE:
      print('Number of data blocks: %d' % len(dbs))
      for x in range(0, len(dbs)):
        print('%d. %s' % (x + 1, dbs[x].type))

        # Calls PrintRawRange, but it'll only print if raw_mode on
        PrintRawRange(dbs[x].GetBlock(), raw_mode)

      PrintSpace(mode)

      print('Number of detailed timing descriptors: %d' % len(dtds))
      for x in range(0, len(dtds)):
        print('%d. %s' % (x + 1, dtds[x].type))

        # Calls PrintRawRange, but it'll only print if raw_mode on
        PrintRawRange(dtds[x].GetBlock(), raw_mode)

      PrintSpace(mode)

      return

    # BASIC CEA INFO

    if raw_mode:
      cea_base = ext.GetBlock()[0:4]
      PrintRawRange(cea_base, raw_mode)

    cea_info = [
        ['Version:', ext.version],
        ['Underscan support:', ext.underscan_support],
        ['Basic audio support:', ext.basic_audio_support],
        ['YCbCr 4:4:4 support:', ext.ycbcr444_support],
        ['YCbCr 4:2:2 support:', ext.ycbcr422_support],
        ['Native DTD count:', ext.native_dtd_count]
    ]

    PrintList(cea_info, mode, '  %-23s %s')

    PrintSpace(mode)

    # DATA BLOCKS

    for db in dbs:

      print('Data Block %s' % db.type)

      if raw_mode:
        PrintRawRange(db.GetBlock(), raw_mode)

      db_basic = [
          ['Tag:', db.tag],
          ['Length:', db.length],
          ['Extension tag:', db.ext_tag],
      ]

      if mode == VERBOSE_MODE:
        PrintList(db_basic, mode, '  %-19s %s')
        print('\n')

      if db.type in (data_block.DB_TYPE_VIDEO,
                     data_block.DB_TYPE_YCBCR420_VIDEO):
        svds = db.short_video_descriptors
        for svd in svds:
          print('  %-20s%s' % (svd.nativity, video_block.GetSvd(svd.vic)))

      elif db.type == data_block.DB_TYPE_AUDIO:
        ads = db.short_audio_descriptors
        for x in range(0, len(ads)):

          ad = ads[x]
          print('Short audio descriptor #%d' % (x + 1))

          ssf = tools.ListTrueOnly(ad.supported_sampling_freqs)

          ad_basic = [
              ['Type:', ad.type],
              ['Max channel count:', ad.max_channel_count],
              ['Supported sampling:', '  '.join(ssf)],
          ]

          if mode == VERBOSE_MODE:
            ad_basic.insert(0, ['Format code:', ad.format_code])

          PrintList(ad_basic, mode, '  %-19s %s')

          if ad.type == data_block.AUDIO_TYPE_LPCM:
            print('  %-19s %s' % ('Bit depth:',
                                  tools.ListTrueOnly(ad.bit_depth)))
          elif ad.type == data_block.AUDIO_TYPE_DRA:
            print('  %-19s %s' % ('DRA value:', ad.value))
          elif ad.format_code <= 8 and ad.format_code >= 2:
            print('  %-19s %s' % ('Max bit rate:', ad.max_bit_rate))
          elif ad.format_code <= 14 and ad.format_code <= 9:
            print('  %-19s %s' % ('Value:', ad.value))
          else:
            print('  %-19s %s' % ('Extension code:', ad.ext_code))
            print('  %-19s %s' % ('Frame length:', ad.frame_length))
            print('  %-19s %s' % ('MPS support:', ad.mps_support))

          print('\n')

      elif db.type == data_block.DB_TYPE_SPEAKER_ALLOCATION:

        print('  Speaker allocation:')
        for a in tools.ListTrueOnly(db.allocation):
          print('    %s' % a)

      elif db.type in (data_block.DB_TYPE_VENDOR_SPECIFIC,
                       data_block.DB_TYPE_VENDOR_SPECIFIC_AUDIO,
                       data_block.DB_TYPE_VENDOR_SPECIFIC_VIDEO):
        print('  %-20s %s' % ('IEEE:', db.ieee_oui))
        print('  %-20s %s' % ('Data payload:', db.payload))

      elif db.type == data_block.DB_TYPE_COLORIMETRY:

        print('  Colorimetry:')
        for c in tools.ListTrueOnly(db.colorimetry):
          print('    %s' % c)
        print('  %-20s %s' % ('Metadata:', db.metadata))

      elif db.type == data_block.DB_TYPE_VIDEO_CAPABILITY:

        vc_info = [
            ['YCC Quantization range:',
             db.selectable_quantization_range_ycc],
            ['RGB Quantization range:',
             db.selectable_quantization_range_rgb],
            ['PT behavior:', db.pt_behavior],
            ['IT behavior:', db.it_behavior],
            ['CE behavior:', db.ce_behavior]
        ]

        PrintList(vc_info, mode, '  %-35s %s')

      elif db.type == data_block.DB_TYPE_INFO_FRAME:

        if_proc = db.if_processing
        vsifs = db.vsifs
        print('  %-25s %s' % ('VSIF count:', len(vsifs)))

        if if_proc.payload:
          print('  %-25s %s' % ('InfoFrame Processing Descriptor Payload:',
                                if_proc.payload))

        for vsif in vsifs:

          if mode == NORMAL_MODE:
            v_type = vsif.type
            print('  %-25s %s' % ('Type:', v_type))
            if v_type == data_block.INFO_FRAME_TYPE_VENDOR_SPECIFIC:
              print('  %-25s %s' % ('IEEE:', vsif.ieee_oui))
            continue

          vsif_info = [
              ['Type code:', vsif.type_code],
              ['Type:', vsif.type],
              ['Payload length:', vsif.payload_length],
              ['Data payload:', vsif.payload]
          ]

          PrintList(vsif_info, mode, '  %-25s %s')

          if vsif.type == data_block.INFO_FRAME_TYPE_VENDOR_SPECIFIC:
            print('  %-25s %s' % ('IEEE:', vsif.ieee_oui))

      elif db.type == data_block.DB_TYPE_YCBCR420_CAPABILITY_MAP:
        sdis = db.supported_descriptor_indices
        for sdi in sdis:
          print(sdi)

      elif db.type == data_block.DB_TYPE_VIDEO_FORMAT_PREFERENCE:
        vps = db.video_preferences
        for vp in vps:
          if vp.type == data_block.VIDEO_PREFERENCE_VIC:
            print(video_block.GetSvd(vp.vic))
          elif vp.type == data_block.VIDEO_PREFERENCE_DTD:
            print('%d-th DTD in EDID' % vp.dtd_index)
          elif vp.type == data_block.VIDEO_PREFERENCE_RESERVED:
            print('Reserved: %d' % vp.svr)

      PrintSpace()

    # DETAILED TIMING DESCRIPTORS

    for x in range(0, len(dtds)):
      print('Detailed Timing Descriptor #%d:' % (x + 1))

      if raw_mode:
        PrintRawRange(dtds[x].GetBlock(), raw_mode)

      PrintDtd(dtds[x])
      PrintSpace(mode)

  elif ext.type == extensions.TYPE_VIDEO_TIMING_BLOCK:

    dtbs = ext.dtbs
    cvts = ext.cvts
    sts = ext.sts

    if mode == LAYOUT_MODE:
      print('Number of detailed timing descriptors: %d' % len(dtbs))
      for x in range(0, len(dtbs)):
        print('%d. %s' % (x + 1, dtbs[x].type))
        this_dtb = dtbs[x].GetBlock()
        PrintRawRange(this_dtb, raw_mode)

      print('Number of coordinated video timing blocks: %d' % len(cvts))
      for x in range(0, len(cvts)):
        print('%d. Coordinated Video Timing Block' % (x + 1))
        this_cvt = cvts[x].GetBlock()
        PrintRawRange(this_cvt, raw_mode)

      print('Number of standard timing blocks: %d' % len(sts))
      for x in range(0, len(sts)):
        print('%d. Standard Timing Block' % (x + 1))
        this_st = sts[x].GetBlock()
        PrintRawRange(this_st, raw_mode)

      return

    if raw_mode:
      vtb_base = ext.GetBlock()[0:4]
      PrintRawRange(vtb_base, raw_mode)

    ext_basic = [
        ['Version:', ext.version],
        ['Number of DTBs:', ext.dtb_count],
        ['Number of CVTs:', ext.cvt_count],
        ['Number of STs:', ext.st_count]
    ]

    PrintList(ext_basic, mode, '%-35s %s')

    for dtb in dtbs:

      print('\n\nDTD')
      if raw_mode:
        PrintRawRange(dtb.GetBlock(), raw_mode)
      PrintDtd(dtb)

    for cvt in cvts:
      print('\n\nCVT')
      if raw_mode:
        PrintRawRange(cvt.GetBlock(), raw_mode)
      PrintCvt(cvt)

    for st in sts:
      print('\n\nSTs')
      if raw_mode:
        PrintRawRange(st.GetBlock(), raw_mode)
      PrintSt(st)

  elif ext.type == extensions.TYPE_EXTENSION_BLOCK_MAP:

    if raw_mode:
      PrintRawRange(ext.GetBlock(), raw_mode)

    if mode == LAYOUT_MODE:
      return

    tags = ext.all_tags

    for x in range(0, len(tags)):
      if tags[x]:  # Not 0, bc 0 indicates unused block
        print('Block %d: %d' % (x + block_num + 1, tags[x]))


def PrintCvt(cvt):
  """Print out information about a single CoordinatedVideoTiming object.

  Full object name: coordinated_video_timings.CoordinatedVideoTiming.

  Args:
    cvt: A single CoordinatedVideoTiming object.
  """
  svr = tools.ListTrueOnly(cvt.supported_vertical_rates)

  cvt_info = [
      ['Active vertical lines:', cvt.active_vertical_lines],
      ['Aspect ratio:', cvt.aspect_ratio],
      ['Preferred refresh rate:', cvt.preferred_vertical_rate],
      ['Supported refresh rates:', '  '.join(svr)]
  ]

  if cvt:
    PrintList(cvt_info, VERBOSE_MODE, '  %-35s %s')
  else:
    print('UNUSED FIELD')


def PrintBase(e, mode, raw_mode, types):
  """Print and interpret all information of the base EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
    types: The list of types being analyzed.
  """
  print('BASE EDID:')
  if TYPE_BASE in types:  # If TYPE_ALL was specified, BASE would be added in
    types = [TYPE_VENDOR, TYPE_BD, TYPE_DCC, TYPE_ET, TYPE_ST, TYPE_DP]

  # The following are no longer mutually exclusive
  if TYPE_VENDOR in types:
    # Note: Vendor info is very brief; no consideration for modes
    GetManufacturerInfo(e, mode, raw_mode)
    PrintSpace()
  if TYPE_BD in types:
    GetBasicDisplay(e, mode, raw_mode)
    PrintSpace()
  if TYPE_DCC in types:
    GetChromaticity(e, mode, raw_mode)
    PrintSpace()
  if TYPE_ET in types:
    GetEstablishedTiming(e, mode, raw_mode)
    PrintSpace()
  if TYPE_ST in types:
    GetBaseStandardTiming(e, mode, raw_mode)
    PrintSpace()
  if TYPE_DP in types:
    GetDescriptorBlocks(e, mode, raw_mode)
    PrintSpace()


def PrintExtensions(e, mode, raw_mode, exts):
  """Print and interpret all information of one or more extensions.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
    exts: The list of extensions to be analyzed.
  """
  if TYPE_XALL in exts:
    GetXall(e, mode, raw_mode)

  else:
    for ext in exts:
      block_num = int(ext[1:])
      AnalyzeExtension(e, mode, raw_mode, block_num)
      PrintSpace()


def GetXall(e, mode, raw_mode):
  """Print and interpret the information of all extensions to an EDID.

  Args:
    e: The EDID being parsed.
    mode: The level of verbosity for analysis.
    raw_mode: The type of raw data print out, if any.
  """
  num_ext = e.extension_count

  for x in range(1, num_ext + 1):
    AnalyzeExtension(e, mode, raw_mode, x)
    PrintSpace(mode)


def PrintRawRange(e, raw_mode, start=0, end=None):
  """Print the raw data of a section of an EDID.

  If no arguments given for start and end, the whole list is printed.

  Args:
    e: The list of bytes being printed.
    raw_mode: The type of raw data print out - hex, decimal, or none.
    start: The index of the first byte to print.
    end: The index of the last byte NOT to print.
  """
  data = e[start:end]

  if raw_mode:
    if raw_mode == RAW_HEX:
      my_format = '  Byte 0x%02X:\t0x%02X'
    else:  # Decimal
      my_format = '  Byte %04d:\t%04d'

    for x in range(0, len(data)):
      print(my_format % (x + start, data[x]))

    print('\n')


def PrintHexEdid(e):
  """Print the entire EDID in hexadecimal form.

  Args:
    e: The EDID to be printed.
  """
  PrintHexData(e.GetData())


def PrintDecEdid(e):
  """Print the entire EDID in decimal form.

  Args:
    e: The EDID to be printed.
  """
  # Decimal
  data = e.GetData()
  dec_rows = len(data) // 8
  for x in range(0, dec_rows):

    start = 8 * x
    row = '%04d %04d  ' * 4 % tuple(data[start: start + 8])
    line_range = '%d-%d:' % (start, start + 7)
    print('%9s\t%s' % (line_range, row))


def Verify(e):
  """Error checks the EDID.

  Args:
    e: The EDID for error checking.
  """
  errors = e.GetErrors()

  print('Found %d errors\n\n' % len(errors))

  # Can format this better later
  for x in range(0, len(errors)):
    print('ERROR %d' % (x + 1))
    print(errors[x].location)
    print(errors[x].message)

    if errors[x].expected:
      print('\tExpected\t%s' % errors[x].expected)
    if errors[x].found:
      print('\tFound\t\t\t%s' % errors[x].found)

    PrintSpace(0)


def Version(e):
  """Print the EDID version.

  Args:
    e: The EDID being analyzed.
  """
  print('EDID version: %s' % e.edid_version)


def Xc(e):
  """Print the number of extensions (extension count).

  Args:
    e: The EDID being analyzed.
  """
  print('Extension count: %d' % e.extension_count)


def CheckInvalidTypes(types, exts):
  """Check the types listed for the parse subcommand for validity.

  Args:
    types: The list of strings indicating sections of base EDID to parse.
    exts: The list of strings indicating sections of extensions to parse.
  """
  for t in types:
    if t not in VALID_TYPES and t not in exts:
      print(('Error: %s is not a valid type. %s')
             % (t, VALID_TYPE_MESSAGE))
      PrintSpace()


def ParseEdid():
  """Parse an EDID and print its info according to commands and flags."""
  p = argparse.ArgumentParser(description='Select sections of EDID to parse.')

  sp = p.add_subparsers(title='subcommands', description='valid subcommands',
                        metavar='')
  sp_verify = sp.add_parser('verify', help='Error check the EDID')
  sp_verify.set_defaults(func=Verify)
  sp_version = sp.add_parser('version', help='Print EDID version')
  sp_version.set_defaults(func=Version)
  sp_hex = sp.add_parser('hex', help='Print full EDID in hex')
  sp_hex.set_defaults(func=PrintHexEdid)
  sp_dec = sp.add_parser('dec', help='Print full EDID in decimal')
  sp_dec.set_defaults(func=PrintDecEdid)
  sp_xc = sp.add_parser('xc', help='Print extension count')
  sp_xc.set_defaults(func=Xc)
  sp_parse = sp.add_parser('parse', help='Parse full or sections of EDID.'
                           ' Run \'./edidparser.py parse -h\' for more info '
                           'on additional arguments.')

  # The following are all arguments for the parse subcommand
  parse_mode = sp_parse.add_mutually_exclusive_group()
  parse_mode.add_argument('-v', '--verbose', action='store_true',
                          help='Show detailed information')
  parse_mode.add_argument('-n', '--normal', action='store_true',
                          help='Show standard information')
  parse_mode.add_argument('-l', '--layout', action='store_true',
                          help='Show basic layout information')

  parse_raw = sp_parse.add_mutually_exclusive_group()
  parse_raw.add_argument('--hex', action='store_true',
                         help='Print raw data for each section in hex')
  parse_raw.add_argument('--dec', action='store_true',
                         help='Print raw data for each section in decimal')

  sp_parse.add_argument('-t', '--types', type=str, help=type_help_string)

  # Positional arguments: 1) EDID name
  p.add_argument('edid_name', type=str,
                 help='Name of EDID binary blob for parsing')

  args = p.parse_args()

  # Fill the edid list with bytes from binary blob
  e = edid.Edid(BytesFromFile(args.edid_name))

  print('Parsing %s' % args.edid_name)

  if hasattr(args, 'func'):  # Not the 'parse' subcommand
    args.func(e)
    exit()

  # Set defaults here
  mode = NORMAL_MODE
  raw_mode = RAW_OFF

  if args.layout:  # Layout of EDID only
    # Here, set the mode to layout mode
    # Run each analysis method but get back basic info only
    mode = LAYOUT_MODE

  elif args.verbose:
    # Here, set the mode to verbose mode
    mode = VERBOSE_MODE

  # Otherwise, mode remains NORMAL_MODE

  if args.dec:
    raw_mode = RAW_DEC

  elif args.hex:
    raw_mode = RAW_HEX

  if not args.types or TYPE_ALL in args.types.split(','):
    base_types = [TYPE_BASE]
    ext_types = [TYPE_XALL]

  else:
    base_types = args.types.split(',')
    regex = re.compile('(x).*')
    ext_types = [m.group(0) for t in base_types for m in [regex.search(t)] if m]
    CheckInvalidTypes(base_types, ext_types)

  print('EDID version: %s' % e.edid_version)
  PrintSpace()

  PrintBase(e, mode, raw_mode, base_types)
  PrintExtensions(e, mode, raw_mode, ext_types)


####################
# CODE STARTS HERE #
####################
if __name__ == '__main__':
  ParseEdid()
