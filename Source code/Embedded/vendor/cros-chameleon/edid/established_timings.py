# Copyright 2014 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


"""Provides the EstablishedTimings class with methods for parsing info.

EstablishedTimings information is found in bytes 23h-25h of the base EDID.
"""

from . import tools


_timings = (
    (0x800000, '720x400 @ 70 Hz'),
    (0x400000, '720x400 @ 88 Hz'),
    (0x200000, '640x480 @ 60 Hz'),
    (0x100000, '640x480 @ 67 Hz'),
    (0x80000, '640x480 @ 72 Hz'),
    (0x40000, '640x480 @ 75 Hz'),
    (0x20000, '800x600 @ 56 Hz'),
    (0x10000, '800x600 @ 60 Hz'),
    (0x8000, '800x600 @ 72 Hz'),
    (0x4000, '800x600 @ 75 Hz'),
    (0x2000, '832x624 @ 75 Hz'),
    (0x1000, '1024x768 @ 87 Hz, interlaced (1024x768i)'),
    (0x800, '1024x768 @ 60 Hz'),
    (0x400, '1024x768 @ 72 Hz'),
    (0x200, '1024x768 @ 75 Hz'),
    (0x100, '1280x1024 @ 75 Hz'),
    (0x80, '1152x870 @ 75 Hz (Apple Macintosh II)'),
    (0x40, 'Manufacturer specific display mode 1'),
    (0x20, 'Manufacturer specific display mode 2'),
    (0x10, 'Manufacturer specific display mode 3'),
    (0x8, 'Manufacturer specific display mode 4'),
    (0x4, 'Manufacturer specific display mode 5'),
    (0x2, 'Manufacturer specific display mode 6'),
    (0x1, 'Manufacturer specific display mode 7'),
)


class EstablishedTimings(object):
  """Identify and return a list of supported Established Timings."""

  def __init__(self, edid):
    """Create an EstablishedTimings object.

    Args:
      edid: The EDID being analyzed.
    """
    self._edid = edid

  @property
  def supported_timings(self):
    """Create the list of supported timings.

    Returns:
      The dict of strings and bools indicating supported timings.
    """
    # Bytes 35, 36, 37
    timing_byte = 0
    timing_byte += self._edid[0x23] << 16
    timing_byte += self._edid[0x24] << 8
    timing_byte += self._edid[0x25]
    return tools.DictFilter(_timings, timing_byte)
